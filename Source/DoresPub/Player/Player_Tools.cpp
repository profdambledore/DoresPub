// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_Tools.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Player/Player_Character.h"
#include "Player/Player_Controller.h"
#include "Player/Player_BuildToolDisplay.h"
#include "World/World_BuildingLevel.h"
#include "UI/UI_Player_Build.h"
#include "UI/UI_Player_Master.h"
#include "Data/BuildToolData.h"
#include "UI/UI_Tools_BuildWidget.h"

// Sets default values
APlayer_Tools::APlayer_Tools()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = Root;

	// Build Tool Components
	BuildToolWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Build Tool Widget"));
	BuildToolWidgetComponent->SetGenerateOverlapEvents(false);
	BuildToolWidgetComponent->SetVisibility(false, false);
	BuildToolWidgetComponent->SetupAttachment(Root, "");

	// Object Tool Components
	ObjectToolMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Object Tool Mesh"));
	ObjectToolMeshComponent->SetupAttachment(Root, "");

	// Also find and store the build tools display class
	static ConstructorHelpers::FClassFinder<APlayer_BuildToolDisplay>BTDClass(TEXT("/Game/Player/BP_BuildToolDisplay"));
	if (BTDClass.Succeeded()) {
		BuildToolDisplayClass = BTDClass.Class;
	}
}

// Called when the game starts or when spawned
void APlayer_Tools::BeginPlay()
{
	Super::BeginPlay();

	// Complete the pointer to the Build Tool Widget
	BTW = Cast<UUI_Tools_BuildWidget>(BuildToolWidgetComponent->GetWidget());

	// Spawn the BuildToolDisplay class
	if (BuildToolDisplayClass) {
		BTD = GetWorld()->SpawnActor<APlayer_BuildToolDisplay>(BuildToolDisplayClass);
	}

	// TEMP -- Find the ground floor and store a pointer to it
	GroundFloor = Cast<AWorld_BuildingLevel>(UGameplayStatics::GetActorOfClass(GetWorld(), AWorld_BuildingLevel::StaticClass()));
	
}

