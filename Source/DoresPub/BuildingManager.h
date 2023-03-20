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
		bool SpawnFurnishingAtLocation(int Floor, FName FurnishingID, FVector WorldLocation, FRotator WorldRotation);

	UFUNCTION(BlueprintCallable)
		void AddFurnishingToFloorData(int Floor, AParentFurnishing* Object);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Player Reference
	class AEditorPlayer* EditorPlayer = nullptr;

	// Data Table
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	UDataTable* FurnishingDataTable = nullptr;

	// Array of world data (the area where the player can build)
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	TArray<FWorldFloorData> WorldData;

	// Map of each floor's data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TMap<int, FBuildingFloorData> FloorData;

private:
	// Struct used when adding new data (may not be needed)
	FBuildingFloorData NewData;
};
