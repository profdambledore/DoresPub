// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_Controller.h"

#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "Inputs/Input_ConfigData.h"
#include "Player/Player_Character.h"

// Constructor
APlayer_Controller::APlayer_Controller()
{
	// Find the input objects
	// First, find the Input Context Data
	ConstructorHelpers::FObjectFinder<UInput_ConfigData>ICDObject(TEXT("/Game/Inputs/ICD_DoresPub"));
	if (ICDObject.Succeeded()) { InputConfig = ICDObject.Object; }

	// Last, find the Input Mapping Context
	ConstructorHelpers::FObjectFinder<UInputMappingContext>IMCObject(TEXT("/Game/Inputs/IMC_DoresPub"));
	if (IMCObject.Succeeded()) { InputMapping = IMCObject.Object; }
}

void APlayer_Controller::BeginPlay()
{
	// Called when the game starts or when spawned
	Super::BeginPlay();

	// Enable the mouse cursor on screen
	bShowMouseCursor = true;
}

// Called to setup the inputs for this player controller
void APlayer_Controller::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Check if the input object pointers are valid
	if (InputConfig && InputMapping) {
		// Get and store the local player subsystem
		auto EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

		// Then clear any existing mapping, then add the new mapping
		EnhancedInputSubsystem->ClearAllMappings();
		EnhancedInputSubsystem->AddMappingContext(InputMapping, 0);

		// Get the EnhancedInputComponent
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

		// Bind the inputs
		EnhancedInputComponent->BindAction(InputConfig->MoveXInput, ETriggerEvent::Triggered, this, &APlayer_Controller::MoveX);
		EnhancedInputComponent->BindAction(InputConfig->MoveYInput, ETriggerEvent::Triggered, this, &APlayer_Controller::MoveY);
		EnhancedInputComponent->BindAction(InputConfig->RotateCamera, ETriggerEvent::Triggered, this, &APlayer_Controller::RotateCamera);
		EnhancedInputComponent->BindAction(InputConfig->ZoomCamera, ETriggerEvent::Triggered, this, &APlayer_Controller::ZoomCamera);
		EnhancedInputComponent->BindAction(InputConfig->PrimaryActionInput, ETriggerEvent::Triggered, this, &APlayer_Controller::PrimaryAction);
		EnhancedInputComponent->BindAction(InputConfig->SecondaryActionInput, ETriggerEvent::Triggered, this, &APlayer_Controller::SecondaryAction);
		EnhancedInputComponent->BindAction(InputConfig->IncreaseGridSnapInput, ETriggerEvent::Triggered, this, &APlayer_Controller::IncreaseGridSnap);
		EnhancedInputComponent->BindAction(InputConfig->DecreaseGridSnapInput, ETriggerEvent::Triggered, this, &APlayer_Controller::DecreaseGridSnap);
		EnhancedInputComponent->BindAction(InputConfig->DefaultToolInput, ETriggerEvent::Triggered, this, &APlayer_Controller::SwapToDefaultTool);
		EnhancedInputComponent->BindAction(InputConfig->BuildingToolInput, ETriggerEvent::Triggered, this, &APlayer_Controller::SwapToBuildingTool);
		EnhancedInputComponent->BindAction(InputConfig->ObjectToolInput, ETriggerEvent::Triggered, this, &APlayer_Controller::SwapToObjectTool);
	}
}

// Called when the controller possesses a character/pawn
void APlayer_Controller::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Cast to the character pawn and store it
	Character = Cast<APlayer_Character>(GetPawn());
}

/// -- Inputs --
// Called to move the character on the X axis
void APlayer_Controller::MoveX(const FInputActionValue& Value)
{
	// Check if the Character Pointer has been created successfully.  If so...
	if (Character) {
		// Call the MoveX command on the character
		Character->MoveX(Value.Get<float>());
	}
}

// Called to move the character on the Y axis
void APlayer_Controller::MoveY(const FInputActionValue& Value)
{
	// Check if the Character Pointer has been created successfully.  If so...
	if (Character) {
		// Call the MoveY command on the character
		Character->MoveY(Value.Get<float>());
	}
}

// Called to make the character's camera rotate
void APlayer_Controller::RotateCamera(const FInputActionValue& Value)
{
	// Check if the Character Pointer has been created successfully.  If so...
	if (Character) {
		Character->RotateCameraByStep(GetInputValueIsPositive(Value.Get<float>()));
	}
}

// Called to modify the camera's zoom
void APlayer_Controller::ZoomCamera(const FInputActionValue& Value)
{
	// Check if the Character Pointer has been created successfully.  If so...
	if (Character) {
		UE_LOG(LogTemp, Warning, TEXT("%f"), Value.Get<float>());
		Character->ZoomCameraByStep(GetInputValueIsPositive(Value.Get<float>()));
	}
}

// Called to fire a primary action input (lmb)
void APlayer_Controller::PrimaryAction(const FInputActionValue& Value)
{
	// Check if the Character Pointer has been created successfully.  If so...
	if (Character) {
		Character->PrimaryActionTrace();
	}
}

// Called to fire a secondary action input (rmb)
void APlayer_Controller::SecondaryAction(const FInputActionValue& Value)
{
	// Check if the Character Pointer has been created successfully.  If so...
	if (Character) {
		Character->SecondaryActionTrace();
	}
}

// Called to change the current snapping value of the grid
void APlayer_Controller::IncreaseGridSnap(const FInputActionValue& Value)
{
	// Check if the Character Pointer has been created successfully.  If so...
	if (Character) {
		Character->ChangeGridSnapSize(true);
	}
}

// Called to change the current snapping value of the grid
void APlayer_Controller::DecreaseGridSnap(const FInputActionValue& Value)
{
	// Check if the Character Pointer has been created successfully.  If so...
	if (Character) {
		Character->ChangeGridSnapSize(false);
	}
}

// Called to swap the current active tool to the default tool
void APlayer_Controller::SwapToDefaultTool(const FInputActionValue& Value)
{
	// Check if the Character Pointer has been created successfully.  If so...
	if (Character) {
		Character->SwapCurrentActiveTool(0);
	}
}

// Called to swap the current active tool to the buildng tool
void APlayer_Controller::SwapToBuildingTool(const FInputActionValue& Value)
{
	// Check if the Character Pointer has been created successfully.  If so...
	if (Character) {
		Character->SwapCurrentActiveTool(1);
	}
}

// Called to swap the current active tool to the object tool
void APlayer_Controller::SwapToObjectTool(const FInputActionValue& Value)
{
	// Check if the Character Pointer has been created successfully.  If so...
	if (Character) {
		Character->SwapCurrentActiveTool(2);
	}
}

/// -- Utility Functions --
// Called to return if an input value is positive or negative
bool APlayer_Controller::GetInputValueIsPositive(float InInputValue)
{
	if (InInputValue == 1) {
		return false;
	}
	return true;
}


