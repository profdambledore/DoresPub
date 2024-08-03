// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/StaticMeshComponent.h"

#include "Player_BuildToolDisplay.generated.h"

USTRUCT(BlueprintType, Category = "Levels")
struct DORESPUB_API FBuildingData
{
public:
	GENERATED_BODY();

	FBuildingData();
	FBuildingData(UStaticMesh* NewMesh, FVector NewLocation, FRotator NewRotation);

	~FBuildingData();

public:
	// The mesh of the SMC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
	UStaticMesh* Mesh;

	// The location of the SMC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
	FVector Location;

	// The rotation of the SMC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
	FRotator Rotation;
};

UCLASS()
class DORESPUB_API APlayer_BuildToolDisplay : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayer_BuildToolDisplay();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// -- Material Functions --
	// Called to setup the initial material istance
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeMaterial();

	// Called to modify the material instance to a new colour
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMaterial(FLinearColor NewColour);

	// Called to update the display's validity
	void UpdateDisplayValidity(bool bIsValid);

	// Called to return how many walls are currently being used
	// Returns as half walls
	int GetDisplayWallsInUse();

	// Called to generate a new building display
	void GenerateNewBuildDisplay(FVector StartPosition, FVector EndPosition);

	// Called to clear the building display
	void ClearBuildDisplay();

	// Called to return the wall data
	TArray<FBuildingData> GetDisplayData();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// -- Uitlity Functions --
	// Called to add a new StaticMeshComponent to the BuildToolDisplay
	void AddNewStaticMeshComponent(int Target);

	// Called to create a wall along the X axis
	void CreateXWall(float Start, float End, float Y, int WallSegments, int StartWallSegment);

	// Called to create a wall along the Y axis
	void CreateYWall(float Start, float End, float X, int WallSegments, int StartWallSegment);

	// Called to create a pillar at a series of points
	void CreatePillars(FVector Start, FVector End);

	// Called to check if the component is overlapping a matching static mesh
	bool GetOverlappingPlacedMesh(UStaticMeshComponent* BuildToolComponent);

public:	
	/// -- Components --
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Global Components")
	USceneComponent* Root;

	/// -- 
	// Pointer to the Wall object
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Static Meshes")
	UStaticMesh* WallMesh = nullptr;

	// Pointer to the Half Wall object
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Static Meshes")
	UStaticMesh* HalfWallMesh = nullptr;

	// Pointer to the Corner Pillar Wall object
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Static Meshes")
	UStaticMesh* PillarWallMesh = nullptr;

	// Pointer to the Build Tool Material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build Tool Material")
	UMaterialInstanceDynamic* BuildToolMaterial = nullptr;

	/// -- Material Colours --
	UPROPERTY(EditDefaultsOnly, Category = "Material Colours")
	FLinearColor ValidColour;

	UPROPERTY(EditDefaultsOnly, Category = "Material Colours")
	FLinearColor InvalidColour;

	/// -- Tool Options --
	// bool denoting if the tool is in erase mode
	bool bInEraseMode = false;

protected:
	/// -- Spawned Components --
	// TArray of all spawned StaticMeshComponents, used for displaying the building before it is built
	TArray<UStaticMeshComponent*> SMCPool;

	// TEMP
	// Pointer to the World Building Level
	class AWorld_BuildingLevel* GroundFloor = nullptr;

private:
	// Bool denoting if the helper is currently visible
	bool bDisplayVisible = false;

	// The size of a wall static mesh in uu
	float WallSize = 125;

	// Int denoting the current amount of SMC needed for the Display
	int Total = 0;
};
