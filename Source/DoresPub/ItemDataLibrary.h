// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Engine/DataTable.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"

#include "ItemDataLibrary.generated.h"

UCLASS()
class DORESPUB_API UItemDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};

UENUM(BlueprintType)
enum EFurnishingType
{
	DefaultType UMETA(Display Name = "Default"),
	Seating UMETA(Display Name = "Seating"),
};

UENUM(BlueprintType)
enum EFurnishingSubType
{
	DefaultSubType UMETA(Display Name = "Default"),
	Seating_Sofa UMETA(Display Name = "Seating Sofa"),
	Seating_Bench UMETA(Display Name = "Seating Bench"),
};

USTRUCT(BlueprintType)
struct FFurnishingItem : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EFurnishingType> Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EFurnishingSubType> SubCategory;

	// Class
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AParentFurnishing> Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Price;
};

USTRUCT(BlueprintType)
struct FCategoryBtn
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EFurnishingType> Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Icon;
};

USTRUCT(BlueprintType)
struct FSubCategoryBtn
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EFurnishingSubType> SubCategory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Icon;
};

USTRUCT(BlueprintType)
struct FFurnishingSlot
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Icon;
};

USTRUCT(BlueprintType)
struct FWorldFloorData
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int GridX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int GridY;
};

USTRUCT(BlueprintType)
struct FBuildingFloorData
{
	GENERATED_USTRUCT_BODY();

public:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// WallParent

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// CeilingParent

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FloorParent

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AParentFurnishing*> Furnishings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FWorldFloorData> FloorGrid;
};

USTRUCT(BlueprintType)
struct FLevelData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FWorldFloorData> BaseFloorGrid;
};