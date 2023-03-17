// Fill out your copyright notice in the Description page of Project Settings.

#include "FurnishingInterface.h"
#include "EditorPlayer.h"
#include "ItemTool.h"

UItemTool::UItemTool() {
	// Update Inherited Properties
	ToolType = EToolType::Item;

	static ConstructorHelpers::FObjectFinder<UMaterial>ErrorObject(TEXT("/Game/Tools/M_ItemToolError.M_ItemToolError"));
	if (ErrorObject.Succeeded())
	{
		ErrorMaterial = ErrorObject.Object;
	}
}

void UItemTool::ToolPrimary() {
	UE_LOG(LogTemp, Warning, TEXT("Item Tool Primary"));

}

void UItemTool::ToolSecondary() {

}
