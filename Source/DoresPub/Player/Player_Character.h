// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

#include "Player_Character.generated.h"

UCLASS()
class DORESPUB_API APlayer_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayer_Character();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// -- Inputs --
	// Called to make the player move on the X axis (forward/backward)
	void MoveX(float AxisValue);

	// Called to make the player move on the Y axis (strafe left/right)
	void MoveY(float AxisValue);

	// Called to rotate the camera by CameraRotationStep in a direction.  True if rotation is clockwise.
	void RotateCameraByStep(bool bRotateClockwise);

	// Called to modify the camera's zoom by a set amount (ZoomStep)
	void ZoomCameraByStep(bool bZoomOut);

	// Called to fire a Primary Action Trace
	void PrimaryActionTrace();

	// Called to fire a Secondary Action Trace
	void SecondaryActionTrace();

	// Called to modify the current grid snap size
	void ChangeGridSnapSize(bool bIncrement);

	// Called to change the current active tool
	void SwapCurrentActiveTool(int NewTool);

	// -- World Functions --
	// Called to update the world bounds with a new size (origin is always (0, 0)
	void UpdateWorldBounds(float X, float Y);

	/// -- Utility Functions --
	// Called to get the Player_Controller controlling this player
	class APlayer_Controller* GetPC();

	// Called to get the current grid snap value
	int GetCurrentGridSnapValue();

	// Called to get if a point is inside of the World Bounds
	bool GetIsPointInsideBound(FVector Point);

	// Called to get the player's current money
	int GetCurrentMoney();

	// Called to modify the player's money
	void UpdateMoney(int ModificationAmount);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// -- Utility Functions --


public:	
	/// -- Components --
	// Camera Spring Arm
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* CameraSpringArm = nullptr;

	// Camera 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera = nullptr;

protected:
	/// -- Pointers --
	// Pointer to the controller of this player
	class APlayer_Controller* PC = nullptr;

	// Pointer to the player tools object
	class APlayer_Tools* PT = nullptr;

	// Pointer to the subclass of PlayerTools
	TSubclassOf<class APlayer_Tools> PTC = nullptr;

	// Pointer to the UI added to the viewport
	class UUI_Player_Master* UI = nullptr;

	/// -- Player Properties
	// bool denoting if the player cannot move
	bool bMovementDisabled = false;

	// Float denoting how much money the player has
	int Money = 0.0f;

	/// -- World Properties --
	// FBox2D denoting the world size
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	FBox2D WorldBounds;

	// Float denoting the index of the current grid snap value
	float CurrentGridSnapValue = 0;

	// TArray of grid snap values
	UPROPERTY(EditDefaultsOnly, Category = "World Properties")
	TArray<int> GridSnapValues;

	/// -- Camera Properties --
	// Float to denote zoom step value
	int ZoomStep = 100;

	// Ints denoting the max and min zoom of the camera
	int ZoomMin = 300;
	int ZoomMax = 3000;

	// Float to denote rotation step value
	float RotationStep = 1;

	// Int denoting the current rotation step multiplier
	// Incremented when rotating clockwise, or decremented when counterclockwise
	int CurrentStep = 0;
};
