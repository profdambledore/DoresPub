// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "InputAction.h"

#include "Input_ConfigData.generated.h"

UCLASS()
class DORESPUB_API UInput_ConfigData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// Pointer to the forward/backwards player input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement Inputs")
	class UInputAction* MoveXInput = nullptr;

	// Pointer to the strafe player input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement Inputs")
	class UInputAction* MoveYInput = nullptr;

	// Pointer to the camera rotation player input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Inputs")
	class UInputAction* RotateCamera = nullptr;

	// Pointer to the modify camrera zoom player input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Inputs")
	class UInputAction* ZoomCamera = nullptr;

	// Pointer to the primary action player input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action Inputs")
	class UInputAction* PrimaryActionInput = nullptr;

	// Pointer to the secondary action player input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action Inputs")
	class UInputAction* SecondaryActionInput = nullptr;

	// Pointer to the modify grid snapping input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid Inputs")
	class UInputAction* IncreaseGridSnapInput = nullptr;

	// Pointer to the modify grid snapping input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid Inputs")
	class UInputAction* DecreaseGridSnapInput = nullptr;

	// Pointer to the default tool input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tools Inputs")
	class UInputAction* DefaultToolInput = nullptr;

	// Pointer to the building tool input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tools Inputs")
	class UInputAction* BuildingToolInput = nullptr;

	// Pointer to the object tool input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tools Inputs")
	class UInputAction* ObjectToolInput = nullptr;
};
