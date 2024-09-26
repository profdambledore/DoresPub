// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Player_Tools.generated.h"

// Allows the macro to be used with the EToolType Enum
ENUM_RANGE_BY_COUNT(EToolType, EToolType::ToolCount);

UCLASS()
class DORESPUB_API APlayer_Tools : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayer_Tools();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// -- Global Tool Functions --
	// Called to set the Player Character pointer
	void SetupTools(class APlayer_Character* NewPC);

	// Called to swap to a selected tool
	void SwapTool(TEnumAsByte<EToolType> NewTool);

	// Called to update the rotation of the tool class to re-face the player's camera
	void UpdateToolRotation();

	// Called to fire the primary tool function of the selected tool
	// On first press, fire the pressed event.  On second press, fire the released event
	void SelectedToolPrimary();

	// Called to fire the primary tool press function of the selected tool
	void SelectedToolPrimaryPressed();

	// Called to fire the primary tool release function of the selected tool
	void SelectedToolPrimaryReleased();

	/// -- Build Tool Function --
	// Called to toggle the erase mode to normal mode (and vice versa)
	void ToggleEraseMode(bool bOverride = false, bool bOverrideTo = false);

	// Called to toggle the erase mode to normal mode (and vice versa)
	void SwapSubTool(TEnumAsByte<EBuildToolSubType> NewSubTool);

	// Called to update the selected mesh in the BuildToolDisplay
	// Leave empty to clear (nullptr)
	void UpdateSelectedItem(FName WallID = "", TEnumAsByte<EBuildToolSubType> Type = BuildToolSubTypeMax);

	/// -- Object Tool Functions --
	// Called to update the current mesh shown (leave blank to clear)
	void UpdateObjectMesh(UStaticMesh* NewStaticMesh = nullptr);

	// Called to toggle the from rotation mode to normal mode (and vice versa)
	void ToggleRotationMode();

	/// -- Utility Functions --
	// Called to fire a trace to hit an object, returning FHitResult from the trace
	FHitResult FireTraceToActor();

	// Called to round a float to the nearest multple, returning the output
	float GetNearestMultiple(float Input, int Multiple);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// -- Build Tool Tick Functions --
	// Called in OnTick when the Build Tool is selected and the erase mode is selected
	void BuildToolEraseTick();

	// Called in OnTick when the Build Tool is selected and the sub-tool is in Wall mode
	void BuildToolWallModeTick();

	// Called in OnTick when the Build Tool is selected and the sub-tool is in Floor mode
	void BuildToolFloorModeTick();

	// Called in OnTick when the Build Tool is selected and the sub-tool is in Window mode
	void BuildToolWindowModeTick();

	/// -- Build Tool Release Functions --
	//  Called in PrimaryReleased when the Build Tool is selected and the sub-tool is in Wall mode
	void BuildToolWallModeReleased();

	//  Called in PrimaryReleased when the Build Tool is selected and the sub-tool is in Floor mode
	void BuildToolFloorModeReleased();

	//  Called in PrimaryReleased when the Build Tool is selected and the sub-tool is in Window mode
	void BuildToolWindowModeReleased();

public:	
	/// -- Global Components --
	// SceneComponent acting as the root component
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Global Components")
	USceneComponent* Root;

	/// -- Building Tool Components --
	// WidgetComponent used to show the player where the build tool is snapping to
	// Widget is set in BP as it has little to no functionallity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Tool Components")
	UWidgetComponent* BuildToolWidgetComponent;

	/// -- Object Tool Components --
	// StaticMeshComponent used to show the player where they're placing the new object
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Tool Components")
	UStaticMeshComponent* ObjectToolMeshComponent;

protected:
	/// -- Pointers --
	// Pointer to the player character
	class APlayer_Character* PC = nullptr;

	// Pointer to the spawned BuildToolDisplay
	class APlayer_BuildToolDisplay* BTD = nullptr;

	// Pointer to the build tool widget class
	class UUI_Tools_BuildWidget* BTW = nullptr;

	// TEMP
	// Pointer to the World Building Level
	class AWorld_BuildingLevel* GroundFloor = nullptr;

	/// -- Global Tool Properties --
	// Enum denoting the current tool selected
	TEnumAsByte<EToolType> CurrentTool = EToolType::Default;

	// FVector denoting the current location of the tool
	// Saves calling some functions on tick, 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector LastPosition;

	// FVector denoting the click position
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector ClickPosition = FVector(-1, -1, -1);

	// Bool denoting if the PrimaryToolButton is currently being held
	bool bPrimaryHeld = false;

	/// -- Building Tool Properties --
	// Class of the BuildToolDisplay
	TSubclassOf<class APlayer_BuildToolDisplay>BuildToolDisplayClass = nullptr;

	// Float denoting the multiplier of the refund
	float RefundMultiplier = 0.8;

	// bool denoting if the player is in erase mode
	bool bInEraseMode = false;

	// Data Table of all available walls
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Data")
	UDataTable* WallDataTable = nullptr;

	// Pointer to the last hit SMC
	UStaticMeshComponent* LastHitSMC = nullptr;

	// Pointer to the LastHitSMC's mesh, used to return the mesh to the one before replacement
	UStaticMesh* LastHitMesh = nullptr;

	/// -- Object Tool Properties --
	// bool denoting if the player is in rotation mode
	bool bInRotationMode = false;
};
