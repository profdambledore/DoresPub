// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Data/BuildingData.h"

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
	void AddBuildingObjects(TArray<struct FBuildingData> DataToBuild);

	// Called to return the mesh of a static mesh component at a location and rotation
	// If returned nullptr, then none was found
	UStaticMeshComponent* GetWallObjectMeshAtPosition(FVector Location, bool bOnXAxis);

	// Called to regenerate a BuildData array from scratch
	void ReGenerateBuildData();

	// Called to find the BuildData struct index based on an inputted FVector.
	// Returns an empty struct if one doesn't exist
	int GetBuildDataAtLocation(FVector Location);

	/// -- Object Functions --
	// Called to add a new object at a position
	// TO:DO - Update this to use Actors instead, not implemented yet so do that first
	void AddObjectToLevel(UStaticMesh* MeshToSpawn, FTransform MeshTransform);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// -- Uitlity Functions --
	// Called to add a new StaticMeshComponent to the BuildToolDisplay
	void AddNewStaticMeshComponent(int Target);

private:
	// Called in GenerateBuildData to insert a new FBuildData struct
	void AddNewBuildData(UStaticMeshComponent* SMC);

	// Called in GenerateBuildData to update a previous FBuildData struct
	void UpdateBuildData(UStaticMeshComponent* SMC, int Index);

public:	
	/// -- Components --
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Global Components")
	USceneComponent* Root;

protected:
	/// -- Spawned Components --
	// TArray of all spawned StaticMeshComponents, used for displaying the building before it is built
	TArray<UStaticMeshComponent*> SMCPool;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Build Data")
	TArray<FBuildData> BuildData;
};
