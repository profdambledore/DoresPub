// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Data/BuildToolData.h"
#include "Data/ObjectData.h"
#include "Dataflow/DataflowEngineTypes.h"

#include "World_BuildingLevel.generated.h"

UCLASS()
class DORESPUB_API AWorld_BuildingLevel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorld_BuildingLevel();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// -- Building Functions --
	// Called to add an array of building meshes to the level
	void AddBuildingObjects(TArray<struct FBuildToolData> DataToBuild);

	// Called to remove an array of building meshes from the level
	// Returns a TArray of the ID's of the removed walls
	TArray<FName> RemoveBuildingObjects(TArray<struct FBuildToolData> DataToRemove);

	// Called to add to the current BuildData
	void AddToBuildData(UStaticMeshComponent* ComponentToAdd, FName ID);

	// Called to update the ID of a wall at a location and rotation
	void UpdateWallID(FVector Location, FRotator Rotation, FName NewID);

	// Called to regenerate a BuildData array from scratch
	// Update to store ID's (how?)
	void ReGenerateBuildData();

	/// -- Object Functions --
	// Called to add a new object at a position
	// TO:DO - Update this to use Actors instead, not implemented yet so do that first
	AObject_Parent* AddObjectToLevel(FName ObjectID, FObjectData ObjectToSpawn, FTransform MeshTransform);

	/// -- Uitlity Functions --
	// Called to return the mesh of a static mesh component at a location and rotation
	// If returned nullptr, then none was found
	UStaticMeshComponent* GetWallObjectMeshAtPosition(FVector Location, bool bOnXAxis);

	// Called to return the floor static mesh at a location
	// If returned nullprt, then none was found
	UStaticMeshComponent* GetFloorObjectMeshAtPosition(FVector Location);

	// Called to find the BuildData struct index based on an inputted FVector.
	// Returns an empty struct if one doesn't exist
	int GetBuildDataAtLocation(FVector Location);

	// Called to return a BuildData at a supplied index
	// Returns a pointer to the FBuildData at the index
	FBuildData* GetBuildDataFromIndex(int IndexToUse) { return &BuildData[IndexToUse]; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// -- Uitlity Functions --
	// Called to add a new StaticMeshComponent to the SMCPool
	void AddNewStaticMeshComponent(int Target);

	// Called to add a new object StaticMeshComponent to the ObjectPool
	void AddNewObjectComponent(int Target);

private:
	/// -- Building Functions --
	// Called in GenerateBuildData to insert a new FBuildData struct
	void AddNewBuildData(UStaticMeshComponent* SMC, FName ID = "");

	// Called in GenerateBuildData to update a previous FBuildData struct
	void UpdateBuildData(UStaticMeshComponent* SMC, int Index, FName ID = "");

	// Called to setup a FWallData's default materials 
	void SetupDefaultMaterials(FWallData& WallDataToUpdate);

public:	
	/// -- Components --
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Global Components")
	USceneComponent* Root;

protected:
	/// -- Spawned Components --
	// TArray of all spawned StaticMeshComponents, used for displaying the building before it is built
	TArray<UStaticMeshComponent*> SMCPool;

	// TArray of all spawned object actors
	//TArray<class AObject_Parent*> SpawnedObjects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Build Data")
	TArray<FBuildData> BuildData;
};
