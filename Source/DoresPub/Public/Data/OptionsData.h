// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "OptionsData.generated.h"

/// -- Erase Mode Options

// Enum denoting the different types of erase mode overlap method to use
UENUM(BlueprintType, Category = "Tools")
enum EEraseOverlapMethod
{
	Partial UMETA(DisplayName = "Partial Overlap"),
	Full UMETA(DisplayName = "Full Overlap"),
	EEraseOverlapMethodMax UMETA(Hidden),
};

USTRUCT(BlueprintType, Category = "Build Tool")
struct FEraseOptions
{
public:
	GENERATED_BODY();

	// Default Constructor/Deconstructor
	FEraseOptions();
	~FEraseOptions();

	// Called to update the bIncludeWalls setting
	void UpdateIncludeWalls(bool bNowTrue);

	// Called to update the bIncludeFloors setting
	void UpdateIncludeFloors(bool bNowTrue);

	// Called to update the OverlapMethod setting
	void UpdateOverlapMethod(TEnumAsByte<EEraseOverlapMethod> NewSetting);

public:
	// Bool denoting if the erase mode should include walls (defaults true)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIncludeWalls = false;

	// Bool denoting if the erase mode should include floors (defaults true)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIncludeFloors = true;

	// TEnum
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<EEraseOverlapMethod> OverlapMethod = Full;
};
