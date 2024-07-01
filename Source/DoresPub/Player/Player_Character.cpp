// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_Character.h"

#include "Player/Player_Controller.h"
#include "UI/UI_Player_Master.h"

// Sets default values
APlayer_Character::APlayer_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup the Camera Spring Arm
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Character Spring Arm"));
	CameraSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	CameraSpringArm->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
	CameraSpringArm->TargetArmLength = 2000.0f;
	CameraSpringArm->bUsePawnControlRotation = false;
	CameraSpringArm->SetupAttachment(GetCapsuleComponent(), "");
	CameraSpringArm->bInheritPitch = false; CameraSpringArm->bInheritYaw = true; CameraSpringArm->bInheritRoll = false;

	// Setup the Player's Cameras
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Character Camera"));
	Camera->SetupAttachment(CameraSpringArm, "");

	// Find the UI object and store it
	static ConstructorHelpers::FClassFinder<UUserWidget>UIClass(TEXT("/Game/UI/WBP_Player_Master"));
	if (UIClass.Succeeded()) {
		UE_LOG(LogTemp, Warning, TEXT("UI Found"));
		UI = CreateWidget<UUI_Player_Master>(GetWorld(), UIClass.Class);
	};

}

// Called when the game starts or when spawned
void APlayer_Character::BeginPlay()
{
	Super::BeginPlay();
	
	// Get and store a pointer to the player controller
	PC = Cast<APlayer_Controller>(GetController());
	if (PC) {
		PC->bShowMouseCursor = true;
	}

	// Add the UI to thew viewport
	if (UI) {
		UI->AddToViewport();
	}
}

// Called every frame
void APlayer_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayer_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/// -- Inputs --
void APlayer_Character::MoveX(float AxisValue)
{
	if (AxisValue != 0 && !bMovementDisabled) {
		// Move on the X axis based on the input's axis value
		// Get the camera's forward vector X and Y, removing the Z
		FVector Direction = FRotationMatrix(GetControlRotation()).GetScaledAxis(EAxis::X);
		Direction = FVector(Direction.X, Direction.Y, 0.0f).GetSafeNormal();
		AddMovementInput(Direction, AxisValue, false);
	}
}

void APlayer_Character::MoveY(float AxisValue)
{
	if (AxisValue != 0 && !bMovementDisabled) {
		// Move on the Y axis based on the input's axis value
		AddMovementInput(GetCapsuleComponent()->GetRightVector(), AxisValue, false);
	}
}

void APlayer_Character::RotateCameraByStep(bool bRotateClockwise)
{
	// If the camera is to rotate counter clockwise, decrement the current step
	if (!bRotateClockwise) {
		CurrentStep--;
	}
	// Else, increment the step if the camera is to rotate clockwise
	else {
		CurrentStep++;
	}
	// Then set the rotation based on the current step * the rotation step
	PC->SetControlRotation(FRotator(0.0f, CurrentStep * RotationStep, 0.0f));
}

void APlayer_Character::ZoomCameraByStep(bool bZoomIn)
{
	// Check if the player wants to zoom in or out
	if (bZoomIn) {
		// If they want to zoom in, then remove steps from the CameraSpringArm not going below the ZoomMin
		CameraSpringArm->TargetArmLength -= ZoomStep;
		if (CameraSpringArm->TargetArmLength < ZoomMin) {
			CameraSpringArm->TargetArmLength = ZoomMin;
		}
	}
	else {
		// If they want to zoom out, then add steps to the CameraSpringArm not going above the ZoomMax
		CameraSpringArm->TargetArmLength += ZoomStep;
		if (CameraSpringArm->TargetArmLength < ZoomMax) {
			CameraSpringArm->TargetArmLength = ZoomMax;
		}
	}
}

void APlayer_Character::PrimaryActionTrace()
{
	AActor* HitActor = FireTraceToActor().GetActor();
}

void APlayer_Character::SecondaryActionTrace()
{
	AActor* HitActor = FireTraceToActor().GetActor();
}

///
FHitResult APlayer_Character::FireTraceToActor()
{
	// Fire a line trace infront of this camera
	FHitResult TraceHit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	// Mouse Location/Direction Vectors
	FVector MouseDir, MouseLoc;

	// Get the player's mouse location in world space
	PC->DeprojectMousePositionToWorld(MouseLoc, MouseDir);

	bool InteractTrace = GetWorld()->LineTraceSingleByChannel(TraceHit, MouseLoc, MouseLoc + (MouseDir * 4000), ECC_WorldDynamic, TraceParams);
	//DrawDebugLine(GetWorld(), MouseLoc, MouseLoc + (MouseDir * 2000), FColor::Red, false, 5, 2, 5);

	return TraceHit;
}

