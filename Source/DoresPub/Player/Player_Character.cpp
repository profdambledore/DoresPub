// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_Character.h"

#include "DrawDebugHelpers.h"

#include "Player/Player_Controller.h"
#include "Player/Player_Tools.h"
#include "UI/UI_Player_Master.h"

// Sets default values
APlayer_Character::APlayer_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup the Camera Spring Arm
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Character Spring Arm"));
	CameraSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	CameraSpringArm->SetRelativeRotation(FRotator(-70.0f, 0.0f, 0.0f));
	CameraSpringArm->TargetArmLength = 2000.0f;
	CameraSpringArm->bUsePawnControlRotation = false;
	CameraSpringArm->bDoCollisionTest = false;
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

	// Also find and create the player tools class
	static ConstructorHelpers::FClassFinder<APlayer_Tools>PTClass(TEXT("/Game/Player/BP_Tools"));
	if (PTClass.Succeeded()) {
		UE_LOG(LogTemp, Warning, TEXT("Tools found"));
		PTC = PTClass.Class;
	}

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
		UI->PC = this;
		UI->UpdateGridSnapText(GridSnapValues[CurrentGridSnapValue]);
		UI->UpdateRotationSnapText(RotationSnapValues[CurrentRotationSnapValue]);
	}

	// Setup the player tools, defaulting to the Default tool
	if (PTC) {
		PT = GetWorld()->SpawnActor<APlayer_Tools>(PTC);
		PT->SetupTools(this);
		SwapCurrentActiveTool(0);
	}

	// For testing purposes...
	// Create a new WorldBounds
	UpdateWorldBounds(3000.0f, 3000.0f);

	// Give the player 500 big ones
	UpdateMoney(100050);

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

		PT->UpdateToolRotation();
	}
}

void APlayer_Character::MoveY(float AxisValue)
{
	if (AxisValue != 0 && !bMovementDisabled) {
		// Move on the Y axis based on the input's axis value
		AddMovementInput(GetCapsuleComponent()->GetRightVector(), AxisValue, false);

		PT->UpdateToolRotation();
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

	// And update the Tool's rotation
	if (PT) {
		PT->UpdateToolRotation();
	}
}

void APlayer_Character::ZoomCameraByStep(bool bZoomOut)
{
	// Check if the player wants to zoom in or out
	if (bZoomOut) {
		// If they want to zoom out, then add steps to the CameraSpringArm not going above the ZoomMax
		CameraSpringArm->TargetArmLength += ZoomStep;
		if (CameraSpringArm->TargetArmLength >= ZoomMax) {
			CameraSpringArm->TargetArmLength = ZoomMax;
		}
	}
	else {
		// If they want to zoom in, then remove steps from the CameraSpringArm not going below the ZoomMin
		CameraSpringArm->TargetArmLength -= ZoomStep;
		if (CameraSpringArm->TargetArmLength <= ZoomMin) {
			CameraSpringArm->TargetArmLength = ZoomMin;
		}
	}

	PT->UpdateToolRotation();
}

void APlayer_Character::PrimaryActionTrace()
{
	//AActor* HitActor = PT->FireTraceToActor().GetActor();
	PT->SelectedToolPrimary();
}

void APlayer_Character::SecondaryActionTrace()
{
	AActor* HitActor = PT->FireTraceToActor().GetActor();
}

void APlayer_Character::ChangeGridSnapSize(bool bIncrement)
{
	
	if (bIncrement) {
		UE_LOG(LogTemp, Warning, TEXT("Incrementing"));
		if (CurrentGridSnapValue + 1 >= GridSnapValues.Num() - 1) {
			// Set to max grid snap value
			CurrentGridSnapValue = GridSnapValues.Num() - 1;
		}
		else {
			CurrentGridSnapValue++;
		}
	}
	else {
		if (CurrentGridSnapValue -1 <= 0) {
			// Set to max grid snap value
			CurrentGridSnapValue = 0;
		}
		else {
			CurrentGridSnapValue--;
		}
	}

	// Finally, update the GridSnapText on the UI
	UI->UpdateGridSnapText(GetCurrentGridSnapValue());
}

void APlayer_Character::ChangeRotationSnapSize(bool bIncrement)
{
	if (bIncrement) {
		if (CurrentRotationSnapValue + 1 >= RotationSnapValues.Num() - 1) {
			// Set to max grid snap value
			CurrentRotationSnapValue = RotationSnapValues.Num() - 1;
		}
		else {
			CurrentRotationSnapValue++;
		}
	}
	else {
		if (CurrentRotationSnapValue - 1 <= 0) {
			// Set to max grid snap value
			CurrentRotationSnapValue = 0;
		}
		else {
			CurrentRotationSnapValue--;
		}
	}

	// Finally, update the GridSnapText on the UI
	UI->UpdateRotationSnapText(GetCurrentRotationSnapValue());
}

void APlayer_Character::SwapCurrentActiveTool(int NewTool)
{
	// Update the UI
	UI->UpdateUIToCurrentTool(NewTool);

	// Swap to the new tool based on the input
	switch (NewTool) {
	case 0: // Default Tool
		PT->SwapTool(EToolType::Default);
		break;

	case 1: // Build Tool
		PT->SwapTool(EToolType::Building);
		break;

	case 2: // Build Tool
		PT->SwapTool(EToolType::Object);
		break;

	default:
		break;
	}
}

/// -- World Functions --
void APlayer_Character::UpdateWorldBounds(float X, float Y)
{
	// Clear the world bounds if they exist already
	if (WorldBounds.bIsValid) {
		WorldBounds.Init();
	}

	// Then create a FBox2D from the new points, with Min being the world origin at (0, 0)
	// Add a little buffer so the player can still build on the extent outside
	WorldBounds = FBox2D(FVector2D(-1.0f, -1.0f), FVector2D(X + 1, Y + 1));

	// Draw it too
	DrawDebugBox(GetWorld(), FVector(WorldBounds.GetCenter(), 1.0f), FVector(WorldBounds.GetExtent(), 1.0f), FQuat(), FColor::Blue, true, -1.0f, 0U, 3.0f);
}

/// -- Utility Functions --
APlayer_Controller* APlayer_Character::GetPC()
{
	return PC;
}

APlayer_Tools* APlayer_Character::GetPT()
{
	return PT;
}

int APlayer_Character::GetCurrentGridSnapValue()
{
	return GridSnapValues[CurrentGridSnapValue];
}

int APlayer_Character::GetCurrentRotationSnapValue()
{
	return RotationSnapValues[CurrentRotationSnapValue];
}

bool APlayer_Character::GetIsPointInsideBound(FVector Point)
{
	return WorldBounds.IsInside(FVector2D(Point));
}

int APlayer_Character::GetCurrentMoney()
{
	return Money;
}

void APlayer_Character::UpdateMoney(int ModificationAmount)
{
	Money += ModificationAmount;
	UI->UpdateMoneyText(Money);
}



