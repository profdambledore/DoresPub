// Fill out your copyright notice in the Description page of Project Settings.

#include "FurnishingInterface.h"
#include "DrawDebugHelpers.h"
#include "EditorPlayer.h"
#include "SelectTool.h"

USelectTool::USelectTool() {
	// Update Inherited Properties
	ToolType = EToolType::Select;
}

void USelectTool::ToolPrimary() {
	UE_LOG(LogTemp, Warning, TEXT("Select Tool Primary"));

	// Get object under mouse
	ToolOwner->PC->GetHitResultUnderCursorByChannel(TraceChannel, false, TraceHit);

	// IDamageableInterface* inf = Cast<IDamageableInterface>(hitResult.Actor)
	IFurnishingInterface* inf = Cast<IFurnishingInterface>(TraceHit.Actor);
	if (inf) {
		FString outName = inf->GetObjectData();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *outName);
	}


	// Find distance between mouse world pos and closest floor or placeable object (may not be needed)
	//bool PlacementTrace = GetWorld()->LineTraceSingleByChannel(TraceHit, MouseWorldLocation, MouseWorldLocation + (MouseWorldDirection * PlacementDistance), TraceChannel, FCollisionQueryParams(FName("DistTrace"), true));
	//DrawDebugLine(GetWorld(), MouseWorldLocation, MouseWorldLocation + (MouseWorldDirection * 2000), FColor::Green, false, 1.f, ECC_WorldStatic, 0.5f);

}

void USelectTool::ToolSecondary() {

}