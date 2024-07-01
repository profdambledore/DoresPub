// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UI_Player_Master.h"

void UUI_Player_Master::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_Player_Master::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UUI_Player_Master::SwapActiveState(int Index)
{
	if (Index != StateSwitcher->GetActiveWidgetIndex()) {
		StateSwitcher->SetActiveWidgetIndex(Index);
		switch (Index) {
		case 0:
			
			break;

		case 1:
			//InventoryState->OnStateActive();
			break;

		case 2:
			//CraftingState->OnStateActive();
			break;

		case 3:
			//StorageState->OnStateActive();
			break;

		default:
			break;
		}
	}
}

void UUI_Player_Master::SwapActiveState(FString StateName)
{
}
