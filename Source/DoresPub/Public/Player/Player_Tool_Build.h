// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Player_Tool_Parent.h"

#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"

#include "Data/BuildToolData.h"
#include "Data/OptionsData.h"

#include "Player_Tool_Build.generated.h"

UCLASS()
class DORESPUB_API APlayer_Tool_Build : public APlayer_Tool_Parent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	APlayer_Tool_Build();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// -- Parent Functions Override --
	// Called when the primary tool action is pressed
	virtual void PrimaryActionPressed() override;

	// Called when the primary tool action is released
	virtual void PrimaryActionReleased() override;

	// Called when the primary tool action is pressed
	virtual void SecondaryActionPressed() override;

	// Called when the tool action is ticked
	virtual void ToolTick() override;

	// Called when the tool is equipped
	virtual void OnEquip() override;

	// Called when the tool is unequipped
	virtual void OnUnequip() override;

	// Called to setup the tool when given to the player for the first time
	virtual void SetupTool(class APlayer_Character* NewPC) override;

	// Called to update the rotation of the tool class to re-face the player's camera
	virtual void UpdateToolRotation() override;

	/// -- Tool Functions --
	// Called to update the selected sub-tool
	void UpdateSubTool(TEnumAsByte<EBuildToolSubType> NewSubTool);

	// Called to get the current sub-tool
	TEnumAsByte<EBuildToolSubType> GetSubTool() { return SelectedSubTool; }

	// Called to update if erase mode is enabled or not
	void UpdateEraseMode(bool bOverride = false, bool bOverrideTo = false);

	// Called to get the current state of the erase mode
	bool GetEraseMode() { return bInEraseMode; }

	// Called to update the selected mesh in the BuildToolDisplay
	// Leave empty to clear (nullptr)
	void UpdateSelectedItem(FName WallID = "", TEnumAsByte<EBuildToolSubType> Type = BuildToolSubTypeMax);

	/// -- Display Generation Functions --
	// Called to generate a new display based on the current sub-tool
	void GenerateBuildDisplay(FVector StartPos, FVector EndPos = FVector(0.0f, 0.0f, 0.0f));

	// Called to clear the building display
	void ClearBuildDisplay();

	/// -- Display Utility Functions --
	// Called to setup the initial material istance
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeMaterial();

	// Called to modify the material instance to a new colour
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMaterial(FLinearColor NewColour);

	// Called to update the display's validity
	// Use 0/false for invalid, 1/true for valid and 2 for hidden
	void UpdateDisplayValidity(int NewVisibility);

	// Called to return how many walls are currently being used
	// Return an int of how many SMC's match the correct criteria
	int GetDisplayWallsInUse();

	// Called to return the wall data
	// Returns a TArray of FBuildToolData structs for each valid SMC
	TArray<FBuildToolData> GetDisplayData();

	// Called to return the current size of the display
	// Returns as a pair of ints
	std::pair<int, int> GetDisplaySize();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// -- Tool Tick Functions --
	// Called in OnTick when the Build Tool is selected and the erase mode is selected
	void BuildToolEraseTick();

	// Called in OnTick when the Build Tool is selected and the sub-tool is in Wall mode
	void BuildToolWallModeTick();

	// Called in OnTick when the Build Tool is selected and the sub-tool is in Floor mode
	void BuildToolFloorModeTick();

	// Called in OnTick when the Build Tool is selected and the sub-tool is in Extra mode
	void BuildToolExtraModeTick();

	/// -- Tool Release Functions --
	//  Called in PrimaryReleased when the Build Tool is selected and the sub-tool is in Wall mode
	void BuildToolEraseReleased();

	//  Called in PrimaryReleased when the Build Tool is selected and the sub-tool is in Wall mode
	void BuildToolWallModeReleased();

	//  Called in PrimaryReleased when the Build Tool is selected and the sub-tool is in Floor mode
	void BuildToolFloorModeReleased();

	//  Called in PrimaryReleased when the Build Tool is selected and the sub-tool is in Extra mode
	void BuildToolExtraModeReleased();

	/// -- Display Generation Functions --
	// Called to generate a display of things to erase
	void GenerateEraseDisplay(FVector StartPosition, FVector EndPosition);

	// Called to generate a new wall display
	void GenerateWallDisplay(FVector StartPosition, FVector EndPosition);

	// Called to generate a new floor display
	void GenerateFloorDisplay(FVector StartPosition, FVector EndPosition);

	// Called to generate a new Extra display
	void GenerateExtraDisplay(FVector StartPosition);

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

	/// -- Display Utility Functions --
	// Called to add a new StaticMeshComponent to the BuildToolDisplay
	void AddNewStaticMeshComponent(int Target);

	// Called to check if a point is inside a rectange
	// A is the bottom left point on the rectange, D is the top right
	// Point is the point to check
	bool GetPointInsideRectange(FVector A, FVector D, FVector Point);

public:
	/// -- Display Properties --
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
	/// -- Build Widget Properties --
	// Pointer to the build tool widget class
	class UUI_Tools_BuildWidget* BTW = nullptr;

	// WidgetComponent used to show the player where the build tool is snapping to
	// Widget is set in BP as it has little to no functionallity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Tool Components")
	UWidgetComponent* BuildToolWidgetComponent;

	/// -- Static Mesh Pool Properties --
	// TArray of all spawned StaticMeshComponents, used for displaying the building before it is built
	TArray<UStaticMeshComponent*> SMCPool;

	/// -- Building Tool Properties --
	// Data Table of all available walls
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Data")
	UDataTable* WallDataTable = nullptr;

	// Pointer to the Build State in the player's UI
	class UUI_Player_Build* BuildWidget = nullptr;

private:
	/// -- Building Tool Properties --
	// Enum denoting the current selected sub tool (defaults to Wall)
	TEnumAsByte<EBuildToolSubType> SelectedSubTool = EBuildToolSubType::Wall;

	// Bool denoting if the helper is currently visible
	bool bDisplayVisible = false;

	// bool denoting if the tool is in erase mode
	bool bInEraseMode = false;

	// The size of a wall static mesh in uu
	float WallSize = 250;

	// Int's denoting the current size of the display
	int XSize = 0, YSize = 0;

	// Int denoting the current amount of SMC needed for the Display
	int Total = 0;

	// Pointer to the last hit SMC
	UStaticMeshComponent* LastHitSMC = nullptr;

	// Pointer to the LastHitSMC's mesh, used to return the mesh to the one before replacement
	UStaticMesh* LastHitMesh = nullptr;
};
