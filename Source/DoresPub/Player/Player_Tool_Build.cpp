// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_Tool_Build.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Game/Game_DoresPubGI.h"

#include "Player/Player_Character.h"
#include "World/World_BuildingLevel.h"

#include "UI/UI_Player_Build.h"
#include "UI/UI_Player_Master.h"
#include "UI/UI_Tools_BuildWidget.h"

APlayer_Tool_Build::APlayer_Tool_Build()
{
	// Build Tool Components
	BuildToolWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Build Tool Widget"));
	BuildToolWidgetComponent->SetGenerateOverlapEvents(false);
	BuildToolWidgetComponent->SetVisibility(false, false);
	BuildToolWidgetComponent->SetupAttachment(Root, "");
}

void APlayer_Tool_Build::BeginPlay()
{
	Super::BeginPlay();

	// Complete the pointer to the Build Tool Widget
	BTW = Cast<UUI_Tools_BuildWidget>(BuildToolWidgetComponent->GetWidget());

	InitializeMaterial();
}

void APlayer_Tool_Build::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/// -- Parent Functions --
void APlayer_Tool_Build::PrimaryActionPressed()
{
	// Check if the display mode for the sub tool is a draw method (used by erase mode, wall mode and floor mode)
	// If the player is in erase mode, then...
	if (bInEraseMode) {
		// Check where the player has clicked is inside of the WorldBounds
		FVector testClickPos = FireTraceToActor().Location;
		if (PC->GetIsPointInsideBound(testClickPos)) {
			// If it is, then round it to the Building Bounds and start "drawing" with the BuildToolDisplay 
			testClickPos.X = GetNearestMultiple(testClickPos.X, PC->GetCurrentGridSnapValue());
			testClickPos.Y = GetNearestMultiple(testClickPos.Y, PC->GetCurrentGridSnapValue());
			testClickPos.Z = 1.0f;
			ClickPosition = testClickPos;
			GenerateBuildDisplay(ClickPosition, ClickPosition);
		}
	}
	else
	{
		// Check if the selected sub tool uses a draw method
		if (GetSubTool() == Wall || Floor) {
			// Check where the player has clicked is inside of the WorldBounds
			FVector testClickPos = FireTraceToActor().Location;
			if (PC->GetIsPointInsideBound(testClickPos)) {
				// If it is, then round it to the Building Bounds and start "drawing" with the BuildToolDisplay 
				testClickPos.X = GetNearestMultiple(testClickPos.X, 250);
				testClickPos.Y = GetNearestMultiple(testClickPos.Y, 250);
				testClickPos.Z = 1.0f;
				ClickPosition = testClickPos;
				GenerateBuildDisplay(ClickPosition, ClickPosition);

				BTW->UpdateTextVisibility(true);
			}
		}
	}
}

void APlayer_Tool_Build::PrimaryActionReleased()
{
	switch (GetSubTool()) {
	case Wall:
		BuildToolWallModeReleased();
		break;

	case Floor:
		BuildToolFloorModeReleased();
		break;

	case Window:
		BuildToolWindowModeReleased();
		break;

	default:
		break;
	}

	ClickPosition = FVector(-1, -1, -1);
	// Also clear the BTD 
	ClearBuildDisplay();
}

void APlayer_Tool_Build::ToolTick()
{
	// Fire a trace to the mouse's position, updating the tools location where the trace hits if it is inside of the WorldBounds
	// Also snap it to the building snapping distance (half of a wall size)
	FHitResult TickTrace = FireTraceToActor();
	FVector MouseLocation = TickTrace.Location;

	// Branch off based on the sub-tool.  They uses a draw method (wall, floor) or replace method (window, door)
	// Start by checking if the erase mode is enabled
	if (bInEraseMode) {
		BuildToolEraseTick();
	}
	else
	{
		switch (GetSubTool()) {
		case Wall:
			BuildToolWallModeTick();
			break;

		case Floor:
			BuildToolFloorModeTick();
			break;

		case Window:
			BuildToolWindowModeTick();
			break;

		default:
			break;
		}
	}
}

void APlayer_Tool_Build::OnEquip()
{
	// Make the widget visible
	BuildToolWidgetComponent->SetVisibility(true, false);
}

void APlayer_Tool_Build::OnUnequip()
{
	// Reset required properties to their default (the click position, then clear the BTD and hide the Build Tool's widget)
	ClickPosition = FVector(-1, -1, -1);
	ClearBuildDisplay();
	BuildToolWidgetComponent->SetVisibility(false, false);
}

void APlayer_Tool_Build::SetupTool(APlayer_Character* NewPC)
{
	Super::SetupTool(NewPC);

	PC = NewPC;
	UpdateToolRotation();

	BuildWidget = PC->GetUI()->BuildState;
	BuildWidget->BuildTool = this;

	// Setup the Build Tool
	// Update the SelectableWallTileView
	for (FName i : WallDataTable->GetRowNames()) {
		FSelectableWallData* d = WallDataTable->FindRow<FSelectableWallData>(i, "");
		if (d->Tags.Contains<FString>("selectable_wall")) {
			BuildWidget->AddSelectableWallToList(i, *d);
		}
		else if (d->Tags.Contains<FString>("selectable_window")) {
			BuildWidget->AddSelectableWindowToList(i, *d);
		}
	}
}

