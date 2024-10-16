// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "InputAction.h"
#include "Engine/DataAsset.h"

#include "Player_Controller.generated.h"

UCLASS()
class DORESPUB_API APlayer_Controller : public APlayerController
{
	GENERATED_BODY()

public:
	// Constructor
	APlayer_Controller();

	// Called to setup the inputs for this player controller
	virtual void SetupInputComponent() override;

	// Called when the controller possesses a character/pawn
	virtual void OnPossess(APawn* InPawn) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// -- Inputs --
	// Called to move the character on the X axis
	void MoveX(const FInputActionValue& Value);

	// Called to move the character on the Y axis
	void MoveY(const FInputActionValue& Value);

	// Called to make the character's camera rotate
	void RotateCamera(const FInputActionValue& Value);

	// Called to modify the camera's zoom
	void ZoomCamera(const FInputActionValue& Value);

	// Called to fire a primary action input (lmb)
	void PrimaryAction(const FInputActionValue& Value);

	// Called to fire a secondary action input (rmb)
	void SecondaryAction(const FInputActionValue& Value);

	// Called to fire a delete action input (delete/backspace)
	void DeleteAction(const FInputActionValue& Value);

	// Called to change the current snapping value of the grid
	void IncreaseGridSnap(const FInputActionValue& Value);

	// Called to change the current snapping value of the grid
	void DecreaseGridSnap(const FInputActionValue& Value);

	// Called to swap the current active tool to the default tool
	void SwapToDefaultTool(const FInputActionValue& Value);

	// Called to swap the current active tool to the buildng tool
	void SwapToBuildingTool(const FInputActionValue& Value);

	// Called to swap the current active tool to the object tool
	void SwapToObjectTool(const FInputActionValue& Value);

private:
	/// -- Utility Functions --
	// Called to return if an input value is positive or negative
	bool GetInputValueIsPositive(float InInputValue);

public:

protected:

private:
	/// -- Controlled Character --
	// Pointer to the character this controller possesses
	class APlayer_Character* Character = nullptr;

	/// -- Inputs --
	// Pointer to the Input Config File
	class UInput_ConfigData* InputConfig = nullptr;

	// Pointer to the Input Mapping Context
	class UInputMappingContext* InputMapping = nullptr;


};
