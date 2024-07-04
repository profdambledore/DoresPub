// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_Tools.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Player/Player_Character.h"
#include "Player/Player_Controller.h"
#include "Player/Player_BuildToolDisplay.h"
#include "World/World_BuildingLevel.h"

// Sets default values
APlayer_Tools::APlayer_Tools()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = Root;

	BuildToolWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Menu Widget Component"));
	BuildToolWidgetComponent->SetGenerateOverlapEvents(false);
	BuildToolWidgetComponent->SetupAttachment(Root, "");

	// Also find and store the build tools display class
	static ConstructorHelpers::FClassFinder<APlayer_BuildToolDisplay>BTDClass(TEXT("/Game/Player/BP_BuildToolDisplay"));
	if (BTDClass.Succeeded()) {
		BuildToolDisplayClass = BTDClass.Class;
	}
}

// Called when the game starts or when spawned
void APlayer_Tools::BeginPlay()
{
	Super::BeginPlay();

	// Spawn the BuildToolDisplay class
	if (BuildToolDisplayClass) {
		BTD = GetWorld()->SpawnActor<APlayer_BuildToolDisplay>(BuildToolDisplayClass);
	}

	// TEMP -- Find the ground floor and store a pointer to it
	GroundFloor = Cast<AWorld_BuildingLevel>(UGameplayStatics::GetActorOfClass(GetWorld(), AWorld_BuildingLevel::StaticClass()));
	
}

// Called every frame
void APlayer_Tools::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check which tool the player is in
	// If the current tool is the Build tool, then...
	if (CurrentTool == EToolType::Building) {
		// Fire a trace to the mouse's position, updating the tools location where the trace hits if it is inside of the WorldBounds
		// Also snap it to the building snapping distance (half of a wall size)
		FVector MouseLocation = FireTraceToActor().Location;
		
		MouseLocation.X = GetNearestMultiple(MouseLocation.X, 125);
		MouseLocation.Y = GetNearestMultiple(MouseLocation.Y, 125);
		MouseLocation.Z = 1.0f;
		SetActorLocation(MouseLocation);

		// Update the LastPosition and call any related functions
		if (MouseLocation != LastPosition) {
			LastPosition = MouseLocation;
			UpdateToolRotation();

			// If the ClickPosition is valid (not (-1, -1, -1), then update the BTD
			if (ClickPosition != FVector(-1, -1, -1)) {
				BTD->GenerateNewBuildDisplay(ClickPosition, LastPosition);

				// Check if it is valid (money)
				if (BTD->GetDisplayWallsInUse() * HalfWallCost <= PC->GetCurrentMoney()) {
					// Next, check if the MouseLocation is outside the zone.
					if (!PC->GetIsPointInsideBound(MouseLocation)) {
						BTD->UpdateDisplayValidity(false);
					}
					else {
						BTD->UpdateDisplayValidity(true);
					}
				}
				else {
					BTD->UpdateDisplayValidity(false);
				}
			}
		}
	}

}

void APlayer_Tools::SetupTools(APlayer_Character* NewPC)
{
	PC = NewPC;
	UpdateToolRotation();
}

void APlayer_Tools::SwapTool(TEnumAsByte<EToolType> NewTool)
{
	CurrentTool = NewTool;
	ClickPosition = FVector(-1, -1, -1);
	BTD->ClearBuildDisplay();
}

void APlayer_Tools::UpdateToolRotation()
{
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PC->Camera->GetComponentLocation()));
}

void APlayer_Tools::SelectedToolPrimary()
{
	bPrimaryHeld = !bPrimaryHeld;

	if (bPrimaryHeld) {
		SelectedToolPrimaryPressed();
	}
	else {
		SelectedToolPrimaryReleased();
	}
}

void APlayer_Tools::SelectedToolPrimaryPressed()
{
	// If the current tool is the BuildTool, then...
	if (CurrentTool == EToolType::Building) {
		// Check where the player has clicked is inside of the WorldBounds
		FVector testClickPos = FireTraceToActor().Location;
		if (PC->GetIsPointInsideBound(testClickPos)) {
			// If it is, then round it to the Building Bounds and start "drawing" with the BuildToolDisplay 
			testClickPos.X = GetNearestMultiple(testClickPos.X, 125);
			testClickPos.Y = GetNearestMultiple(testClickPos.Y, 125);
			testClickPos.Z = 1.0f;
			ClickPosition = testClickPos;
			BTD->GenerateNewBuildDisplay(ClickPosition, ClickPosition);
		}
	}
}

void APlayer_Tools::SelectedToolPrimaryReleased()
{
	// If the current tool is the BuildTool, then...
	if (CurrentTool == EToolType::Building) {
		// Check where the player has clicked is inside of the WorldBounds
		FVector testClickPos = FireTraceToActor().Location;
		if (PC->GetIsPointInsideBound(testClickPos)) {
			// Check if they have enough money for the building
			int cost = BTD->GetDisplayWallsInUse() * HalfWallCost;
			if (cost <= PC->GetCurrentMoney()) {
				// If they do, then 
				GroundFloor->AddBuildingObjects(BTD->GetDisplayData());
				PC->UpdateMoney(-cost);
			}
		}
		ClickPosition = FVector(-1, -1, -1);
		// Also clear the BTD 
		BTD->ClearBuildDisplay();
	}
}

/// -- Utility Functions --
FHitResult APlayer_Tools::FireTraceToActor()
{
	// Fire a line trace infront of this camera
	FHitResult TraceHit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(PC);
	TraceParams.AddIgnoredActor(BTD);

	// Mouse Location/Direction Vectors
	FVector MouseDir, MouseLoc;

	// Get the player's mouse location in world space
	PC->GetPC()->DeprojectMousePositionToWorld(MouseLoc, MouseDir);

	bool InteractTrace = GetWorld()->LineTraceSingleByChannel(TraceHit, MouseLoc, MouseLoc + (MouseDir * 4000), ECC_WorldDynamic, TraceParams);
	//DrawDebugLine(GetWorld(), MouseLoc, MouseLoc + (MouseDir * 2000), FColor::Red, false, 5, 2, 5);

	return TraceHit;
}

float APlayer_Tools::GetNearestMultiple(float Input, int Multiple)
{
	if (Multiple == 0) {
		return Input;
	}
	return round(Input / Multiple) * Multiple;
}
