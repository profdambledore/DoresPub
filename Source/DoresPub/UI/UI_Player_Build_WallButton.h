// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Image.h"
#include "Components/Button.h"

#include "UI_Player_Build_WallButton.generated.h"

UCLASS()
class DORESPUB_API UUI_Player_Build_WallButton : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// Called to clear this as the selected category
	void ClearSelectedCategory();

protected:
	// Button event called to update the current selected category
	UFUNCTION(BlueprintImplementableEvent)
	void OnItemButtonReleased();

public:
	/// -- Components --
	// Image to display the icon of the item
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* WallImage = nullptr;

	// Button to select this item
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* WallButton = nullptr;

};
