// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player_Tool_Parent.generated.h"

UCLASS()
class DORESPUB_API APlayer_Tool_Parent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayer_Tool_Parent();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// -- Parent Functions to Override --
	// Called when the primary tool action is pressed
	virtual void PrimaryActionPressed();

	// Called when the primary tool action is released
	virtual void PrimaryActionReleased();

	// Called when the tool action is ticked
	virtual void ToolTick();

	// Called when the tool is equipped
	virtual void OnEquip();

	// Called when the tool is unequipped
	virtual void OnUnequip();

	// Called to setup the tool when given to the player for the first time
	UFUNCTION(BlueprintCallable) // TEMP: Need to find a way to spawn via the player c++ class, currently spawned in BeginPlay of BP
	virtual void SetupTool(class APlayer_Character* NewPC);

	// Called to update the tools rotation
	virtual void UpdateToolRotation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// -- Utility Functions --
	// Called to fire a trace to hit an object, returning FHitResult from the trace
	FHitResult FireTraceToActor();

	// Called to round a float to the nearest multple, returning the output
	float GetNearestMultiple(float Input, int Multiple);

public:	
	/// -- Parent Components --
	// SceneComponent acting as the tool's root
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parent Components")
	USceneComponent* Root = nullptr;

protected:
	/// -- Parent Properties --
	// Pointer back to the Player_Character class that owns this tool
	class APlayer_Character* PC = nullptr;

	// FVector denoting the current location of the tool
	// Saves calling some functions on tick, 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector LastPosition;

	// FVector denoting the click position
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector ClickPosition = FVector(-1, -1, -1);
};
