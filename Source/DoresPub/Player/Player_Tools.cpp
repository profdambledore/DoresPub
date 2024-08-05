// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_Tools.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Player/Player_Character.h"
#include "Player/Player_Controller.h"
#include "Player/Player_BuildToolDisplay.h"
#include "World/World_BuildingLevel.h"

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
		FVector MouseLocation = FireTraceToActor().Location;
		
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
				BTD->GenerateNewBuildDisplay(ClickPosition, LastPosition);

				// Check if it is valid (money)
				if (BTD->GetDisplayWallsInUse() * HalfWallCost <= PC->GetCurrentMoney()) {
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
			BTD->GenerateNewBuildDisplay(ClickPosition, ClickPosition);
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
			// Check what mode they are in
			if (!bInEraseMode) {
				// Check if they have enough money for the building
				int cost = BTD->GetDisplayWallsInUse() * HalfWallCost;
				if (cost <= PC->GetCurrentMoney()) {
					// If they do, then 
					GroundFloor->AddBuildingObjects(BTD->GetDisplayData());
					PC->UpdateMoney(-cost);
				}
			}
			else {
				// TODO - Refund the player for the walls being removed
				GroundFloor->RemoveBuildingObjects(BTD->GetDisplayData());
			}
		}

		ClickPosition = FVector(-1, -1, -1);
		// Also clear the BTD 
		BTD->ClearBuildDisplay();
	}

	// If the current tool is the ObjectTool, then...
	else if (CurrentTool == EToolType::Object) {
		GroundFloor->AddObjectToLevel(ObjectToolMeshComponent->GetStaticMesh(), ObjectToolMeshComponent->GetComponentTransform());
	}
}

/// -- Build Tool Functions --
// Called to toggle the erase mode to normal mode (and vice versa)
void APlayer_Tools::ToggleEraseMode()
{
	bInEraseMode = !bInEraseMode;
	BTD->bInEraseMode = bInEraseMode;
}

// Called to update the selected mesh in the BuildToolDisplay
// Leave empty to clear (nullptr)
void APlayer_Tools::UpdateSelectedWall(UStaticMesh* NewWallMesh)
{
	BTD->SelectedMesh = NewWallMesh;
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
