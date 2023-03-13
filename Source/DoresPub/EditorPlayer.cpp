// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/CharacterMovementComponent.h"
#include "EditorPlayer.h"

// Sets default values
AEditorPlayer::AEditorPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup Camera and Sprint Arm
	PlayerCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	PlayerCameraSpringArm->SetRelativeRotation(FRotator(-40.0f, 0.0f, 0.0f));
	PlayerCameraSpringArm->bUsePawnControlRotation = false;
	PlayerCameraSpringArm->TargetArmLength = MinZoomValue + ((MaxZoomValue - MinZoomValue) / 2);
	PlayerCameraSpringArm->bDoCollisionTest = false;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	PlayerCamera->SetupAttachment(PlayerCameraSpringArm, USpringArmComponent::SocketName);

	// Remove Controller Yaw
	bUseControllerRotationYaw = false;

	// Set Gravity scale to 0 on this character
	GetCharacterMovement()->GravityScale = 0;

}

// Called when the game starts or when spawned
void AEditorPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// Complete the reference to the player controller and show the mouse cursor
	PC = Cast<APlayerController>(GetController());
	PC->bShowMouseCursor = true;
}


// Called every frame
void AEditorPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEditorPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Add Axis Binds
	PlayerInputComponent->BindAxis("MoveX", this, &AEditorPlayer::MoveX);
	PlayerInputComponent->BindAxis("MoveY", this, &AEditorPlayer::MoveY);
	PlayerInputComponent->BindAxis("RotateX", this, &AEditorPlayer::RotateX);
	PlayerInputComponent->BindAxis("ZoomCamera", this, &AEditorPlayer::ZoomCamera);
}

void AEditorPlayer::MoveX(float AxisValue)
{
	if (GetHasAxisValue(AxisValue))
	{
		AddActorLocalOffset(FVector((MovementStep * AxisValue), 0.0f, 0.0f), false, false);
	}
}

void AEditorPlayer::MoveY(float AxisValue)
{
	if (GetHasAxisValue(AxisValue))
	{
		AddActorLocalOffset(FVector(0.0f, (MovementStep * AxisValue), 0.0f), false, false);
	}
}

void AEditorPlayer::RotateX(float AxisValue)
{
	if (GetHasAxisValue(AxisValue))
	{
		AddActorWorldRotation(FRotator(0.0f, (RotationDegree * AxisValue), 0.0f), false, false);
	}
}

void AEditorPlayer::ZoomCamera(float AxisValue)
{
	if (GetHasAxisValue(AxisValue))
	{
		if (AxisValue == 1)
		{
			if (PlayerCameraSpringArm->TargetArmLength + ZoomMultiplier >= MaxZoomValue)
			{
				PlayerCameraSpringArm->TargetArmLength = MaxZoomValue;
			}
			else
			{
				PlayerCameraSpringArm->TargetArmLength = PlayerCameraSpringArm->TargetArmLength + ZoomMultiplier;
			}
		}
		else if (AxisValue == -1)
		{
			if (PlayerCameraSpringArm->TargetArmLength - ZoomMultiplier <= MinZoomValue)
			{
				PlayerCameraSpringArm->TargetArmLength = MinZoomValue;
			}
			else
			{
				PlayerCameraSpringArm->TargetArmLength = PlayerCameraSpringArm->TargetArmLength - ZoomMultiplier;
			}
		}
	}
}

bool AEditorPlayer::GetHasAxisValue(float AxisValue)
{
	if (AxisValue != 0) {
		return true;
	}
	else return false;
}

