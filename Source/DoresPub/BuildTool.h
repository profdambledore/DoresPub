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

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void PressToolPrimary();
	virtual void ReleaseToolPrimary();
	virtual void ToolSecondary();

public:

	
};