// Called to update the rotation of the tool class to re-face the player's camera
void APlayer_Tool_Build::UpdateToolRotation()
{
	// Update the rotation of the tool class to re - face the player's camera
	BuildToolWidgetComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PC->Camera->GetComponentLocation()));
	BuildToolWidgetComponent->SetWorldRotation(FRotator(0.0f, BuildToolWidgetComponent->GetComponentRotation().Yaw, 0.0f));
}

/// -- Tool Functions --
// Called to update the selected sub-tool
void APlayer_Tool_Build::UpdateSubTool(TEnumAsByte<EBuildToolSubType> NewSubTool)
{
	SelectedSubTool = NewSubTool;

	// Also clear the selected meshes and ID's
	SelectedID = "";
	SelectedMesh = nullptr;

	// Clear the Window Mode properties
	if (LastHitSMC) {
		if (LastHitSMC->GetStaticMesh() != LastHitMesh) {
			LastHitSMC->SetStaticMesh(LastHitMesh);
		}
	}
	LastHitSMC = nullptr;
	LastHitMesh = nullptr;

	// Also clear the BTD
	ClearBuildDisplay();

	UpdateEraseMode(true, false);
	PC->GetUI()->BuildState->UpdateSelectedSubToolButton(NewSubTool);

	BTW->UpdateTextVisibility(false);
}

// Called to update if erase mode is enabled or not
void APlayer_Tool_Build::UpdateEraseMode(bool bOverride, bool bOverrideTo)
{
	if (bOverride == true) {
		bInEraseMode = bOverrideTo;
		PC->GetUI()->BuildState->UpdateEraseModeEnabled(bOverrideTo);
	}
	else {
		bInEraseMode = !bInEraseMode;
		PC->GetUI()->BuildState->UpdateEraseModeEnabled(bInEraseMode);
	}
}

// Called to update the selected mesh in the BuildToolDisplay
// Leave empty to clear (nullptr)
void APlayer_Tool_Build::UpdateSelectedItem(FName ID, TEnumAsByte<EBuildToolSubType> Type)
{
	if (ID == "") {
		SelectedMesh = nullptr;
		SelectedID = ID;
		ClearBuildDisplay();
	}
	else {
		SelectedMesh = WallDataTable->FindRow<FSelectableWallData>(ID, "")->Mesh;
		SelectedID = ID;
	}
}

/// -- Tool Tick Functions --
void APlayer_Tool_Build::BuildToolEraseTick()
{
	// Fire a trace to the mouse's position, updating the tools location where the trace hits if it is inside of the WorldBounds
	// Also snap it to the building snapping distance (half of a wall size)
	FHitResult TickTrace = FireTraceToActor();
	FVector MouseLocation = TickTrace.Location;

	MouseLocation.X = GetNearestMultiple(MouseLocation.X, PC->GetCurrentGridSnapValue());
	MouseLocation.Y = GetNearestMultiple(MouseLocation.Y, PC->GetCurrentGridSnapValue());
	MouseLocation.Z = 1.0f;

	SetActorLocation(MouseLocation);

	// Update the LastPosition and call any related functions
	if (MouseLocation != LastPosition) {
		LastPosition = MouseLocation;
		UpdateToolRotation();

		// If the ClickPosition is valid (not (-1, -1, -1), then update the BTD
		if (ClickPosition != FVector(-1, -1, -1)) {
			GenerateBuildDisplay(ClickPosition, LastPosition);
		}
	}
}

void APlayer_Tool_Build::BuildToolWallModeTick()
{
	// Fire a trace to the mouse's position, updating the tools location where the trace hits if it is inside of the WorldBounds
	// Also snap it to the building snapping distance (half of a wall size)
	FHitResult TickTrace = FireTraceToActor();
	FVector MouseLocation = TickTrace.Location;

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
			GenerateBuildDisplay(ClickPosition, LastPosition);

			// Check if we are in erase mode
			if (!bInEraseMode) {
				// If we aren't in erase mode, check that there is a wall ID selected
				if (SelectedID != "") {
					// Calculate the cost
					int cost;  cost = GetDisplayWallsInUse() * WallDataTable->FindRow<FSelectableWallData>(SelectedID, "")->Price;

					// Update the BTW with the new price
					BTW->UpdateDisplayedText(GetDisplaySize().first, GetDisplaySize().second, cost);

					// Check if it is valid (money)
					if (cost <= PC->GetCurrentMoney()) {
						// Next, check if the MouseLocation is outside the zone.
						if (!PC->GetIsPointInsideBound(MouseLocation)) {
							UpdateDisplayValidity(false);
						}
						else {
							UpdateDisplayValidity(true);
						}
					}
					else {
						UpdateDisplayValidity(false);
					}
				}
			}
			else {
				// If we are in erase mode, calculate the cost
				int cost = 0;

				// For each display data returned
				for (FBuildToolData i : GetDisplayData()) {
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
				BTW->UpdateDisplayedText(GetDisplaySize().first, GetDisplaySize().second, cost);

				// Next, check if the MouseLocation is outside the zone.
				if (!PC->GetIsPointInsideBound(MouseLocation)) {
					UpdateDisplayValidity(false);
				}
				else {
					UpdateDisplayValidity(true);
				}
			}
		}
	}
}

