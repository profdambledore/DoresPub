// Fill out your copyright notice in the Description page of Project Settings.

#include "ParentFurnishing.h"
#include "EditorPlayer.h"
#include "BuildingManager.h"

// Sets default values
ABuildingManager::ABuildingManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Get Data Table object and store it
	ConstructorHelpers::FObjectFinder<UDataTable>DTObject(TEXT("/Game/Furnishing/DT_FurnishingItem"));
	if (DTObject.Succeeded()) { FurnishingDataTable = DTObject.Object; }
}

// Called when the game starts or when spawned
void ABuildingManager::BeginPlay()
{
	Super::BeginPlay();

	bool test = SpawnFurnishingAtLocation(FName("SEA_SOF_001"), FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
	if (test == false) {UE_LOG(LogTemp, Warning, TEXT("Not Spawned")) }
}

// Called every frame
void ABuildingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ABuildingManager::SpawnFurnishingAtLocation(FName FurnishingID, FVector WorldLocation, FRotator WorldRotation)
{
	// Get Object from Data Table
	FFurnishingItem* item = FurnishingDataTable->FindRow<FFurnishingItem>(FurnishingID, "", false);
	// Check if we found an item, if we did, continue...
	if (item == nullptr) { return false; }

	// Spawn data for the furnishing
	FActorSpawnParameters furnishingSpawnParams;
	furnishingSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;;

	// Spawn the item in the world
	AParentFurnishing* newFurnishing = GetWorld()->SpawnActor<AParentFurnishing>(item->Class, WorldLocation, WorldRotation, furnishingSpawnParams);
	newFurnishing->Mesh->SetStaticMesh(item->Mesh);
	
	// Set the new objects data
	// TODO - Find a better way of doing this
	newFurnishing->Data.Name = item->Name;
	newFurnishing->Data.Description = item->Description;
	newFurnishing->Data.Category = item->Category;
	newFurnishing->Data.SubCategory = item->SubCategory;
	newFurnishing->Data.Icon = item->Icon;
	newFurnishing->Data.Price = item->Price;

	// However, set the data not needed to nullptr
	// This is stored somewhere else in the object (class is itself, mesh on Mesh component)
	newFurnishing->Data.Mesh = nullptr;
	newFurnishing->Data.Class = nullptr;

	return true;
}