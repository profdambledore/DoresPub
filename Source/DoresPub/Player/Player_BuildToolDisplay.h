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

	/// -- Global Functions --
	// Called to update the display's validity
	// Use 0/false for invalid, 1/true for valid and 2 for hidden
	void UpdateDisplayValidity(int NewVisibility);

	// Called to update the selected sub-tool
	void UpdateSubTool(TEnumAsByte<EBuildToolSubType> NewSubTool);

	// Called to get the current sub-ttol
	TEnumAsByte<EBuildToolSubType> GetSubTool();

	// Called to return how many walls are currently being used
	// Return an int of how many SMC's match the correct criteria
	int GetDisplayWallsInUse();

	// Called to return the wall data
	// Returns a TArray of FBuildToolData structs for each valid SMC
	TArray<FBuildToolData> GetDisplayData();

	// Called to return the current size of the display
	// Returns as a pair of ints
	std::pair<int, int> GetDisplaySize();

	/// -- Display Building Functions --
	// Called to generate a new display based on the current sub-tool
	void GenerateBuildDisplay(FVector StartPos, FVector EndPos = FVector(0.0f, 0.0f, 0.0f));

	// Called to generate a new wall display
	void GenerateWallDisplay(FVector StartPosition, FVector EndPosition);

	// Called to generate a new floor display
	void GenerateFloorDisplay(FVector StartPosition, FVector EndPosition);

	// Called to generate a new window display
	void GenerateWindowDisplay(FVector StartPosition);

	// Called to clear the building display
	void ClearBuildDisplay();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// -- Display Functions --
	// Called to create a line of static meshes along the X axis
	// Start is the X of the start position, End is the X of the end position
	// Y is the Y co-ord of the start position, AmountOfSegements is the amount of static meshes to draw
	// PoolStartIndex is the position to start taking components from in the static mesh pool
	// MeshOverride is the mesh set in the components (will use SelectedMesh if left empty)
	void CreateLineOnX(float Start, float End, float Y, int AmountOfSegements, int PoolStartIndex, UStaticMesh* MeshOverride = nullptr);

	// Called to create a line of static meshes along the Y axis
	// Start is the Y of the start position, End is the Y of the end position
	// X is the X co-ord of the start position, AmountOfSegements is the amount of static meshes to draw
	// PoolStartIndex is the position to start taking components from in the static mesh pool
	// MeshOverride is the mesh set in the components (will use SelectedMesh if left empty)
	void CreateLineOnY(float Start, float End, float X, int AmountOfSegements, int PoolStartIndex, UStaticMesh* MeshOverride = nullptr);

	// Called to create a pillar at a series of points
	void CreatePillars(FVector Start, FVector End);

	/// -- Uitlity Functions --
	// Called to add a new StaticMeshComponent to the BuildToolDisplay
	void AddNewStaticMeshComponent(int Target);

public:	
	/// -- Components --
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Global Components")
	USceneComponent* Root;

	/// -- Global Properties --
	// Pointer to the Build Tool Material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global Properties")
	UMaterialInstanceDynamic* BuildToolMaterial = nullptr;

	// -- Material Colours --
	UPROPERTY(EditDefaultsOnly, Category = "Global Properties")
	FLinearColor ValidColour;

	UPROPERTY(EditDefaultsOnly, Category = "Global Properties")
	FLinearColor InvalidColour;

	UPROPERTY(EditDefaultsOnly, Category = "Global Properties")
	FLinearColor HiddenColour;

	// bool denoting if the tool is in erase mode
	bool bInEraseMode = false;

	/// -- Wall Sub Tool Properties --
	// Pointer to the current selected mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Sub Tool Properties")
	UStaticMesh* SelectedMesh = nullptr;

	// FName of the ID of the selected wall
	FName SelectedID = "";

	/// -- Floor Sub Tool Properties --
	// Pointer to the floor mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor Sub Tool Properties")
	UStaticMesh* FloorTileMesh = nullptr;

protected:
	/// -- Spawned Components --
	// TArray of all spawned StaticMeshComponents, used for displaying the building before it is built
	TArray<UStaticMeshComponent*> SMCPool;

	// TEMP
	// Pointer to the World Building Level
	class AWorld_BuildingLevel* GroundFloor = nullptr;

private:
	// Enum denoting the current selected sub tool (defaults to Wall)
	TEnumAsByte<EBuildToolSubType> SelectedSubTool = EBuildToolSubType::Wall;

	// Bool denoting if the helper is currently visible
	bool bDisplayVisible = false;

	// The size of a wall static mesh in uu
	float WallSize = 250;

	// Int's denoting the current size of the display
	int XSize, YSize;

	// Int denoting the current amount of SMC needed for the Display
	int Total = 0;
};
