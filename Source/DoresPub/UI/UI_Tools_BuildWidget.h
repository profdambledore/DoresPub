// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"

#include "UI_Tools_BuildWidget.generated.h"

UCLASS()
class DORESPUB_API UUI_Tools_BuildWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	virtual void SynchronizeProperties() override;

	// Called to update the visibility of the text on the BuildWidget
	void UpdateTextVisibility(bool bVisible);

	// Called to update the text on the BuildWidget
	void UpdateDisplayedText(int X, int Y, int Price);

public:
	/// -- Components --
	// TextBlock displaying the build size
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* SizeText = nullptr;

	// TextBlock displaying the build cost
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* CostText = nullptr;
};
