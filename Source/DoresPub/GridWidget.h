// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/UniformGridPanel.h"

#include "GridWidget.generated.h"

/**
 * 
 */
UCLASS()
class DORESPUB_API UGridWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void SynchronizeProperties() override;

	UFUNCTION(BlueprintImplementableEvent)
		void SetupGridWidget(int GridX, int GridY);

	UFUNCTION(BlueprintImplementableEvent)
		void UpdateGridWidget(int GridX, int GridY);

public:
	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		//class UUniformGridPanel* Grid = nullptr;
};