// Called every frame
void APlayer_Tools::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check which tool the player is in
	// If the current tool is the Build tool, then...
	if (CurrentTool == EToolType::Building) {
		// Fire a trace to the mouse's position, updating the tools location where the trace hits if it is inside of the WorldBounds
		// Also snap it to the building snapping distance (half of a wall size)
		FHitResult TickTrace = FireTraceToActor();
		FVector MouseLocation = TickTrace.Location;
		
		// Branch off based on if the sub-tool uses a draw method (wall, floor) or replace method (window, door)
		// Start by checking if the sub-tool is one of the replace methods
		if (BTD->GetSubTool() == Window) {
			MouseLocation.X = GetNearestMultiple(MouseLocation.X, PC->GetCurrentGridSnapValue());
			MouseLocation.Y = GetNearestMultiple(MouseLocation.Y, PC->GetCurrentGridSnapValue());
			MouseLocation.Z = 1.0f;

			BTD->GenerateBuildDisplay(MouseLocation);
			SetActorLocation(MouseLocation);
			UpdateToolRotation();

			// Next, check if a mesh has been set via GetDisplayWallsInUse
			// If this doesn't return empty, then it is in use
			if (BTD->GetDisplayWallsInUse() != 0) {
				// Start by checking that an actor was actually hit
				if (TickTrace.GetActor()) {
					// Then check that the trace hit a World_BuildingLevel
					if (TickTrace.GetActor()->IsA<AWorld_BuildingLevel>()) {
						// Next check if the component hit is a StaticMeshComponent
						if (TickTrace.Component.Get()->IsA<UStaticMeshComponent>()) {
							// If it is a StaticMeshComponent, then check if it matches the previous hit SMC
							if (TickTrace.GetComponent() != LastHitSMC) {
								// First, reset the LastHitSMC if required
								if (LastHitSMC) {
									if (LastHitSMC->GetStaticMesh() != LastHitMesh) {
										LastHitSMC->SetStaticMesh(LastHitMesh);
									}
								}
								LastHitSMC = Cast<UStaticMeshComponent>(TickTrace.GetComponent());
								LastHitMesh = LastHitSMC->GetStaticMesh();

								// Compare the LastHitMesh to the Wall mesh
								// Must match the default wall mesh to be suitable (at this time, could change in the future)
								if (LastHitMesh == WallDataTable->FindRow<FSelectableWallData>("wall_default", "")->Mesh) {
									// If it does match, hide the BTD, set the hit SMC to the window mesh
									BTD->UpdateDisplayValidity(2);
									LastHitSMC->SetStaticMesh(BTD->SelectedMesh);

									// Update the BuildToolWidget to display the selected window's price
									BTW->UpdateTextVisibility(true);
									BTW->UpdateDisplayedText(-1, 0, WindowDataTable->FindRow<FSelectableWindowData>(BTD->SelectedID, "")->Price);
								}
								else {
									// If it doesn't match, invalidate the SMC
									BTD->UpdateDisplayValidity(false);
									BTW->UpdateTextVisibility(false);
								}
							}
						}
					}
					else {
						// If we don't hit a StaticMeshComponent, clear LastHitSMC and LastHitMesh
						// Also, invalidate the BTD
						if (LastHitSMC) {
							if (LastHitSMC->GetStaticMesh() != LastHitMesh) {
								LastHitSMC->SetStaticMesh(LastHitMesh);
							}
						}
						LastHitSMC = nullptr;
						LastHitMesh = nullptr;
						BTD->UpdateDisplayValidity(false);
						BTW->UpdateTextVisibility(false);
					}
				}						
			}
		}
		// Else, the selected sub-tool is a draw method
		else {
			MouseLocation.X = GetNearestMultiple(MouseLocation.X, 250);
			MouseLocation.Y = GetNearestMultiple(MouseLocation.Y, 250);
			MouseLocation.Z = 1.0f;

			SetActorLocation(MouseLocation);

			// Update the LastPosition and call any related functions
			if (MouseLocation != LastPosition) {
				LastPosition = MouseLocation;
				UpdateToolRotation();

				// If the ClickPosition is valid (not (-1, -1, -1), then update the BTD
				if (ClickPosition != FVector(-1, -1, -1)) {
					BTD->GenerateBuildDisplay(ClickPosition, LastPosition);

					// Continue based on the selected sub-tool
					if (BTD->GetSubTool() == Wall) {
						// Check if we are in erase mode
						if (!bInEraseMode) {
							// Check that there is a wall ID selected
							if (BTD->SelectedID != "") {
								// Calculate the cost
								int cost;  cost = BTD->GetDisplayWallsInUse() * WallDataTable->FindRow<FSelectableWallData>(BTD->SelectedID, "")->Price;

								// Update the BTW with the new price
								BTW->UpdateDisplayedText(BTD->GetDisplaySize().first, BTD->GetDisplaySize().second, cost);

								// Check if it is valid (money)
								if (cost <= PC->GetCurrentMoney()) {
									// Next, check if the MouseLocation is outside the zone.
									if (!PC->GetIsPointInsideBound(MouseLocation)) {
										BTD->UpdateDisplayValidity(false);
									}
									else {
										BTD->UpdateDisplayValidity(true);
									}
								}
								else {
									BTD->UpdateDisplayValidity(false);
								}
							}
						}
						else {
							// Calculate the cost
							int cost = 0;

							// For each display data returned
							for (FBuildToolData i : BTD->GetDisplayData()) {
								// Check if the id of the data is valid.  If so...
								// TO:DO - World_BuildingLevel gets a function to return the ID of the wall at a position and rotation
								// Work in same way as GetWallObjectMeshAtPosition but returns FName/FString whatever is used in BuildData
								// Dont use ID from i directly as this will not be correct
								if (i.ID != "") {
									// Add to the total cost of the refund
									cost += WallDataTable->FindRow<FSelectableWallData>(i.ID, "")->Price * RefundMultiplier;
								}
							}

							// Update the BTW with the new price
							BTW->UpdateDisplayedText(BTD->GetDisplaySize().first, BTD->GetDisplaySize().second, cost);

							// Next, check if the MouseLocation is outside the zone.
							if (!PC->GetIsPointInsideBound(MouseLocation)) {
								BTD->UpdateDisplayValidity(false);
							}
							else {
								BTD->UpdateDisplayValidity(true);
							}
						}
					}
					// Continue based on the selected sub-tool
					else if (BTD->GetSubTool() == Floor) {
						int cost;  cost = 0; //BTD->GetDisplayWallsInUse() * WallDataTable->FindRow<FSelectableWallData>(BTD->SelectedWallID, "")->Price;

						// Update the BTW with the new price
						BTW->UpdateDisplayedText(BTD->GetDisplaySize().first, BTD->GetDisplaySize().second, cost);

						// Check if it is valid (money)
						if (cost <= PC->GetCurrentMoney()) {
							// Next, check if the MouseLocation is outside the zone.
							if (!PC->GetIsPointInsideBound(MouseLocation)) {
								BTD->UpdateDisplayValidity(false);
							}
							else {
								BTD->UpdateDisplayValidity(true);
							}
						}
						else {
							BTD->UpdateDisplayValidity(false);
						}
					}
				}
			}
		}
		
	}
	// If the current tool is the Object tool, then...
	else if (CurrentTool == EToolType::Object) {
		// Check what mode the Object tool is in
		// If they are in Normal Mode
		if (!bInRotationMode) {
			// Fire a trace to the mouse's position, updating the tools location where the trace hits if it is inside of the WorldBounds
			// Also snap it to the building snapping distance (half of a wall size)
			FVector MouseLocation = FireTraceToActor().Location;

			MouseLocation.X = GetNearestMultiple(MouseLocation.X, PC->GetCurrentGridSnapValue());
			MouseLocation.Y = GetNearestMultiple(MouseLocation.Y, PC->GetCurrentGridSnapValue());
			MouseLocation.Z = 1.0f;
			SetActorLocation(MouseLocation);

			// Update the LastPosition and call any related functions
			if (MouseLocation != LastPosition) {
				LastPosition = MouseLocation;

				// Set the ObjectToolMeshComponent to the MouseLocation
			}
		}
		// Else, if they are in Rotation Mode
		else {
			FRotator ObjectRotation = UKismetMathLibrary::FindLookAtRotation(ObjectToolMeshComponent->GetComponentLocation(), FireTraceToActor().Location);

			// Snap the rotation's yaw to the bounds, while also nullifying the roll and pitch
			ObjectRotation.Yaw = GetNearestMultiple(ObjectRotation.Yaw, PC->GetCurrentRotationSnapValue());
			ObjectRotation.Pitch = 0.0f;
			ObjectRotation.Roll = 0.0f;

			// Then rotate the mesh
			ObjectToolMeshComponent->SetWorldRotation(ObjectRotation);
		}
	}
}

