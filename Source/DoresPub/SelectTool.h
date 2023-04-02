// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParentTool.h"
#include "SelectTool.generated.h"

class AEditorPlayer;

UCLASS()
class DORESPUB_API USelectTool : public UParentTool
{
	GENERATED_BODY()
	
public:
	USelectTool();

	virtual void PressToolPrimary();
	virtual void ToolSecondary();

protected:
	FVector	MouseWorldLocation;
	FVector MouseWorldDirection;
	FHitResult TraceHit = FHitResult(ForceInit);
	ETraceTypeQuery TraceChannel = TraceTypeQuery1;

};
