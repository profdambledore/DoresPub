// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World_BuildingLevel.generated.h"

UCLASS()
class DORESPUB_API AWorld_BuildingLevel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorld_BuildingLevel();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// -- Building Functions --
	// Called to add an array of building meshes to the level
	void AddBuildingObjects(TArray<struct FBuildingData> DataToBuild);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// -- Uitlity Functions --
	// Called to add a new StaticMeshComponent to the BuildToolDisplay
	void AddNewStaticMeshComponent(int Target);

public:	
	/// -- Components --
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Global Components")
	USceneComponent* Root;

protected:
	/// -- Spawned Components --
	// TArray of all spawned StaticMeshComponents, used for displaying the building before it is built
	TArray<UStaticMeshComponent*> SMCPool;
};
