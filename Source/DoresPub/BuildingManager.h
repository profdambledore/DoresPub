// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Engine/DataTable.h"

#include "ItemDataLibrary.h"

#include "BuildingManager.generated.h"

class AParentFurnishing;

UCLASS()
class DORESPUB_API ABuildingManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildingManager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		bool SpawnFurnishingAtLocation(FName FurnishingID, FVector WorldLocation, FRotator WorldRotation);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Player Reference
	class AEditorPlayer* EditorPlayer = nullptr;

	// Data Table
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	UDataTable* FurnishingDataTable = nullptr;

	// Array of all objects in the world
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<AParentFurnishing*> FurnishingInWorld;
};
