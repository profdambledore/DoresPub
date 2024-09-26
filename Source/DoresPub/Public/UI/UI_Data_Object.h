// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Data/ObjectData.h"
#include "Engine/Texture2D.h"

#include "UI_Data_Object.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DORESPUB_API UCategoryButtonData : public UObject
{
	GENERATED_BODY()

public:
	UCategoryButtonData();
	~UCategoryButtonData();

	void SetupData(class UUI_Player_Object* UI, TEnumAsByte<EObjectType> NewCategory);

public:
	// Pointer back to the Object UI class
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UUI_Player_Object* ObjectUIState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EObjectType> Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;
};


UCLASS(BlueprintType, Blueprintable)
class DORESPUB_API USubCategoryButtonData : public UObject
{
	GENERATED_BODY()

public:
	USubCategoryButtonData();
	~USubCategoryButtonData();

	void SetupData(class UUI_Player_Object* UI, FString SubCatName);

public:
	// Pointer back to the Object UI class
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UUI_Player_Object* ObjectUIState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SubCategory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;
};

UCLASS(BlueprintType, Blueprintable)
class DORESPUB_API UItemButtonData : public UObject
{
	GENERATED_BODY()

public:
	UItemButtonData();
	~UItemButtonData();

	void SetupData(class UUI_Player_Object* UI, FName IDName, UTexture2D* NewItemIcon);

public:
	// Pointer back to the Player Tools class
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UUI_Player_Object* ObjectUIState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemIcon;
};