void APlayer_Tool_Build::BuildToolFloorModeTick()
{
	// Fire a trace to the mouse's position, updating the tools location where the trace hits if it is inside of the WorldBounds
	// Also snap it to the building snapping distance (half of a wall size)
	FHitResult TickTrace = FireTraceToActor();
	FVector MouseLocation = TickTrace.Location;

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
			GenerateBuildDisplay(ClickPosition, LastPosition);

			int cost;  cost = 0; //BTD->GetDisplayWallsInUse() * WallDataTable->FindRow<FSelectableWallData>(BTD->SelectedWallID, "")->Price;

			// Update the BTW with the new price
			BTW->UpdateDisplayedText(GetDisplaySize().first, GetDisplaySize().second, cost);

			// Check if it is valid (money)
			if (cost <= PC->GetCurrentMoney()) {
				// Next, check if the MouseLocation is outside the zone.
				if (!PC->GetIsPointInsideBound(MouseLocation)) {
					UpdateDisplayValidity(false);
				}
				else {
					UpdateDisplayValidity(true);
				}
			}
			else {
				UpdateDisplayValidity(false);
			}
		}
	}
}

void APlayer_Tool_Build::BuildToolWindowModeTick()
{
	// Fire a trace to the mouse's position, updating the tools location where the trace hits if it is inside of the WorldBounds
	// Also snap it to the building snapping distance (half of a wall size)
	FHitResult TickTrace = FireTraceToActor();
	FVector MouseLocation = TickTrace.Location;

	MouseLocation.X = GetNearestMultiple(MouseLocation.X, PC->GetCurrentGridSnapValue());
	MouseLocation.Y = GetNearestMultiple(MouseLocation.Y, PC->GetCurrentGridSnapValue());
	MouseLocation.Z = 1.0f;

	GenerateBuildDisplay(MouseLocation);
	SetActorLocation(MouseLocation);
	UpdateToolRotation();

	// Next, check if a mesh has been set via GetDisplayWallsInUse
	// If this doesn't return empty, then it is in use
	if (GetDisplayWallsInUse() != 0) {
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
							UpdateDisplayValidity(2);
							LastHitSMC->SetStaticMesh(SelectedMesh);

							// Update the BuildToolWidget to display the selected window's price
							BTW->UpdateTextVisibility(true);
							BTW->UpdateDisplayedText(-1, 0, WallDataTable->FindRow<FSelectableWallData>(SelectedID, "")->Price);
						}
						else {
							// If it doesn't match, invalidate the SMC
							UpdateDisplayValidity(false);
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
				UpdateDisplayValidity(false);
				BTW->UpdateTextVisibility(false);
			}
		}
	}
}

/// -- Tool Release Functions --
void APlayer_Tool_Build::BuildToolWallModeReleased()
{
	// Check where the player has released is inside of the WorldBounds
	FVector testClickPos = FireTraceToActor().Location;
	if (PC->GetIsPointInsideBound(testClickPos)) {
		// Get the display data
		TArray<FBuildToolData> data = GetDisplayData();

		// Check if there is some data in the display mode
		if (data.Num() > 0) {
			// Check what mode they are in
			if (!bInEraseMode) {
				// Get the cost of a single wall (as you can only ever place one type of wall at a time, just check the ID of the first index and get the cost)
				int cost = WallDataTable->FindRow<FSelectableWallData>(data[0].ID, "")->Price;
				// Check if they have enough money for the building
				int total = GetDisplayWallsInUse() * cost;
				if (total <= PC->GetCurrentMoney()) {
					// If they do, then 
					PC->GetCurrentBuildingLevel()->AddBuildingObjects(data);
					PC->UpdateMoney(-total);
				}
			}
			else {
				// Remove the Walls from the WorldBuildingLevel
				TArray<FName> wallsToRefund = PC->GetCurrentBuildingLevel()->RemoveBuildingObjects(data);
				int total = 0;
				for (FName w : wallsToRefund) {
					total += WallDataTable->FindRow<FSelectableWallData>(w, "")->Price * RefundMultiplier;
				}
				PC->UpdateMoney(total);
			}
		}
	}
}

