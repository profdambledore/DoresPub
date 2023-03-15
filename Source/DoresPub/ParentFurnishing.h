// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/StaticMeshComponent.h"

#include "ItemDataLibrary.h"
#include "FurnishingInterface.h"

#include "ParentFurnishing.generated.h"

UCLASS()
class DORESPUB_API AParentFurnishing : public AActor, public IFurnishingInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParentFurnishing();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual FString GetObjectData();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// COMPONENTS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* Mesh;

	// Data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
		FFurnishingItem Data;
};
