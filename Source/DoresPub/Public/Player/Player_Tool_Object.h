// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Player_Tool_Parent.h"

#include "Dataflow/DataflowEngineTypes.h"

#include "Player_Tool_Object.generated.h"

UCLASS()
class DORESPUB_API APlayer_Tool_Object : public APlayer_Tool_Parent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	APlayer_Tool_Object();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// -- Parent Functions Override --
	// Called when the primary tool action is pressed
	virtual void PrimaryActionPressed() override;

	// Called when the primary tool action is released
	virtual void PrimaryActionReleased() override;

	// Called when the primary tool action is pressed
	virtual void SecondaryActionPressed() override;

	// Called when the primary tool action is released
	virtual void SecondaryActionReleased() override;

	// Called when the delete tool action is released
	virtual void DeleteAction() override;

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

	/// -- Object Tool Functions --
	// Called to update the current mesh shown (leave blank to clear)
	void UpdateObjectMesh(FName NewID = "");

	// Called to toggle the from rotation mode to normal mode (and vice versa)
	void ToggleRotationMode();

	// Called to get the object data table asset
	UDataTable* GetObjectDataTable() { return ObjectDataTable; }


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSOMCBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSOMCEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	// Called to access a data table row from the ObjectDataTable
	struct FObjectData* GetObjectDataRow(FName ID) { return ObjectDataTable->FindRow<FObjectData>(ID, "", false); }

	// Called to get if the placement is valid
	bool GetPlacementIsValid() { return (bValidOverlap || bSnapping) && bValidPlacement; }

public:
	// Pointer to the last object hit by a trace
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Object Query")
	class AObject_Parent* LastObjectHitByTrace = nullptr;

	// Int Index to the last wall hit by a trace
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Object Query")
	int LastWallHitByTrace = -1;

protected:
	/// -- Object Tool Components --
	// StaticMeshComponent used to show the player where they're placing the new object
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Object Tool Components")
	UStaticMeshComponent* SelectedObjectMeshComponent;

	/// -- Object Tool Properties --
	// Pointer to the current selected mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Tool Properties")
	UStaticMesh* SelectedMesh = nullptr;

	// bool denoting if the player is in rotation mode
	bool bInRotationMode = false;

	// Bool denoting that the tool is currently placing
	bool bPlacing = false;

	// Bool denoting that the tool is currently snapping
	bool bSnapping = false;

	// Pointer to the Build State in the player's UI
	class UUI_Player_Object* ObjectWidget = nullptr;

	// Float denoting the snapping distance (how far the trace needs to be from the snap point to use)
	float SnapDistance = 50.0f;

	// Data Table of all available objects
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Data")
	UDataTable* ObjectDataTable = nullptr;

	// UMaterial* denoting an invalid placement
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UMaterial* InvalidMaterial = nullptr;

	/// -- Selected Object Properties --
	// FName of the ID of the selected object
	FName SelectedID = "";

	// Pointer to the selected object
	class AObject_Parent* SelectedObject = nullptr;

	// FTransform of the selected object's original transform
	FTransform SelectedObjectPrevious;

	// UMaterial denoting the selected object's material
	TArray<UMaterialInterface*> SelectedObjectMaterials;

	/// -- Object Tag Properties --
	// FString denoting the objects placement type
	FString ObjectPlacementType = "";

	// FString denoting what objects the current object can snap to
	FString ObjectSnapType = "";

	// TArray of available snapping points
	TArray<FObjectSnappingData> SnappingTransforms;

	/// -- Object Query Properties --
	// TArray of AActors that the selected object is currently overlapping
	TArray<AActor*> OverlappedActors;

	// Bool denoting if the placement would overlap other actors
	bool bValidOverlap = true;

	bool bValidPlacement = true;

};
