// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Data/ObjectData.h"
#include "Engine/Texture2D.h"

#include "UI_Data_Build.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DORESPUB_API UWallButtonData : public UObject
{
	GENERATED_BODY()

public:
	UWallButtonData();
	~UWallButtonData();

	void SetupData(class UUI_Player_Build* UI, FName NewName, UTexture2D* NewWallIcon);

public:
	// Pointer back to the Object UI class
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UUI_Player_Build* BuildUIState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* WallIcon;
};
