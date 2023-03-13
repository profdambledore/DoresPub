// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "EditorPlayer.generated.h"

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


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Axis Binds
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);
	void RotateX(float AxisValue);
	void ZoomCamera(float AxisValue);

private:
	// Check if there is any axis value on input
	bool GetHasAxisValue(float AxisValue);

public:	
	// References
	APlayerController* PC;

protected:
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		UCameraComponent* PlayerCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		USpringArmComponent* PlayerCameraSpringArm;

	// Control Bounds
	// How much the value of SpringArmLength changes per update
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
		float ZoomMultiplier = 10;

	// The minimum SpringArmLength
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
		float MinZoomValue = 500;

	// The maximum SpringArmLength
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
		float MaxZoomValue = 1000;

	// How much the camera's angle changes per update
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
		float RotationDegree = 2;

	// How many unreal units to move each step
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
		float MovementStep = 4;

};
