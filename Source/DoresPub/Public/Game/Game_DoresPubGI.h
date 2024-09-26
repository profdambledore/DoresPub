// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Data/OptionsData.h"

#include "Game_DoresPubGI.generated.h"

UCLASS()
class DORESPUB_API UGame_DoresPubGI : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UGame_DoresPubGI();

	// Called to return the erase options as a reference
	FEraseOptions& GetEraseOptions();
	
protected:
	/// -- Player Options --
	// Struct containing all the EraseMode options
	FEraseOptions EraseOptions;
};
