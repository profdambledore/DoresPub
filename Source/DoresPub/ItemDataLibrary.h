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

UENUM()
enum EFurnishingType
{
	DefaultType UMETA(Display Name = "Default"),
	Seating UMETA(Display Name = "Seating"),
};

UENUM()
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