/// -- Global Tool Functions --
// Called to set the Player Character pointer
void APlayer_Tools::SetupTools(APlayer_Character* NewPC)
{
	PC = NewPC;
	UpdateToolRotation();

	// Setup the Build Tool
	// Update the SelectableWallTileView
	for (FName i : WallDataTable->GetRowNames()) {
		FSelectableWallData* d = WallDataTable->FindRow<FSelectableWallData>(i, "");
		if (d->bManualPlacing) {
			PC->GetUI()->BuildState->AddSelectableWallToList(i, *d);
		}
	}

	// Update the SelectableWindowTileView
	for (FName j : WindowDataTable->GetRowNames()) {
		FSelectableWindowData* c = WindowDataTable->FindRow<FSelectableWindowData>(j, "");
		PC->GetUI()->BuildState->AddSelectableWindowToList(j, *c);
	}
}

// Called to swap to a selected tool
void APlayer_Tools::SwapTool(TEnumAsByte<EToolType> NewTool)
{
	// Sets the current too to the new tool
	CurrentTool = NewTool;

	// Then reset required properties to their default (the click position, then clear the BTD and hide the Build Tool's widget
	ClickPosition = FVector(-1, -1, -1);
	BTD->ClearBuildDisplay();
	BuildToolWidgetComponent->SetVisibility(false, false);

	// Update what components are no longer hidden based on the new tool
	// If the tool is now the Building tool, then
	if (CurrentTool == EToolType::Building) {
		// Make the widget visible
		BuildToolWidgetComponent->SetVisibility(true, false);
	}
	// Else, if the tool is now the object tool, then
	else if (CurrentTool == EToolType::Object) {
		// Make the object tool mesh visible
		ObjectToolMeshComponent->SetVisibility(true, false);
	}
}

// Called to update the rotation of the tool class to re-face the player's camera
void APlayer_Tools::UpdateToolRotation()
{
	// Update the rotation of the tool class to re - face the player's camera
	BuildToolWidgetComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PC->Camera->GetComponentLocation()));
	BuildToolWidgetComponent->SetWorldRotation(FRotator(0.0f, BuildToolWidgetComponent->GetComponentRotation().Yaw, 0.0f));
}

