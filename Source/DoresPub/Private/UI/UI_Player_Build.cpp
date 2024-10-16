// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/UI_Player_Build.h"

#include "UI/UI_Data_Build.h"
#include "UI/UI_Player_Master.h"
#include "UI/UI_Player_Build_WallButton.h"

#include "Player/Player_Character.h"
#include "Player/Player_Tool_Build.h"

void UUI_Player_Build::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button events
	EraseModeButton->OnReleased.AddDynamic(this, &UUI_Player_Build::OnEraseButtonReleased);
	WallModeButton->OnReleased.AddDynamic(this, &UUI_Player_Build::OnWallButtonReleased);
	FloorModeButton->OnReleased.AddDynamic(this, &UUI_Player_Build::OnFloorButtonReleased);
	ExtraModeButton->OnReleased.AddDynamic(this, &UUI_Player_Build::OnExtraButtonReleased);

	// Then set the default selected button to wall
	UpdateSelectedSubToolButton(Wall);
}

void UUI_Player_Build::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UUI_Player_Build::AddSelectableWallToList(FName ID, FSelectableWallData WallData)
{
	UBuildSelectButtonData* NewWallObj = NewObject<UBuildSelectButtonData>();
	NewWallObj->SetupData(this, ID, WallData.Icon, Wall);
	WallSelectionTileView->AddItem(NewWallObj);
}

void UUI_Player_Build::AddSelectableExtraToList(FName ID, FSelectableWallData WallData)
{
	UBuildSelectButtonData* NewWallObj = NewObject<UBuildSelectButtonData>();
	NewWallObj->SetupData(this, ID, WallData.Icon, Extra);
	ExtraSelectionTileView->AddItem(NewWallObj);
}

void UUI_Player_Build::UpdateSelectedWall(FName ObjectID, UUI_Player_Build_WallButton* NewSelectedButton)
{
	if (BuildTool) {
		if (SelectedID == ObjectID || ObjectID == "") {
			SelectedID = "";
			SelectedItemButton->ClearSelectedCategory();
			SelectedItemButton = nullptr;
		}
		else {
			SelectedID = ObjectID;
			if (SelectedItemButton) {
				SelectedItemButton->ClearSelectedCategory();
			}
			SelectedItemButton = NewSelectedButton;
			SelectedItemButton->WallBorder->SetBrushColor(SelectedButtonColour);
		}
		
		BuildTool->UpdateSelectedItem(SelectedID, Wall);
	}
}

void UUI_Player_Build::UpdateSelectedExtra(FName ExtraID, UUI_Player_Build_WallButton* NewSelectedButton)
{
	if (BuildTool) {
		if (SelectedID == ExtraID || ExtraID == "") {
			SelectedID = "";
			SelectedItemButton->ClearSelectedCategory();
			SelectedItemButton = nullptr;
		}
		else {
			SelectedID = ExtraID;
			if (SelectedItemButton) {

				SelectedItemButton->ClearSelectedCategory();
			}
			SelectedItemButton = NewSelectedButton;
			SelectedItemButton->WallBorder->SetBrushColor(SelectedButtonColour);
		}
	}

	BuildTool->UpdateSelectedItem(SelectedID, Extra);
}

void UUI_Player_Build::OnEraseButtonReleased()
{
	if (BuildTool) {
		BuildTool->UpdateEraseMode();
	}
}

void UUI_Player_Build::OnWallButtonReleased()
{
	if (BuildTool) {
		BuildTool->UpdateSubTool(EBuildToolSubType::Wall);
	}
}

void UUI_Player_Build::OnFloorButtonReleased()
{
	if (BuildTool) {
		BuildTool->UpdateSubTool(EBuildToolSubType::Floor);
	}
}

void UUI_Player_Build::OnExtraButtonReleased()
{
	if (BuildTool) {
		BuildTool->UpdateSubTool(EBuildToolSubType::Extra);
	}
}

void UUI_Player_Build::UpdateEraseModeEnabled(bool bEnabled)
{
	if (bEnabled) {
		EraseModeButton->SetBackgroundColor(SelectedButtonColour);
		EraseModeSwitcher->SetActiveWidgetIndex(1);
	}
	else {
		EraseModeButton->SetBackgroundColor(UnselectedButtonColour);
		EraseModeSwitcher->SetActiveWidgetIndex(0);
	}
}

void UUI_Player_Build::UpdateSelectedSubToolButton(TEnumAsByte<EBuildToolSubType> NewSubType)
{
	if (SelectedButton) {
		SelectedButton->SetBackgroundColor(UnselectedButtonColour);
	}

	// Clear the selected item and selected button
	if (SelectedItemButton) {
		SelectedID = "";
		SelectedItemButton->ClearSelectedCategory();
		SelectedItemButton = nullptr;
	}
	
	switch (NewSubType) {
	case Wall:
		WallModeButton->SetBackgroundColor(SelectedButtonColour);
		SubStateSwitcher->SetActiveWidgetIndex(0);
		SelectedButton = WallModeButton;
		break;

	case Floor:
		FloorModeButton->SetBackgroundColor(SelectedButtonColour);
		SubStateSwitcher->SetActiveWidgetIndex(1);
		SelectedButton = FloorModeButton;
		break;

	case Extra:
		ExtraModeButton->SetBackgroundColor(SelectedButtonColour);
		SubStateSwitcher->SetActiveWidgetIndex(2);
		SelectedButton = ExtraModeButton;
		break;

	default:
		break;
	}
}