// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Player_Tools.generated.h"

UENUM(BlueprintType, Category = "Tools")
enum EToolType
{
	Default UMETA(DisplayName="Default Tool"),
	Building UMETA(DisplayName = "Building Tool"),
	Object UMETA(DisplayName = "Object Tool")
};

ENUM_RANGE_BY_COUNT(EToolType, EToolType::Object);

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
	void SelectedToolPrimary();

	// Called to fire the primary tool press function of the selected tool
	void SelectedToolPrimaryPressed();

	// Called to fire the primary tool release function of the selected tool
	void SelectedToolPrimaryReleased();

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

	// Int denoting the price of a wall (half for a half wall)
	// Add 2 zeros at end, as currency is int
	int HalfWallCost = 1500;

	// bool denoting if the player is in erase mode
	bool bInEraseMode = false;

	/// -- Object Tool Properties --
	// bool denoting if the player is in rotation mode
	bool bInRotationMode = false;
};
