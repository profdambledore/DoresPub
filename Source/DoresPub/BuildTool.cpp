// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawDebugHelpers.h"
#include "EditorPlayer.h"
#include "BuildTool.h"

UBuildTool::UBuildTool() {
	// Update Inherited Properties
	ToolType = EToolType::Build;
}

void UBuildTool::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
}

void UBuildTool::PressToolPrimary() {
	UE_LOG(LogTemp, Warning, TEXT("Build Tool Primary"));

	// First, check what mode we in (wall, floor)
	// Then, get the closest grid to the mouse position
	// Build a column there
	// Start allowing ticks, update mouse position

}

void UBuildTool::ReleaseToolPrimary()
{
	// On release, check if the wall is longer than 0
	// If it is, pass through the array of walls to the BM and build the wall

}

void UBuildTool::ToolSecondary() {

}