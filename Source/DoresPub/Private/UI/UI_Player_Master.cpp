// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UI_Player_Master.h"

#include "Kismet/KismetTextLibrary.h"

#include "Player/Player_Character.h"

void UUI_Player_Master::NativeConstruct()
{
	Super::NativeConstruct();

	// Setup the button binds
	// Grid Snap Button Binds
	IncrementGridSnapButton->OnReleased.AddDynamic(this, &UUI_Player_Master::OnIGSButtonReleased);
	DecrementGridSnapButton->OnReleased.AddDynamic(this, &UUI_Player_Master::OnDGSButtonReleased);

	// Rotation Snap Button Binds
	IncrementRotationSnapButton->OnReleased.AddDynamic(this, &UUI_Player_Master::OnIRSButtonReleased);
	DecrementRotationSnapButton->OnReleased.AddDynamic(this, &UUI_Player_Master::OnDRSButtonReleased);

	BuildState->MUI = this;
	ObjectState->MUI = this;
}

void UUI_Player_Master::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

/// -- Global Functions --
void UUI_Player_Master::UpdateGridSnapText(int NewGridSnap)
{
	GridSnapText->SetText(FText::FromString(FString::Printf(TEXT("%i"), NewGridSnap)));
}

void UUI_Player_Master::UpdateRotationSnapText(int NewGridSnap)
{
	RotationSnapText->SetText(FText::FromString(FString::Printf(TEXT("%i"), NewGridSnap)));
}

void UUI_Player_Master::UpdateMoneyText(float NewMoney)
{
	//UKismetTextLibrary::Conv_DoubleToText(NewMoney, ERoundingMode::ToZero, false, true, 1, 324, 2, 2))
	PlayerMoneyText->SetText(FText::AsCurrencyBase(NewMoney, "£"));
}

void UUI_Player_Master::UpdateUIToCurrentTool(int NewTool)
{
	// Swap to the new tool based on the input
	switch (NewTool) {
	case 0: // Default Tool
		CurrentToolText->SetText(FText::FromString(FString::Printf(TEXT("Default Tool"))));
		SwapActiveState(0);
		break;

	case 1: // Build Tool
		CurrentToolText->SetText(FText::FromString(FString::Printf(TEXT("Building Tool"))));
		SwapActiveState(1);
		break;

	case 2:
		CurrentToolText->SetText(FText::FromString(FString::Printf(TEXT("Object Tool"))));
		SwapActiveState(2);
		break;

	default:
		break;
	}
}

// -- Global Button Functions --
void UUI_Player_Master::OnIGSButtonReleased()
{
	if (PC) {
		PC->ChangeGridSnapSize(true);
	}
}

void UUI_Player_Master::OnDGSButtonReleased()
{
	if (PC) {
		PC->ChangeGridSnapSize(false);
	}
}

void UUI_Player_Master::OnIRSButtonReleased()
{
	if (PC) {
		PC->ChangeRotationSnapSize(true);
	}
}

void UUI_Player_Master::OnDRSButtonReleased()
{
	if (PC) {
		PC->ChangeRotationSnapSize(false);
	}
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

/// -- Utility Functions --
APlayer_Tools* UUI_Player_Master::GetPlayerTools()
{
	return nullptr;
}