void APlayer_Tool_Build::BuildToolFloorModeReleased()
{
	// Check where the player has clicked is inside of the WorldBounds
	FVector testClickPos = FireTraceToActor().Location;
	if (PC->GetIsPointInsideBound(testClickPos)) {
		// Get the display data
		TArray<FBuildToolData> data = GetDisplayData();

		if (data.Num() > 0) {
			// Check what mode they are in
			if (!bInEraseMode) {
				// If they are, then add the floors to the world
				PC->GetCurrentBuildingLevel()->AddBuildingObjects(data);
			}
			else {
				PC->GetCurrentBuildingLevel()->RemoveBuildingObjects(data);
			}
		}
	}
}

void APlayer_Tool_Build::BuildToolWindowModeReleased()
{
	int cost = WallDataTable->FindRow<FSelectableWallData>(SelectedID, "")->Price;
	if (cost <= PC->GetCurrentMoney()) {
		// Check that there is valid LastHitSMC
		if (LastHitSMC) {
			// Check if a wall has been changed already
			if (LastHitSMC->GetStaticMesh() == SelectedMesh) {
				// Update the wall ID in the BuildData of the mesh that has been changed
				PC->GetCurrentBuildingLevel()->UpdateWallID(LastHitSMC->GetComponentLocation(), LastHitSMC->GetComponentRotation(), SelectedID);

				// Clear the LastHitSMC's and Mesh, as the wall is already replaced
				LastHitSMC = nullptr;
				LastHitMesh = nullptr;

				// Charge the player for the window
				PC->UpdateMoney(-cost);
			}
		}
	}
}

/// -- Display Generation Functions --
// Called to generate a new display based on the current sub-tool
void APlayer_Tool_Build::GenerateBuildDisplay(FVector StartPos, FVector EndPos)
{
	// First, check if the player is in erase mode
	if (bInEraseMode) {
		GenerateEraseDisplay(StartPos, EndPos);
	}
	else {
		// Else, switch on the sub-tool
		switch (SelectedSubTool) {
		case Wall:
			GenerateWallDisplay(StartPos, EndPos);
			break;

		case Floor:
			GenerateFloorDisplay(StartPos, EndPos);
			break;

		case Window:
			GenerateWindowDisplay(StartPos);
			break;

		default:
			break;
		}
	}
}

