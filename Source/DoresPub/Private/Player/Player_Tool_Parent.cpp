// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_Tool_Parent.h"

#include "Player/Player_Character.h"
#include "Player/Player_Controller.h"

// Sets default values
APlayer_Tool_Parent::APlayer_Tool_Parent()
{
	// Setup the Root Component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = Root;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APlayer_Tool_Parent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayer_Tool_Parent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayer_Tool_Parent::PrimaryActionPressed()
{
}

void APlayer_Tool_Parent::PrimaryActionReleased()
{
}

void APlayer_Tool_Parent::ToolTick()
{
}

void APlayer_Tool_Parent::OnEquip()
{
}

void APlayer_Tool_Parent::OnUnequip()
{
}

void APlayer_Tool_Parent::SetupTool(APlayer_Character* NewPC)
{
	PC = NewPC;
}

void APlayer_Tool_Parent::UpdateToolRotation()
{
}

FHitResult APlayer_Tool_Parent::FireTraceToActor()
{
	// Fire a line trace infront of this camera
	FHitResult TraceHit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(PC);
	//TraceParams.AddIgnoredActor(BTD);

	// Mouse Location/Direction Vectors
	FVector MouseDir, MouseLoc;

	// Get the player's mouse location in world space
	PC->GetPC()->DeprojectMousePositionToWorld(MouseLoc, MouseDir);

	bool InteractTrace = GetWorld()->LineTraceSingleByChannel(TraceHit, MouseLoc, MouseLoc + (MouseDir * 4000), ECC_WorldDynamic, TraceParams);
	//DrawDebugLine(GetWorld(), MouseLoc, MouseLoc + (MouseDir * 2000), FColor::Red, false, 5, 2, 5);

	return TraceHit;
}

float APlayer_Tool_Parent::GetNearestMultiple(float Input, int Multiple)
{
	if (Multiple == 0) {
		return Input;
	}
	return round(Input / Multiple) * Multiple;
}
