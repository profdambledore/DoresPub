// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Components/StaticMeshComponent.h"

#include "BuildToolData.generated.h"

// Enum denoting the different types of tools currently available to use
UENUM(BlueprintType, Category = "Tools")
enum EBuildToolSubType
{
	Wall UMETA(DisplayName = "Wall Sub-Tool"),
	Floor UMETA(DisplayName = "Floor Sub-Tool"),
	Extra UMETA(DisplayName = "Extras Sub-Tool"),	
	BuildToolSubTypeMax UMETA(Hidden),
};

// Allows the macro to be used with the EToolType Enum
ENUM_RANGE_BY_COUNT(EBuildToolSubType, EBuildToolSubType::BuildToolSubTypeMax);

USTRUCT(BlueprintType, Category = "Build Tool")
struct FWallData
{
public:
	GENERATED_BODY();

	// Default Constructor/Deconstructor
	FWallData();
	~FWallData();

	// Called to update the ID of this WallData
	void SetNewID(FName NewID);

public:
	// Pointer to the StaticMeshComponent of this wall
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent = nullptr;

	// FName of the wall's ID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName ID = "";

	// TArray of all materials on the wall
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UMaterial*> Materials;
};

USTRUCT(BlueprintType, Category = "Build Tool")
struct FFloorData
{
public:
	GENERATED_BODY();

	// Default Constructor/Deconstructor
	FFloorData();
	~FFloorData();

public:
	// Pointer to the StaticMeshComponent of this wall
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent = nullptr;

	// TArray of all materials on the wall
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterial* Material = nullptr;
};

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

	// FWallData of the wall on the X axis
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FWallData XWallData;

	// FWallData of the wall on the Y axis
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FWallData YWallData;

	// FFloorData of the floor data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FFloorData FloorData;

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

	// TArray of FString denoting item tags
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Tags;

	// bool denoting if the wall's materials can be updated (true if they can)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMaterialUpdatable = true;
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
	FBuildToolData(UStaticMesh* NewMesh, FVector NewLocation, FRotator NewRotation, FName NewName = "");

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

	// The ID of the wall
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build Tool Data")
	FName ID;
};