// Called to fire the primary tool function of the selected tool
// On first press, fire the pressed event.  On second press, fire the released event
void APlayer_Tools::SelectedToolPrimary()
{
	bPrimaryHeld = !bPrimaryHeld;

	if (bPrimaryHeld) {
		SelectedToolPrimaryPressed();
	}
	else {
		SelectedToolPrimaryReleased();
	}
}

// Called to fire the primary tool press function of the selected tool
void APlayer_Tools::SelectedToolPrimaryPressed()
{
	// If the current tool is the BuildTool, then...
	if (CurrentTool == EToolType::Building) {
		// Check where the player has clicked is inside of the WorldBounds
		FVector testClickPos = FireTraceToActor().Location;
		if (PC->GetIsPointInsideBound(testClickPos)) {
			// If it is, then round it to the Building Bounds and start "drawing" with the BuildToolDisplay 
			testClickPos.X = GetNearestMultiple(testClickPos.X, 250);
			testClickPos.Y = GetNearestMultiple(testClickPos.Y, 250);
			testClickPos.Z = 1.0f;
			ClickPosition = testClickPos;
			BTD->GenerateBuildDisplay(ClickPosition, ClickPosition);

			BTW->UpdateTextVisibility(true);
		}
	}
}

// Called to fire the primary tool release function of the selected tool
void APlayer_Tools::SelectedToolPrimaryReleased()
{
	// If the current tool is the BuildTool, then...
	if (CurrentTool == EToolType::Building) {
		// Check where the player has clicked is inside of the WorldBounds
		FVector testClickPos = FireTraceToActor().Location;
		if (PC->GetIsPointInsideBound(testClickPos)) {
			// Get the display data
			TArray<FBuildToolData> data = BTD->GetDisplayData();

			// Continue based on the selected sub-tool
			if (BTD->GetSubTool() == Wall) {
				
				// Check if there is some data in the display mode
				if (data.Num() > 0) {
					// Check what mode they are in
					if (!bInEraseMode) {
						// Get the cost of a single wall (as you can only ever place one type of wall at a time, just check the ID of the first index and get the cost)
						int cost = WallDataTable->FindRow<FSelectableWallData>(data[0].ID, "")->Price;
						// Check if they have enough money for the building
						int total = BTD->GetDisplayWallsInUse() * cost;
						if (total <= PC->GetCurrentMoney()) {
							// If they do, then 
							GroundFloor->AddBuildingObjects(data);
							PC->UpdateMoney(-total);
						}
					}
					else {
						// Remove the Walls from the WorldBuildingLevel
						TArray<FName> wallsToRefund = GroundFloor->RemoveBuildingObjects(data);
						int total = 0;
						for (FName w : wallsToRefund) {
							total += WallDataTable->FindRow<FSelectableWallData>(w, "")->Price * RefundMultiplier;
						}
						PC->UpdateMoney(total);
					}
				}
			}
			else if (BTD->GetSubTool() == Floor) {
				// Check if there is some data in the display mode
				if (data.Num() > 0) {
					// Check what mode they are in
					if (!bInEraseMode) {
						// If they are, then add the floors to the world
						GroundFloor->AddBuildingObjects(data);
					}
					else {
						GroundFloor->RemoveBuildingObjects(data);
					}
				}
			}
			else if (BTD->GetSubTool() == Window) {
				// Check that there is valid LastHitSMC
				if (LastHitSMC) {
					// Check if a wall has been changed already
					if (LastHitSMC->GetStaticMesh() == BTD->SelectedMesh) {
						// Update the wall ID in the BuildData of the mesh that has been changed
						GroundFloor->UpdateWallID(LastHitSMC->GetComponentLocation(), LastHitSMC->GetComponentRotation(), BTD->SelectedID);

						// Clear the LastHitSMC's and Mesh, as the wall is already replaced
						LastHitSMC = nullptr;
						LastHitMesh = nullptr;

						// Charge the player for the window
						PC->UpdateMoney(WindowDataTable->FindRow<FSelectableWindowData>(BTD->SelectedID, "")->Price);
					}
				}
			}
		}

		ClickPosition = FVector(-1, -1, -1);
		// Also clear the BTD 
		BTD->ClearBuildDisplay();

		BTW->UpdateTextVisibility(false);
		BTW->UpdateDisplayedText(0, 0, 0);
	}

	// If the current tool is the ObjectTool, then...
	else if (CurrentTool == EToolType::Object) {
		GroundFloor->AddObjectToLevel(ObjectToolMeshComponent->GetStaticMesh(), ObjectToolMeshComponent->GetComponentTransform());
	}
}

