// Fill out your copyright notice in the Description page of Project Settings.

#include "FurnishingInterface.h"
#include "EditorPlayer.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "BuildingManager.h"

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

void UItemTool::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (bHasItem) {
		// Get Mouse Location in world space
		ToolOwner->PC->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);
		// Find distance between mouse world pos and closest floor or placeable object
		bool InteractTrace = GetWorld()->LineTraceSingleByChannel(TraceHit, MouseWorldLocation, MouseWorldLocation + (MouseWorldDirection * PlacementDistance), ECC_WorldStatic, FCollisionQueryParams(FName("DistTrace"), true));
		if (bRotateMode == false) {
			FurnishingMesh->SetWorldLocation(TraceHit.Location);
		}
		else if (bRotateMode == true) {
			FRotator rotToMouse = UKismetMathLibrary::FindLookAtRotation(FurnishingMesh->GetComponentLocation(), TraceHit.Location);
			FurnishingMesh->SetWorldRotation(FRotator(0.0f, rotToMouse.Yaw, 0.0f));
		}
	}
}

void UItemTool::ToolPrimary() {
	UE_LOG(LogTemp, Warning, TEXT("Item Tool Primary"));
	ToolOwner->BM->SpawnFurnishingAtLocation(ToolOwner->CurrentFloor, CurrentID, FurnishingMesh->GetComponentLocation(), FurnishingMesh->GetComponentRotation());

}

void UItemTool::ToolSecondary() {
	// Swap between location and rotation
	bRotateMode = !bRotateMode;
}

void UItemTool::ClearTool(){
	ClearFurnishingToPlace();
}

void UItemTool::SetFurnishingToPlace(FName ID, FFurnishingItem ItemToPlace)
{
	// If there isn't a static mesh component already, create one now
		// Prev was creating a new static mesh component, swapped to doing on EditorPlayer and never using it without this tool
		// Still need pointer
		//FurnishingMesh = NewObject<UStaticMeshComponent>(ToolOwner, UStaticMeshComponent::StaticClass(), TEXT("Furnishing Placer"));

	// Check if the new furnishing is the same as the current one.  If it is, de-select it
	if (ID != CurrentID) {
		bHasItem = true;
		CurrentItemData = ItemToPlace;
		CurrentID = ID;

		FurnishingMesh->SetStaticMesh(CurrentItemData.Mesh);
	}
	else {
		ClearFurnishingToPlace();
	}
}

void UItemTool::ClearFurnishingToPlace()
{
	// Reset everything
	CurrentItemData = {};
	CurrentID = FName("");
	FurnishingMesh->SetStaticMesh(nullptr);
	bHasItem = false;
	bObjectFlipped = false;
	bRotateMode = false;
	FurnishingMesh->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f));
}

void UItemTool::FlipObject()
{
	if (bObjectFlipped == true) {
		FurnishingMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		bObjectFlipped = false;
	}
	else {
		FurnishingMesh->SetRelativeScale3D(FVector(-1.0f, 1.0f, 1.0f));
		bObjectFlipped = true;
	}
}
