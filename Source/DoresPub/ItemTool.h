// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParentTool.h"
#include "Materials/Material.h"
#include "ItemTool.generated.h"

/**
 * 
 */
UCLASS()
class DORESPUB_API UItemTool : public UParentTool
{
	GENERATED_BODY()

public:
	UItemTool();
	virtual void ToolPrimary();
	virtual void ToolSecondary();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterial* FurnishingMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterial* ErrorMaterial;
	
};