/// -- Build Tool Functions --
// Called to toggle the erase mode to normal mode (and vice versa)
void APlayer_Tools::ToggleEraseMode(bool bOverride, bool bOverrideTo)
{
	if (bOverride == true) {
		bInEraseMode = bOverrideTo;
		BTD->bInEraseMode = bOverrideTo;
		PC->GetUI()->BuildState->UpdateEraseButtonEnabled(bOverrideTo);
	}
	else {
		bInEraseMode = !bInEraseMode;
		BTD->bInEraseMode = bInEraseMode;
		PC->GetUI()->BuildState->UpdateEraseButtonEnabled(bInEraseMode);
	}
}

// Called toswap between sub-tools
void APlayer_Tools::SwapSubTool(TEnumAsByte<EBuildToolSubType> NewSubTool)
{
	BTD->UpdateSubTool(NewSubTool);
	ToggleEraseMode(true, false);
	PC->GetUI()->BuildState->UpdateSelectedSubToolButton(NewSubTool);

	BTW->UpdateTextVisibility(false);

	if (LastHitSMC) {
		if (LastHitSMC->GetStaticMesh() != LastHitMesh) {
			LastHitSMC->SetStaticMesh(LastHitMesh);
		}
	}
	LastHitSMC = nullptr;
	LastHitMesh = nullptr;
}

// Called to update the selected mesh in the BuildToolDisplay
// Leave empty to clear (nullptr)
void APlayer_Tools::UpdateSelectedItem(FName ID, TEnumAsByte<EBuildToolSubType> Type)
{
	if (ID == "") {
		BTD->SelectedMesh = nullptr;
		BTD->SelectedID = ID;
		BTD->ClearBuildDisplay();
	}
	else {
		if (Type == Wall) {
			BTD->SelectedMesh = WallDataTable->FindRow<FSelectableWallData>(ID, "")->Mesh;
			BTD->SelectedID = ID;
		}
		else if (Type == Window) {
			BTD->SelectedMesh = WindowDataTable->FindRow<FSelectableWindowData>(ID, "")->Mesh;
			BTD->SelectedID = ID;
		}
	}
}

/// -- Object Tool Functions --
// Called to update the current mesh shown (leave blank to clear)
void APlayer_Tools::UpdateObjectMesh(UStaticMesh* NewStaticMesh)
{
	if (NewStaticMesh != ObjectToolMeshComponent->GetStaticMesh()) {
		ObjectToolMeshComponent->SetStaticMesh(NewStaticMesh);
	}
	else {
		ObjectToolMeshComponent->SetStaticMesh(nullptr);
	}
}

// Called to toggle the from rotation mode to normal mode (and vice versa)
void APlayer_Tools::ToggleRotationMode()
{
	bInRotationMode = !bInRotationMode;
}

/// -- Utility Functions --
// Called to fire a trace to hit an object, returning FHitResult from the trace
FHitResult APlayer_Tools::FireTraceToActor()
{
	// Fire a line trace infront of this camera
	FHitResult TraceHit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(PC);
	TraceParams.AddIgnoredActor(BTD);

	// Mouse Location/Direction Vectors
	FVector MouseDir, MouseLoc;

	// Get the player's mouse location in world space
	PC->GetPC()->DeprojectMousePositionToWorld(MouseLoc, MouseDir);

	bool InteractTrace = GetWorld()->LineTraceSingleByChannel(TraceHit, MouseLoc, MouseLoc + (MouseDir * 4000), ECC_WorldDynamic, TraceParams);
	//DrawDebugLine(GetWorld(), MouseLoc, MouseLoc + (MouseDir * 2000), FColor::Red, false, 5, 2, 5);

	return TraceHit;
}

// Called to round a float to the nearest multple, returning the output
float APlayer_Tools::GetNearestMultiple(float Input, int Multiple)
{
	if (Multiple == 0) {
		return Input;
	}
	return round(Input / Multiple) * Multiple;
}
