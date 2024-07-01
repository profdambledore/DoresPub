// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"

#include "UI_Player_Master.generated.h"

UCLASS()
class DORESPUB_API UUI_Player_Master : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void SynchronizeProperties() override;

	/// -- UI Functions --
	UFUNCTION(BlueprintCallable)
	void SwapActiveState(int Index);

	void SwapActiveState(FString StateName);

public:
	/// -- Components --
	// Widget switcher that allows the UI to swap between different states
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* StateSwitcher = nullptr;

	/// -- Global Objects --
	// Text Box to display the player's money
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* PlayerMoneyText = nullptr;

	/// -- UI States --
	
};
