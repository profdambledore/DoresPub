// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/StaticMeshComponent.h"

#include "BuildingData.generated.h"

USTRUCT(BlueprintType)
struct FBuildData
{
public:
	GENERATED_BODY();

	FBuildData();
	~FBuildData();

	bool GetLocationMatchesOrigin(FVector Location);


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Origin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* XStaticMeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bXHalfWall = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* YStaticMeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bYHalfWall = false;

	// Pointers to other structs
	FBuildData* UpPoint = nullptr;
	FBuildData* LeftPoint = nullptr;
	FBuildData* DownPoint = nullptr;
	FBuildData* RightPoint = nullptr;


};