// Called to generate a display of things to erase
void APlayer_Tool_Build::GenerateEraseDisplay(FVector StartPosition, FVector EndPosition)
{
	// Move the BuildToolDisplay to the StartPosition
	SetActorLocation(StartPosition);

	// Imagine box c d
	//			   a b
	// Set the StartPoint to a's positon and EndPoint to d's position
	FVector a = FVector((StartPosition.X < EndPosition.X) ? StartPosition.X : EndPosition.X, (StartPosition.Y < EndPosition.Y) ? StartPosition.Y : EndPosition.Y, 1.0);
	FVector d = FVector((StartPosition.X > EndPosition.X) ? StartPosition.X : EndPosition.X, (StartPosition.Y > EndPosition.Y) ? StartPosition.Y : EndPosition.Y, 1.0);

	//UE_LOG(LogTemp, Warning, TEXT("Start = %s, End = %s"), *StartPosition.ToString(), *EndPosition.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("a.x = %f / d.x = %f"),a.X, d.X);
	//UE_LOG(LogTemp, Warning, TEXT("a.y = %f / d.y = %f"), a.Y, d.Y);

	// Draw it too
	DrawDebugBox(GetWorld(), a, FVector(8.0f, 8.0f, 1.0f), FQuat(FRotator(0.0f, 0.0f, 0.0f)), FColor::Red, false, 0.5f, 0U, 3.0f);
	DrawDebugBox(GetWorld(), d, FVector(8.0f, 8.0f, 1.0f), FQuat(FRotator(0.0f, 0.0f, 0.0f)), FColor::Purple, false, 0.5f, 0U, 3.0f);
	DrawDebugBox(GetWorld(), (a + ((d - a) / 2)), ((d - a) / 2), FQuat(FRotator(0.0f, 0.0f, 0.0f)), FColor::Green, false, 0.5f, 0U, 3.0f);

	// Start by calculating how many static mesh components are needed
	XSize = 0;	YSize = 0;

	// Calculate how many are needed for a single x edge (will double for a full box)
	// By calculating the difference between the start and end position, making the output absolute (always pos)
	// Then dividing that by the wall length (125 uu) and rounding up with ceil
	XSize = ceil(fabs(d.X - a.X) / WallSize) + 1;

	// Do the same to the Y axis
	YSize = ceil(fabs(d.Y - a.Y) / WallSize) + 1;

	// Calculate the total required for the floor with X * Y
	Total = (XSize * YSize);

	// Add enough static mesh components to match the amount needed 
	if (Total > SMCPool.Num()) {
		AddNewStaticMeshComponent(Total);
	}

	// Start by rounding down with floor to the nearest 250, 250, 1 value
	FVector RoundedStart = FVector(floor(a.X / 250) * 250, floor(a.Y / 250) * 250, 1.0f);
	int ActiveIndex = 0;

	DrawDebugBox(GetWorld(), RoundedStart, FVector(16.0f, 16.0f, 1.0f), FQuat(FRotator(0.0f, 0.0f, 0.0f)), FColor::Orange, false, 0.5f, 0U, 3.0f);

	for (int i = 0; i < YSize; i++) {
		for (int j = 0; j < XSize; j++) {

			// Math
			//https://stackoverflow.com/questions/18295825/determine-if-point-is-within-bounding-box
			//https://stackoverflow.com/questions/4823789/how-to-check-if-any-point-or-part-of-a-line-is-inside-or-touches-a-rectangle
			//https://stackoverflow.com/questions/17095324/fastest-way-to-determine-if-an-integer-is-between-two-integers-inclusive-with

			// If want the wall to just intercect the box,
			//https://en.wikipedia.org/wiki/Cohen–Sutherland_algorithm

			// Check if we should grab the X Wall
			// Check if the bottom of the wall is inside the box, then check if the top of the wall is inside the box
			if (GetPointInsideRectange(a, d, FVector(RoundedStart.X + (WallSize * j), RoundedStart.Y + (WallSize * i), 1.0f)) && GetPointInsideRectange(a, d, FVector(RoundedStart.X + (WallSize * j) + 250, RoundedStart.Y + (WallSize * i), 1.0f))) {
				// Finally, check if there is an actual wall there
				if (PC->GetCurrentBuildingLevel()->GetWallObjectMeshAtPosition(FVector(RoundedStart.X + (WallSize * j), RoundedStart.Y + (WallSize * i), 1.0f), true)) {
					SMCPool[ActiveIndex]->SetRelativeLocation(FVector(RoundedStart.X + (WallSize * j), RoundedStart.Y + (WallSize * i), 1.0f));
					SMCPool[ActiveIndex]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
					SMCPool[ActiveIndex]->SetStaticMesh(PC->GetCurrentBuildingLevel()->GetWallObjectMeshAtPosition(FVector(RoundedStart.X + (WallSize * j), RoundedStart.Y + (WallSize * i), 1.0f), true)->GetStaticMesh());
					ActiveIndex++;
				}
			}

			// Next, check if we should grab the Y Wall
			// Check if the bottom of the wall is inside the box, then check if the top of the wall is inside the box
			if (GetPointInsideRectange(a, d, FVector(RoundedStart.X + (WallSize * j), RoundedStart.Y + (WallSize * i), 1.0f)) && GetPointInsideRectange(a, d, FVector(RoundedStart.X + (WallSize * j), RoundedStart.Y + (WallSize * i) + 250, 1.0f))) {
				// Finally, check if there is an actual wall there
				if (PC->GetCurrentBuildingLevel()->GetWallObjectMeshAtPosition(FVector(RoundedStart.X + (WallSize * j), RoundedStart.Y + (WallSize * i), 1.0f), false)) {
					SMCPool[ActiveIndex]->SetRelativeLocation(FVector(RoundedStart.X + (WallSize * j), RoundedStart.Y + (WallSize * i), 1.0f));
					SMCPool[ActiveIndex]->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
					SMCPool[ActiveIndex]->SetStaticMesh(PC->GetCurrentBuildingLevel()->GetWallObjectMeshAtPosition(FVector(RoundedStart.X + (WallSize * j), RoundedStart.Y + (WallSize * i), 1.0f), false)->GetStaticMesh());
					ActiveIndex++;
				}
			}

		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("ActiveIndex = %i"), ActiveIndex);

	// Finally, clear the excees SMC
	for (int j = ActiveIndex; j < SMCPool.Num(); j++) {
		SMCPool[j]->SetStaticMesh(nullptr);
	}
}

// Called to generate a new wall display
void APlayer_Tool_Build::GenerateWallDisplay(FVector StartPosition, FVector EndPosition)
{
	// Check that there is a mesh selected.  If so,
	if (SelectedMesh) {
		// Move the BuildToolDisplay to the StartPosition
		SetActorLocation(StartPosition);

		// Start by calculating how many static mesh components are needed
		XSize = 0;	YSize = 0;

		// Calculate how many are needed for a single x edge (will double for a full box)
		// By calculating the difference between the start and end position, making the output absolute (always pos)
		// Then dividing that by the wall length (125 uu) and rounding up with ceil
		XSize = ceil(fabs(EndPosition.X - StartPosition.X) / WallSize);

		// Do the same to the Y axis
		YSize = ceil(fabs(EndPosition.Y - StartPosition.Y) / WallSize);

		// Then calculate the total needed + 4 for the pillar corner blocks
		// If both X and Y require no walls, then don't add any to the required total
		Total = (XSize * ((YSize != 0) ? 2 : 1)) + (YSize * ((XSize != 0) ? 2 : 1));
		//Total = ((Total != 0) ? Total + 0 : 0);

		// Add enough static mesh components to match the amount needed 
		if (Total > SMCPool.Num()) {
			AddNewStaticMeshComponent(Total);
		}

		// Check if the Y axis requires 0 SMC while X requires some (build a wall on the X axis only)
		if (XSize != 0 && YSize == 0) {
			CreateLineOnX(StartPosition.X, EndPosition.X, StartPosition.Y, XSize, 0);
		}

		// Check if the X axis requires 0 SMC while Y requires some (build a wall on the Y axis only)
		else if (XSize == 0 && YSize != 0) {
			CreateLineOnY(StartPosition.Y, EndPosition.Y, StartPosition.X, YSize, 0);
		}

		// Else, check that both are not 0
		else if (XSize != 0 && YSize != 0) {
			// Start with the X walls, then the Y walls
			CreateLineOnX(StartPosition.X, EndPosition.X, StartPosition.Y, XSize, 0);
			CreateLineOnX(StartPosition.X, EndPosition.X, EndPosition.Y, XSize, XSize);

			CreateLineOnY(StartPosition.Y, EndPosition.Y, StartPosition.X, YSize, (XSize * 2));
			CreateLineOnY(StartPosition.Y, EndPosition.Y, EndPosition.X, YSize, (XSize * 2) + YSize);
		}

		if (Total != 0) {
			//CreatePillars(StartPosition, EndPosition);
		}

		// Finally, clear the excees SMC
		for (int j = Total; j < SMCPool.Num(); j++) {
			SMCPool[j]->SetStaticMesh(nullptr);
		}
	}
}

// Called to generate a new floor display
void APlayer_Tool_Build::GenerateFloorDisplay(FVector StartPosition, FVector EndPosition)
{
	// Move the BuildToolDisplay to the StartPosition
	SetActorLocation(StartPosition);

	// Start by calculating how many static mesh components are needed
	XSize = 0;	YSize = 0;

	// Calculate how many are needed for a single x edge (will double for a full box)
	// By calculating the difference between the start and end position, making the output absolute (always pos)
	// Then dividing that by the wall length (125 uu) and rounding up with ceil
	XSize = ceil(fabs(EndPosition.X - StartPosition.X) / WallSize);

	// Do the same to the Y axis
	YSize = ceil(fabs(EndPosition.Y - StartPosition.Y) / WallSize);

	// Calculate the total required for the floor with X * Y
	Total = XSize * YSize;

	UE_LOG(LogTemp, Warning, TEXT("Size = %i * %i, totals %i"), XSize, YSize, Total);

	// Add enough static mesh components to match the amount needed 
	if (Total > SMCPool.Num()) {
		AddNewStaticMeshComponent(Total);
	}

	// Then draw the floor if there is a valid grid
	if (XSize > 0 && YSize > 0) {
		for (int i = 0; i < YSize; i++) {
			CreateLineOnX(StartPosition.X, EndPosition.X, StartPosition.Y + (i * WallSize), XSize, i * XSize, FloorTileMesh);
		}
	}

	// Finally, clear the excees SMC
	for (int j = Total; j < SMCPool.Num(); j++) {
		SMCPool[j]->SetStaticMesh(nullptr);
	}
}

// Called to generate a new window display
void APlayer_Tool_Build::GenerateWindowDisplay(FVector StartPosition)
{
	// Move the BuildToolDisplay to the StartPosition
	SetActorLocation(StartPosition);

	// Check if there is a SMC in index 0 of SMCPool
	if (SMCPool.IsEmpty()) {
		// If so, add a default one now
		AddNewStaticMeshComponent(1);
	}

	// Check that there is a SelectedMesh set
	if (SelectedMesh) {
		// If so, set SMCPool[0] to it if it hasn't already 
		if (!SMCPool[0]->GetStaticMesh()) {
			SMCPool[0]->SetStaticMesh(SelectedMesh);
		}

		// Move the SMC to the start location offsetting it by 125 on the x axis)
		SMCPool[0]->SetRelativeLocation(FVector(StartPosition.X - 125, StartPosition.Y, 1.0f));
		SMCPool[0]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}
}

// Called to create a line of static meshes along the X axis
// Start is the X of the start position, End is the X of the end position
// Y is the Y co-ord of the start position, AmountOfSegements is the amount of static meshes to draw
// PoolStartIndex is the position to start taking components from in the static mesh pool
// MeshOverride is the mesh set in the components (will use SelectedMesh if left empty)
void APlayer_Tool_Build::CreateLineOnX(float Start, float End, float Y, int AmountOfSegements, int PoolStartIndex, UStaticMesh* MeshOverride)
{
	// Calculate which point is the start
	int WallStart = (End - Start < -1) ? End : Start;
	int WallEnd = (End - Start < -1) ? Start : End;

	// For the amount X requires, place a SMC every 250 uu
	for (int i = 0; i < AmountOfSegements; i++) {
		SMCPool[PoolStartIndex + i]->SetRelativeLocation(FVector(WallStart + (WallSize * i), Y, 1.0f));
		SMCPool[PoolStartIndex + i]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		SMCPool[PoolStartIndex + i]->SetStaticMesh((MeshOverride) ? MeshOverride : SelectedMesh);

		// Update the mesh based on what sub-tool we are in
		if (SelectedSubTool == Wall) {
			// If in default mode, check if there is a mesh already at the position
			if (!bInEraseMode) {
				if (PC->GetCurrentBuildingLevel()->GetWallObjectMeshAtPosition(FVector(WallStart + (WallSize * i), Y, 1.0f), true)) {
					// If there is, clear the mesh
					SMCPool[PoolStartIndex + i]->SetStaticMesh(nullptr);
				}
			}
			// Else, check if there isn't a mesh at the position
			//else {
				//if (!GroundFloor->GetWallObjectMeshAtPosition(FVector(WallStart + (WallSize * i), Y, 1.0f), true)) {
					// If there isn't, clear the mesh
					//SMCPool[PoolStartIndex + i]->SetStaticMesh(nullptr);
				//}
				// Else, match the meshes
				//else {
					//SMCPool[PoolStartIndex + i]->SetStaticMesh(GroundFloor->GetWallObjectMeshAtPosition(FVector(WallStart + (WallSize * i), Y, 1.0f), true)->GetStaticMesh());
				//}
			//}
		}
		else if (SelectedSubTool == Floor) {
			// If in default mode, check if there is a mesh already at the position
			UE_LOG(LogTemp, Warning, TEXT("Floor"));
			if (!bInEraseMode) {
				if (PC->GetCurrentBuildingLevel()->GetFloorObjectMeshAtPosition(FVector(WallStart + (WallSize * i), Y, 1.0f))) {
					SMCPool[PoolStartIndex + i]->SetStaticMesh(nullptr);
				}
			}
			else {
				if (!PC->GetCurrentBuildingLevel()->GetFloorObjectMeshAtPosition(FVector(WallStart + (WallSize * i), Y, 1.0f))) {
					SMCPool[PoolStartIndex + i]->SetStaticMesh(nullptr);
				}
			}
		}
	}
}

// Called to create a line of static meshes along the Y axis
// Start is the Y of the start position, End is the Y of the end position
// X is the X co-ord of the start position, AmountOfSegements is the amount of static meshes to draw
// PoolStartIndex is the position to start taking components from in the static mesh pool
// MeshOverride is the mesh set in the components (will use SelectedMesh if left empty)
void APlayer_Tool_Build::CreateLineOnY(float Start, float End, float X, int AmountOfSegements, int PoolStartIndex, UStaticMesh* MeshOverride)
{
	// Calculate which point is the start
	int WallStart = (End - Start < -1) ? End : Start;
	int WallEnd = (End - Start < -1) ? Start : End;

	// For the amount X requires, place a SMC every 250 uu
	for (int i = 0; i < AmountOfSegements; i++) {
		SMCPool[PoolStartIndex + i]->SetRelativeLocation(FVector(X, WallStart + (WallSize * i), 1.0f));
		SMCPool[PoolStartIndex + i]->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
		SMCPool[PoolStartIndex + i]->SetStaticMesh((MeshOverride) ? MeshOverride : SelectedMesh);

		// Update the mesh based on what sub-tool we are in
		if (SelectedSubTool == Wall) {
			// If in default mode, check if there is a mesh already at the position
			if (!bInEraseMode) {
				if (PC->GetCurrentBuildingLevel()->GetWallObjectMeshAtPosition(FVector(X, WallStart + (WallSize * i), 1.0f), false)) {
					SMCPool[PoolStartIndex + i]->SetStaticMesh(nullptr);
				}
			}
			// Else, check if there isn't a mesh at the position
			else {
				if (!PC->GetCurrentBuildingLevel()->GetWallObjectMeshAtPosition(FVector(X, WallStart + (WallSize * i), 1.0f), false)) {
					SMCPool[PoolStartIndex + i]->SetStaticMesh(nullptr);
				}
				// Else, match the meshes
				else {
					SMCPool[PoolStartIndex + i]->SetStaticMesh(PC->GetCurrentBuildingLevel()->GetWallObjectMeshAtPosition(FVector(X, WallStart + (WallSize * i), 1.0f), false)->GetStaticMesh());
				}
			}
		}
		else if (SelectedSubTool == Floor) {
			// If in default mode, check if there is a mesh already at the position
			if (!bInEraseMode) {
				if (PC->GetCurrentBuildingLevel()->GetFloorObjectMeshAtPosition(FVector(X, WallStart + (WallSize * i), 1.0f))) {
					SMCPool[PoolStartIndex + i]->SetStaticMesh(nullptr);
				}
			}
			else {
				if (!PC->GetCurrentBuildingLevel()->GetFloorObjectMeshAtPosition(FVector(X, WallStart + (WallSize * i), 1.0f))) {
					SMCPool[PoolStartIndex + i]->SetStaticMesh(nullptr);
				}
			}
		}
	}
}

// Called to clear the building display
void APlayer_Tool_Build::ClearBuildDisplay()
{
	// Simply clear each spawned SMC in the SMCPool
	for (UStaticMeshComponent* i : SMCPool) {
		i->SetStaticMesh(nullptr);
	}
}

/// -- Display Utility Functions --
// Called to update the display's validity
// Use 0/false for invalid, 1/true for valid and 2 for hidden
void APlayer_Tool_Build::UpdateDisplayValidity(int NewVisibility)
{
	// switch on NewVisibility
	switch (NewVisibility) {
	case 0: // If the tool display is invalid,
		// Set the material instance to use the invalid colour
		UpdateMaterial(InvalidColour);
		break;

	case 1: // If the tool display is valid,
		// Set the material instance to use the valid colour
		UpdateMaterial(ValidColour);
		break;

	case 2: // If the tool display is hidden,
		// Set the material instance to use the hidden colour
		UpdateMaterial(HiddenColour);
		break;

	default:
		break;
	}
}

// Called to return how many walls are currently being used
// Return an int of how many SMC's match the correct criteria
int APlayer_Tool_Build::GetDisplayWallsInUse()
{
	// Init local variable
	int amn = 0;

	// For each SMC in the StaticMeshComponent pool...
	for (UStaticMeshComponent* i : SMCPool) {
		// Check if the mesh is valid.  If so...
		if (i->GetStaticMesh() != nullptr) {
			amn++;
		}
	}
	// Return the total count
	return amn;
}

// Called to return the wall data
// Returns a TArray of FBuildToolData structs for each valid SMC
TArray<FBuildToolData> APlayer_Tool_Build::GetDisplayData()
{
	// Init local variables
	TArray<FBuildToolData> out; FBuildToolData curr;

	// Continue based on the selected sub-tool
	if (SelectedSubTool == Wall) {
		// For each SMC in the StaticMeshComponent pool...
		for (UStaticMeshComponent* i : SMCPool) {
			// Check if the mesh is valid.  If so...
			if (i->GetStaticMesh() != nullptr) {
				// Set the curr FBuildToolData to the correct properties of the current SMC (Mesh, Location and Rotation)
				curr.Mesh = i->GetStaticMesh();
				curr.Location = i->GetComponentLocation();
				curr.Rotation = i->GetComponentRotation();
				curr.ID = SelectedID;

				// And add it to the total array
				out.Add(curr);
			}
		}
	}
	else if (SelectedSubTool == Floor) {
		// For each SMC in the StaticMeshComponent pool...
		for (UStaticMeshComponent* i : SMCPool) {
			// Check if the mesh is valid.  If so...
			if (i->GetStaticMesh() != nullptr) {
				// Set the curr FBuildToolData to the correct properties of the current SMC (Mesh, Location and Rotation)
				curr.Mesh = i->GetStaticMesh();
				curr.Location = i->GetComponentLocation();
				curr.Rotation = i->GetComponentRotation();
				curr.ID = "floor";

				// And add it to the total array
				out.Add(curr);
			}
		}
	}


	// Return the total array
	return out;
}

// Called to return the current size of the display
// Returns as a pair of ints
std::pair<int, int> APlayer_Tool_Build::GetDisplaySize()
{
	return std::make_pair(XSize, YSize);
}

// Called to add a new StaticMeshComponent to the BuildToolDisplay
void APlayer_Tool_Build::AddNewStaticMeshComponent(int Target)
{
	// Init local variable
	int NewNumber = SMCPool.Num() + 1;

	// Create a new StaticMeshComponent with the new number, register it so it can be used and set the mesh's materials and collision
	UStaticMeshComponent* NewMeshComp = NewObject<UStaticMeshComponent>(this, FName(*FString::Printf(TEXT("Wall Mesh %i"), NewNumber)));
	NewMeshComp->RegisterComponent();
	NewMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NewMeshComp->SetMaterial(0, BuildToolMaterial);
	NewMeshComp->SetMaterial(1, BuildToolMaterial);
	NewMeshComp->SetMaterial(2, BuildToolMaterial);
	NewMeshComp->SetMaterial(3, BuildToolMaterial);
	NewMeshComp->SetCollisionProfileName(FName("BuildingTool"));

	// Then add it to the total pool (SMCPool)
	SMCPool.Add(NewMeshComp);

	// Check if there is now enough component sets added.  If not, recurse
	if (SMCPool.Num() < Target) {
		AddNewStaticMeshComponent(Target);
	}
}

// Called to check if a point is inside a rectange
// A is the bottom left point on the rectange, D is the top right
// Point is the point to check
bool APlayer_Tool_Build::GetPointInsideRectange(FVector A, FVector D, FVector Point)
{
	if ((unsigned)(Point.X - A.X) <= (D.X - A.X) && ((unsigned)(Point.Y - A.Y) <= (D.Y - A.Y))) {
		return true;
	}
	return false;
}
