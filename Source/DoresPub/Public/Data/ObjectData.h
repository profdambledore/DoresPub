// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Dataflow/DataflowEngineTypes.h"

#include "ObjectData.generated.h"

// Enum denoting the different types of objects available to place
// Also acts as a main category of the objects
UENUM(BlueprintType)
enum EObjectType
{
	DefaultType UMETA(DisplayName = "Default"),
	Seating UMETA(DisplayName = "Seating"),
	Tables UMETA(DisplayName = "Tables"),
	TestObjects UMETA(DisplayName = "TestObjects"),
	ObjectTypeMax UMETA(Hidden),
};

// Allows the macro to be used with the EObjectType Enum
ENUM_RANGE_BY_COUNT(EObjectType, EObjectType::ObjectTypeMax)

USTRUCT(BlueprintType)
struct FObjectData : public FTableRowBase
{
public:
	GENERATED_BODY();

	// Default Constructor/Deconstructor
	FObjectData();
	~FObjectData();

public:
	// FString denoting the object name
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	// FString denoting a brief description of the object
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	// EObjectType denoting the main category of the object
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EObjectType> Category;

	// FString denoting the subcategory of the object
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SubCategory;

	// Class
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//TSubclassOf<class AParentFurnishing> Class;

	// Texture denoting a picture of the object
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	// StaticMesh of the object, used in the world
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	// Int denoting the price of the object per unit placed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Price;

	// TArray of FString denoting objecy tags
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStringValuePair> ObjectTags;
};

USTRUCT(BlueprintType)
struct FSubCats
{
public:
	GENERATED_BODY();

	// Default Constructor/Deconstructor
	FSubCats();
	~FSubCats();

public:
	// Array of FStrings, to allows a TArray in a TMap
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> X;
};