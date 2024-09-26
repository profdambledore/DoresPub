// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "UI_Player_Object_CatButton.generated.h"


UCLASS()
class DORESPUB_API UUI_Player_Object_CatButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// Called to clear this as the selected category
	void ClearSelectedCategory();

protected:
	// Button event called to update the current selected category
	UFUNCTION(BlueprintImplementableEvent)
	void OnCategoryButtonReleased();


public:
	/// -- Components --
	// Image to display the icon of the category
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* CategoryImage = nullptr;

	// Text Box to display the name of the category
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* CategoryTextBlock = nullptr;

	// Button to select this category
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* CategoryButton = nullptr;
};
