// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "ParentTool.h"
#include "ToolLibrary.h"

#include "EditorPlayer.generated.h"

class UParentTool;

UCLASS()
class DORESPUB_API AEditorPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEditorPlayer();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		void SwapTool(TEnumAsByte<EToolType> NewTool);

	// Control Update
	UFUNCTION(BlueprintCallable)
		void ModifyRotationDegree(bool bIncrease);

	UFUNCTION(BlueprintCallable)
		void ModifyMovementStep(bool bIncrease);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Axis Binds
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);
	void RotateX(float AxisValue);
	void ZoomCamera(float AxisValue);

	// Action Binds
	void UseToolPrimary();
	void UseToolSecondary();
	void ExploreTool();
	void SelectTool();
	void BuildTool();
	void ItemTool();

private:
	// Check if there is any axis value on input
	bool GetHasAxisValue(float AxisValue);

	void AddNewTool(UParentTool* NewTool);

public:	
	// References
	APlayerController* PC;
	class ABuildingManager* BM;

	// Money
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Money;

protected:
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		UCameraComponent* PlayerCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		USpringArmComponent* PlayerCameraSpringArm;

	// UI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "User Interface")
		class UEditorUI* EditorUI;

	// Control Bounds
	// How much the value of SpringArmLength changes per update
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
		float ZoomMultiplier = 10;

	// The minimum SpringArmLength
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Control Properties")
		float MinZoomValue = 500;

	// The maximum SpringArmLength
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Control Properties")
		float MaxZoomValue = 1000;

	// How much the camera's angle changes per update
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
		float RotationDegree = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Control Properties")
		float RotationMin = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Control Properties")
		float RotationMax = 5;

	// How many unreal units to move each step
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
		float MovementStep = 4;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Control Properties")
		float MovementMin = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Control Properties")
		float MovementMax = 10;

	// Tools
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tools")
		TMap<TEnumAsByte<EToolType>, UParentTool*> ToolMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tools")
		TEnumAsByte<EToolType> CurrentTool;
};
