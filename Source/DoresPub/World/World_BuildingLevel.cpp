// Fill out your copyright notice in the Description page of Project Settings.


#include "World/World_BuildingLevel.h"
#include "Player/Player_BuildToolDisplay.h"

// Sets default values
AWorld_BuildingLevel::AWorld_BuildingLevel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = Root;
}

// Called when the game starts or when spawned
void AWorld_BuildingLevel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWorld_BuildingLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWorld_BuildingLevel::AddBuildingObjects(TArray<struct FBuildingData> DataToBuild)
{
	// Start by calculating the amount of SMC's required for the current build + the new build data
	int Required; int Current = 0;
	for (UStaticMeshComponent* i : SMCPool) {
		if (i->GetStaticMesh() != nullptr) {
			Current++;
		}
	}
	Required = Current + DataToBuild.Num();

	UE_LOG(LogTemp, Warning, TEXT("Current = %i / Required (Current + AmnNew) = %i"), Current, Required);

	// Spawn in any required SMC's
	if (Required > SMCPool.Num()) {
		AddNewStaticMeshComponent(Required);
	}

	UE_LOG(LogTemp, Warning, TEXT("Spawned = %i"), SMCPool.Num());

	// Then add the data to the building
	for (int i = 0; i < DataToBuild.Num(); i++) {
		SMCPool[Current + i]->SetStaticMesh(DataToBuild[i].Mesh);
		SMCPool[Current + i]->SetRelativeLocation(DataToBuild[i].Location);
		SMCPool[Current + i]->SetRelativeRotation(DataToBuild[i].Rotation);
	}
}

void AWorld_BuildingLevel::AddNewStaticMeshComponent(int Target)
{
	int NewNumber = SMCPool.Num() + 1;

	UStaticMeshComponent* NewMeshComp = NewObject<UStaticMeshComponent>(this, FName(*FString::Printf(TEXT("Wall Mesh %i"), NewNumber)));
	NewMeshComp->RegisterComponent();
	NewMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SMCPool.Add(NewMeshComp);

	// Check if there is now enough component sets added.  If not, recurse
	if (SMCPool.Num() < Target) {
		AddNewStaticMeshComponent(Target);
	}
}

