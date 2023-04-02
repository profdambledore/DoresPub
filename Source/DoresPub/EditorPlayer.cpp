// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

#include "ParentTool.h"
#include "ExploreTool.h"
#include "SelectTool.h"
#include "BuildTool.h"
#include "ItemTool.h"
#include "ToolInterface.h"

#include "EditorUI.h"
#include "BuildingManager.h"

#include "EditorPlayer.h"

// Sets default values
AEditorPlayer::AEditorPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Remove collision on the mesh and capsule
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Setup Camera and Sprint Arm
	PlayerCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	PlayerCameraSpringArm->SetRelativeRotation(FRotator(-40.0f, 0.0f, 0.0f));
	PlayerCameraSpringArm->bUsePawnControlRotation = false;
	PlayerCameraSpringArm->TargetArmLength = MinZoomValue + ((MaxZoomValue - MinZoomValue) / 2);
	PlayerCameraSpringArm->bDoCollisionTest = false;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	PlayerCamera->SetupAttachment(PlayerCameraSpringArm, USpringArmComponent::SocketName);

	// Setup Tools
	// Explore
	ExploreTool = CreateDefaultSubobject<UExploreTool>(TEXT("Explore Tool"));
	AddNewTool(ExploreTool);
	
	// Select
	SelectTool = CreateDefaultSubobject<USelectTool>(TEXT("Select Tool"));
	AddNewTool(SelectTool);

	// Build
	BuildTool = CreateDefaultSubobject<UBuildTool>(TEXT("Build Tool"));
	AddNewTool(BuildTool);

	// Item
	ItemPlacerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));
	ItemPlacerMesh->SetCollisionProfileName(FName("ItemPlacer"));

	ItemTool = CreateDefaultSubobject<UItemTool>(TEXT("Item Tool"));
	AddNewTool(ItemTool);
	ItemTool->FurnishingMesh = ItemPlacerMesh;

	// Remove Controller Yaw
	bUseControllerRotationYaw = false;

	// Set Gravity scale to 0 on this character
	GetCharacterMovement()->GravityScale = 0;

	// Find UI object and store it
	static ConstructorHelpers::FClassFinder<UUserWidget>EditorClass(TEXT("/Game/UI/WBP_EditorPlayer"));
	if (EditorClass.Succeeded()) {
		EditorUI = CreateWidget<UEditorUI>(GetWorld(), EditorClass.Class);
	};

	// Set the current tool (test purpose only, otherwise this should be EToolType::ExploreTool)
	CurrentTool = EToolType::Explore;
}

// Called when the game starts or when spawned
void AEditorPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// Complete the reference to the player controller and show the mouse cursor
	PC = Cast<APlayerController>(GetController());
	PC->bShowMouseCursor = true;

	// Add the widget to the players viewport
	EditorUI->AddToViewport();
	EditorUI->EditorPlayer = this;
	EditorUI->SetupGlobalState();
	EditorUI->SetupItemState();

	// Find and store pointer to BuildingManager
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABuildingManager::StaticClass(), FoundActors);
	if (FoundActors.Num() == 0){
		// If an interior cannot be found, give a warning
		UE_LOG(LogTemp, Warning, TEXT("Manager not found"));
	}
	else{
		// Set the pointer to the world manager
		UE_LOG(LogTemp, Warning, TEXT("Manager found"));
		BM = Cast<ABuildingManager>(FoundActors[0]);
		BM->EditorPlayer = this;
	}
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

	// Add Action Binds
	PlayerInputComponent->BindAction("UseToolPrimary", IE_Pressed, this, &AEditorPlayer::PressToolPrimary);
	PlayerInputComponent->BindAction("UseToolPrimary", IE_Released, this, &AEditorPlayer::ReleaseToolPrimary);
	PlayerInputComponent->BindAction("UseToolSecondary", IE_Released, this, &AEditorPlayer::UseToolSecondary);
	PlayerInputComponent->BindAction("ExploreTool", IE_Released, this, &AEditorPlayer::SwapToExploreTool);
	PlayerInputComponent->BindAction("SelectTool", IE_Released, this, &AEditorPlayer::SwapToSelectTool);
	PlayerInputComponent->BindAction("BuildTool", IE_Released, this, &AEditorPlayer::SwapToBuildTool);
	PlayerInputComponent->BindAction("ItemTool", IE_Released, this, &AEditorPlayer::SwapToItemTool);
}

// -- Controls
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

void AEditorPlayer::ModifyRotationDegree(bool bIncrease)
{
	if (bIncrease == true) {
		if (RotationDegree + 1 >= RotationMax) {
			RotationDegree = RotationMax;
		}
		else {
			RotationDegree++;
		}
	}
	else {
		if (RotationDegree - 1 <= RotationMin) {
			RotationDegree = RotationMin;
		}
		else {
			RotationDegree--;
		}
	}
}

void AEditorPlayer::ModifyMovementStep(bool bIncrease)
{
	if (bIncrease == true) {
		if (MovementStep + 1 >= MovementMax) {
			MovementStep = MovementMax;
		}
		else {
			MovementStep++;
		}
	}
	else {
		if (MovementStep - 1 <= MovementMin) {
			MovementStep = MovementMin;
		}
		else {
			MovementStep--;
		}
	}
}

void AEditorPlayer::PressToolPrimary()
{
	// Find returns ** pointer (which points to the maps memory location that then points to the ParentTool memory location), while FindRef returns * pointer.
	// See https://www.quora.com/What-does-the-pointer-**-mean-in-C++ for more
	// 
	// No longer need interface cast, found bug
	//IToolInterface* inf = Cast<IToolInterface>(ToolMap.FindRef(CurrentTool));
	//if (inf) { inf->ToolPrimary(); };

	ToolMap.FindRef(CurrentTool)->PressToolPrimary();
}

void AEditorPlayer::ReleaseToolPrimary()
{
}

void AEditorPlayer::UseToolSecondary()
{
	// See UseToolPrimary
	ToolMap.FindRef(CurrentTool)->ToolSecondary();
}

// -- Swap Tools
void AEditorPlayer::SwapTool(TEnumAsByte<EToolType> NewTool)
{
	// First, check if the current tool isn't the same as the new tool
	if (NewTool != CurrentTool) {
		// If it's different, check if the player has that tool in ToolMap
		if (ToolMap.Contains(NewTool) == true) {
			// If they do, set the new tool as the current tool
			ToolMap.FindRef(CurrentTool)->ClearTool();
			CurrentTool = NewTool;
			EditorUI->SwapUIState(NewTool);
		}
	}
}

void AEditorPlayer::SwapToExploreTool()
{
	SwapTool(EToolType::Explore);
}

void AEditorPlayer::SwapToSelectTool()
{
	SwapTool(EToolType::Select);
}

void AEditorPlayer::SwapToBuildTool()
{
	SwapTool(EToolType::Build);
}

void AEditorPlayer::SwapToItemTool()
{
	SwapTool(EToolType::Item);
}

// -- Helpers
bool AEditorPlayer::GetHasAxisValue(float AxisValue)
{
	if (AxisValue != 0) {
		return true;
	}
	else return false;
}

void AEditorPlayer::AddNewTool(UParentTool* NewTool)
{
	// First, check if the tool already exists in the map
	if (ToolMap.Contains(NewTool->ToolType) == false) {
		ToolMap.Add(NewTool->ToolType, NewTool);
		NewTool->ToolOwner = this;
	}
}

