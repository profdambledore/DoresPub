// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParentTool.h"
#include "BuildTool.generated.h"

/**
 * 
 */
UCLASS()
class DORESPUB_API UBuildTool : public UParentTool
{
	GENERATED_BODY()

public:
	UBuildTool();

	virtual void ToolPrimary();
	virtual void ToolSecondary();

public:

	
};
