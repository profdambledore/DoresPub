// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Player_Tool_Parent.h"
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

	/// -- Object Tool Properties --
	// Pointer to the current selected mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Tool Properties")
	UStaticMesh* SelectedMesh = nullptr;

	// FName of the ID of the selected object
	FName SelectedID = "";

protected:
	/// -- Object Tool Components --
	// StaticMeshComponent used to show the player where they're placing the new object
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Object Tool Components")
	UStaticMeshComponent* SelectedObjectMeshComponent;

	/// -- Object Tool Properties --
	// bool denoting if the player is in rotation mode
	bool bInRotationMode = false;

	// Bool denoting that the tool is currently placing
	bool bPlacing = false;

	// Pointer to the Build State in the player's UI
	class UUI_Player_Object* ObjectWidget = nullptr;

	// Data Table of all available objects
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Data")
	UDataTable* ObjectDataTable = nullptr;

};
