// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/WidgetSwitcher.h"
#include "ToolLibrary.h"

#include "EditorUI.generated.h"

UCLASS()
class DORESPUB_API UEditorUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void SynchronizeProperties() override;

	UFUNCTION(BlueprintImplementableEvent)
		void UpdateRotationElement();

	UFUNCTION(BlueprintImplementableEvent)
		void UpdateMovementElement();

	UFUNCTION(BlueprintImplementableEvent)
		void SetupItemState();

	void SwapUIState(EToolType NewState);

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UWidgetSwitcher* ToolState = nullptr;

	UPROPERTY(BlueprintReadOnly)
		class AEditorPlayer* EditorPlayer = nullptr;
};
