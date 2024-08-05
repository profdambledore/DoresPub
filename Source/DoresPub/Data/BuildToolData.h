// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Components/StaticMeshComponent.h"

#include "BuildToolData.generated.h"

USTRUCT(BlueprintType, Category = "Build Tool")
struct FBuildData
{
public:
	GENERATED_BODY();

	// Default Constructor/Deconstructor
	FBuildData();
	~FBuildData();

	// Called to check if a location argument matches the origin of this struct
	// Returns true if they match, otherwise returns false
	bool GetLocationMatchesOrigin(FVector Location);

public:
	// FLocation denoting the point that this struct is located at
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Origin;

	// Pointer to the StaticMeshComponent rotated on the X axis
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* XStaticMeshComponent = nullptr;

	// Pointer to the StaticMeshComponent rotated on the Y axis
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* YStaticMeshComponent = nullptr;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int Cost = 0;

	// Pointers to other structs in each direction
	FBuildData* UpPoint = nullptr;
	FBuildData* LeftPoint = nullptr;
	FBuildData* DownPoint = nullptr;
	FBuildData* RightPoint = nullptr;
};

USTRUCT(BlueprintType, Category = "Build Tool")
struct FSelectableWallData : public FTableRowBase
{
public:
	GENERATED_BODY();

	// Default Constructor/Deconstructor
	FSelectableWallData();
	~FSelectableWallData();


public:
	// FString denoting the walls name
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	// FString denoting a brief description of the wall
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	// Texture denoting a picture of the wall
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	// StaticMesh of the wall, used in the world
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	// Int denoting the price of the wall per unit placed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Price;
};

USTRUCT(BlueprintType, Category = "Build Tool")
struct DORESPUB_API FBuildToolData
{
public:
	GENERATED_BODY();

	// Default Constructor/Deconstructor
	FBuildToolData();
	~FBuildToolData();

	// Constructor which allows the data to be setup on construction
	FBuildToolData(UStaticMesh* NewMesh, FVector NewLocation, FRotator NewRotation, int NewCost = 0);

public:
	// The mesh of the SMC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build Tool Data")
	UStaticMesh* Mesh;

	// The location of the SMC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build Tool Data")
	FVector Location;

	// The rotation of the SMC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build Tool Data")
	FRotator Rotation;

	// The cost of the SMC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build Tool Data")
	int Cost;
};