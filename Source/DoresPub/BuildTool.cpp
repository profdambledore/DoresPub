// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawDebugHelpers.h"
#include "EditorPlayer.h"
#include "BuildTool.h"

UBuildTool::UBuildTool() {
	// Update Inherited Properties
	ToolType = EToolType::Build;
}

void UBuildTool::ToolPrimary() {
	UE_LOG(LogTemp, Warning, TEXT("Build Tool Primary"));

}

void UBuildTool::ToolSecondary() {

}