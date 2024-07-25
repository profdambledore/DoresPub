// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "UI/UI_Player_Object.h"

#include "UI_Player_Master.generated.h"

UCLASS()
class DORESPUB_API UUI_Player_Master : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void SynchronizeProperties() override;

	/// -- Global Functions --
	// Called to update the GridSnapText
	void UpdateGridSnapText(int NewGridSnap);

	// Called to update the GridSnapText
	void UpdateRotationSnapText(int NewGridSnap);

	// Called to update the MoneyText
	void UpdateMoneyText(float NewMoney);

	// Called to update the current active tool
	void UpdateUIToCurrentTool(int NewTool);

	/// -- State Functions --
	UFUNCTION(BlueprintCallable)
	void SwapActiveState(int Index);

	void SwapActiveState(FString StateName);

	/// -- Utility Functions --
	// Called to return a pointer to the PlayerTools class spawned by the PC
	class APlayer_Tools* GetPlayerTools();

protected:
	/// -- Global Button Functions --
	// Button event to increase the grid snap size
	UFUNCTION()
	void OnIGSButtonReleased();

	// Button event to decrease the grid snap size
	UFUNCTION()
	void OnDGSButtonReleased();

	// Button event to increase the rotation snap size
	UFUNCTION()
	void OnIRSButtonReleased();

	// Button event to decrease the rotation snap size
	UFUNCTION()
	void OnDRSButtonReleased();

public:
	/// -- Components --
	// Widget switcher that allows the UI to swap between different states
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* StateSwitcher = nullptr;

	/// -- Global Objects --
	// Text Box to display the player's money
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* PlayerMoneyText = nullptr;

	// Text Box to display the player's current grid snap
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Grid Snapping")
	UTextBlock* GridSnapText = nullptr;

	// Button to increase the grid snap size
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Grid Snapping")
	UButton* IncrementGridSnapButton = nullptr;

	// Button to decrease the grid snap size
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Grid Snapping")
	UButton* DecrementGridSnapButton = nullptr;

	// Text Box to display the player's current grid snap
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Grid Snapping")
	UTextBlock* RotationSnapText = nullptr;

	// Button to increase the grid snap size
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Grid Snapping")
	UButton* IncrementRotationSnapButton = nullptr;

	// Button to decrease the grid snap size
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Grid Snapping")
	UButton* DecrementRotationSnapButton = nullptr;

	// Text Box to display the player's active tool
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* CurrentToolText = nullptr;

	/// -- UI States --
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "States")
	UUI_Player_Object* ObjectState = nullptr;

	/// -- Player Pointer --
	// Pointer to the player class which uses this UI
	class APlayer_Character* PC = nullptr;
};
