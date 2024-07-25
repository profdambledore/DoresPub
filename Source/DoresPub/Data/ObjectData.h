// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"

#include "ObjectData.generated.h"

UENUM(BlueprintType)
enum EObjectType
{
	DefaultType UMETA(DisplayName = "Default"),
	Seating UMETA(DisplayName = "Seating"),
	Tables UMETA(DisplayName = "Tables"),
	Count UMETA(Hidden),
};

ENUM_RANGE_BY_COUNT(EObjectType, EObjectType::Count)

USTRUCT(BlueprintType)
struct FObjectData : public FTableRowBase
{
public:
	GENERATED_BODY();

	FObjectData();
	~FObjectData();


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EObjectType> Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SubCategory;

	// Class
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//TSubclassOf<class AParentFurnishing> Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Price;
};

USTRUCT(BlueprintType)
struct FSubCats
{
public:
	GENERATED_BODY();

	FSubCats();
	~FSubCats();


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> X;
};