// Fill out your copyright notice in the Description page of Project Settings.

#include "ParentFurnishing.h"
#include "EditorPlayer.h"
#include "GridWidget.h"
#include "BuildingManager.h"

// Sets default values
ABuildingManager::ABuildingManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Get Data Table object and store it
	ConstructorHelpers::FObjectFinder<UDataTable>DTObject(TEXT("/Game/Furnishing/DT_FurnishingItem"));
	if (DTObject.Succeeded()) { FurnishingDataTable = DTObject.Object; }

	// Create the Grid Widget and add the Blueprint Class
	GridWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Grid Widget"));
	ConstructorHelpers::FClassFinder<UGridWidget>GClass(TEXT("/Game/UI/Grid/WBP_Grid"));
	if (GClass.Succeeded()) { UE_LOG(LogTemp, Warning, TEXT("Succ"));  foundclass=GClass.Class; }
}

// Called when the game starts or when spawned
void ABuildingManager::BeginPlay()
{
	Super::BeginPlay();

	// Set the widget class
	GridWidget->SetWidgetClass(foundclass);

	// Setup the pointer to the widget class
	WidgetClass = Cast<UGridWidget>(GridWidget->GetUserWidgetObject());
	if (WidgetClass == nullptr) {UE_LOG(LogTemp, Warning, TEXT("nullptr")) }
	SetupGrid(3, 3, FVector(0.0f, 0.0f, 0.0f));
}

// Called every frame
void ABuildingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ABuildingManager::SpawnFurnishingAtLocation(int Floor, FName FurnishingID, FVector WorldLocation, FRotator WorldRotation)
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

	// Add the data to the map (update with floor)
	AddFurnishingToFloorData(Floor, newFurnishing);

	// Return true
	return true;
}

void ABuildingManager::AddFurnishingToFloorData(int Floor, AParentFurnishing* Object){
	// Clear the NewData Struct and get the data already in the map
	NewData = {};	
	NewData = FloorData.FindRef(Floor);

	// Add the new furnishing to the data, and re-set it in the map
	NewData.Furnishings.Add(Object);
	FloorData.Add(Floor, NewData);
}

void ABuildingManager::SetupGrid(int Width, int Height, FVector Origin)
{
	// Modify the widget component to match the grid size
	GridWidget->SetDrawSize(FVector2D((Width + 1) * 250, (Height + 1) * 250));

	// Set the grid's rotation too
	GridWidget->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	GridWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 21.0f));

	WidgetClass->SetupGridWidget(Width, Height);
}

void ABuildingManager::UpdateGridSize(int Width, int Height, float NewZ)
{
}
