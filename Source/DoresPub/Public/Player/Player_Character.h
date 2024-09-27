// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

#include "Player_Character.generated.h"

// Enum denoting the different types of tools currently available to use
UENUM(BlueprintType, Category = "Tools")
enum EToolType
{
	Default UMETA(DisplayName = "Default Tool"),
	Building UMETA(DisplayName = "Building Tool"),
	Object UMETA(DisplayName = "Object Tool"),
	ToolCount UMETA(Hidden),
};

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

	// Called to fire a Primary Action
	void PrimaryAction();

	// Called to fire a Secondary Action Trace
	void SecondaryActionTrace();

	// Called to modify the current grid snap size
	void ChangeGridSnapSize(bool bIncrement);

	// Called to modify the current rotation snap size
	void ChangeRotationSnapSize(bool bIncrement);

	// Called to change the current active tool
	void SwapCurrentActiveTool(int NewTool);

	// -- World Functions --
	// Called to update the world bounds with a new size (origin is always (0, 0)
	void UpdateWorldBounds(float X, float Y);

	/// -- Utility Functions --
	// Called to get the Player_Controller controlling this player
	class APlayer_Controller* GetPC();

	// Called to get the UI
	class UUI_Player_Master* GetUI();

	// Called to get the current grid snap value
	int GetCurrentGridSnapValue();

	// Called to get the current rotation snap value
	int GetCurrentRotationSnapValue();

	// Called to get if a point is inside of the World Bounds
	bool GetIsPointInsideBound(FVector Point);

	// Called to get the player's current money
	int GetCurrentMoney();

	// Called to modify the player's money
	void UpdateMoney(int ModificationAmount);

	// Called to get the current selected tool from the map
	class APlayer_Tool_Parent* GetCurrentTool() { return AvailableTools.FindRef(CurrentTool); }

	// Called to get the current selected building level
	class AWorld_BuildingLevel* GetCurrentBuildingLevel() { return GroundFloor; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// -- Utility Functions --
	void GiveTool(TEnumAsByte<EToolType> NewToolToAdd);

private:


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

	// Float denoting the index of the current grid snap value
	float CurrentRotationSnapValue = 0;

	// TArray of grid snap values
	UPROPERTY(EditDefaultsOnly, Category = "World Properties")
	TArray<int> RotationSnapValues;

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

	/// -- Tools Properties --
	// TMap of all available tools
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool Properties")
	TMap< TEnumAsByte<EToolType>, class APlayer_Tool_Parent*> AvailableTools;

	// Enum denoting the current tool selected
	TEnumAsByte<EToolType> CurrentTool = EToolType::Default;

	// Bool denoting if the PrimaryToolButton is currently being held
	bool bPrimaryHeld = false;

	// Build Tool Class
	UPROPERTY(EditDefaultsOnly, Category = "Tool Properties")
	TSubclassOf<APlayer_Tool_Parent> BuildToolClass = nullptr;

	// Object Tool Class
	UPROPERTY(EditDefaultsOnly, Category = "Tool Properties")
	TSubclassOf<APlayer_Tool_Parent> ObjectToolClass = nullptr;

	// TEMP
	// Pointer to the World Building Level
	class AWorld_BuildingLevel* GroundFloor = nullptr;
};
