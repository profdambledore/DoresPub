// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Data/ObjectData.h"
#include "Engine/Texture2D.h"

#include "UI_Data_Build.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DORESPUB_API UBuildSelectButtonData : public UObject
{
	GENERATED_BODY()

public:
	UBuildSelectButtonData();
	~UBuildSelectButtonData();

	void SetupData(class UUI_Player_Build* UI, FName NewName, UTexture2D* NewWallIcon, TEnumAsByte<EBuildToolSubType> NewSubType);

public:
	// Pointer back to the Object UI class
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UUI_Player_Build* BuildUIState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* WallIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EBuildToolSubType> SubType;
};
