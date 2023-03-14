// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ToolLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DORESPUB_API UToolLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};

UENUM()
enum EToolType
{
	Explore UMETA(Display Name = "Explore"),
	Select UMETA(Display Name = "Select"),
	Build UMETA(Display Name = "Build"),
	Item UMETA(Display Name = "Item"),
};
