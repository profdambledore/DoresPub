// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/UI_Player_Build.h"

#include "UI/UI_Data_Build.h"
#include "UI/UI_Player_Master.h"
#include "Player/Player_Tools.h"

void UUI_Player_Build::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button events
	EraseModeButton->OnReleased.AddDynamic(this, &UUI_Player_Build::OnRMButtonReleased);
}

void UUI_Player_Build::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UUI_Player_Build::AddSelectableWallToList(FName ID, FSelectableWallData WallData)
{
	UWallButtonData* NewWallObj = NewObject<UWallButtonData>();
	NewWallObj->SetupData(this, ID, WallData.Icon);
	WallSelectionTileView->AddItem(NewWallObj);
}

void UUI_Player_Build::UpdateSelectedWall(FName ObjectID)
{
	if (MUI) {
		//UStaticMesh* NewMesh = WallDataTable->FindRow<FSelectableWallData>(ObjectID, "")->Mesh; 
		if (SelectedID == ObjectID) {
			SelectedID = "";
			
		}
		else {
			SelectedID = ObjectID;
		}
		
		MUI->GetPlayerTools()->UpdateSelectedWall(SelectedID);
	}
}

void UUI_Player_Build::OnRMButtonReleased()
{
	if (MUI) {
		MUI->GetPlayerTools()->ToggleEraseMode();
	}
}
