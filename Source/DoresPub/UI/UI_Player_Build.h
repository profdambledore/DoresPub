// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Button.h"

#include "UI_Player_Build.generated.h"

UCLASS()
class DORESPUB_API UUI_Player_Build : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void SynchronizeProperties() override;

protected:
	/// -- Button Functions --
	// Button event to swap to/from rotation mode
	UFUNCTION()
	void OnRMButtonReleased();

public:
	/// -- Components --
	// Button to swap to/from erase mode
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Grid Snapping")
	UButton* EraseModeButton = nullptr;

	/// -- Pointers --
	// Pointer back to the Player_Master UI class
	class UUI_Player_Master* MUI = nullptr;
	
};
