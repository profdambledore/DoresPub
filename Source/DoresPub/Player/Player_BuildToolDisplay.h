// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/StaticMeshComponent.h"

#include "Data/BuildToolData.h"

#include "Player_BuildToolDisplay.generated.h"

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

	/// -- Display Functions --
	// Called to update the display's validity
	void UpdateDisplayValidity(bool bIsValid);

	// Called to return how many walls are currently being used
	// Return an int of how many SMC's match the correct criteria
	int GetDisplayWallsInUse();

	// Called to return the wall data
	// Returns a TArray of FBuildToolData structs for each valid SMC
	TArray<FBuildToolData> GetDisplayData();

	// Called to generate a new building display
	void GenerateNewBuildDisplay(FVector StartPosition, FVector EndPosition);

	// Called to clear the building display
	void ClearBuildDisplay();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// -- Display Functions --
	// Called to create a wall along the X axis
	void CreateXWall(float Start, float End, float Y, int WallSegments, int StartWallSegment);

	// Called to create a wall along the Y axis
	void CreateYWall(float Start, float End, float X, int WallSegments, int StartWallSegment);

	// Called to create a pillar at a series of points
	void CreatePillars(FVector Start, FVector End);

	/// -- Uitlity Functions --
	// Called to add a new StaticMeshComponent to the BuildToolDisplay
	void AddNewStaticMeshComponent(int Target);

public:	
	/// -- Components --
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Global Components")
	USceneComponent* Root;

	/// -- Meshes --
	// Pointer to the current selected mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Static Meshes")
	UStaticMesh* SelectedMesh = nullptr;

	// FName of the ID of the selected wall
	FName SelectedWallID = "";

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
	float WallSize = 250;

	// Int denoting the current amount of SMC needed for the Display
	int Total = 0;
};
