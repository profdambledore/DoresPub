// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_Tool_Object.h"

#include "Kismet/KismetMathLibrary.h"

#include "Player/Player_Character.h"
#include "World/World_BuildingLevel.h"

#include "UI/UI_Player_Object.h"
#include "UI/UI_Player_Master.h"

APlayer_Tool_Object::APlayer_Tool_Object()
{
	// Object Tool Components
	SelectedObjectMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Object Tool Mesh"));
	SelectedObjectMeshComponent->SetupAttachment(Root, "");
	SelectedObjectMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SelectedObjectMeshComponent->SetCollisionProfileName(FName("BuildingTool"));
}

void APlayer_Tool_Object::BeginPlay()
{
	Super::BeginPlay();
}


void APlayer_Tool_Object::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayer_Tool_Object::PrimaryActionPressed()
{
	// Check where the player has released is inside of the WorldBounds
	FVector testClickPos = FireTraceToActor().Location;
	if (PC->GetIsPointInsideBound(testClickPos)) {
		// Check that the player can afford the item
		if (ObjectDataTable->FindRow<FObjectData>(SelectedID, "", false)->Price <= PC->GetCurrentMoney()) {
			ToggleRotationMode();
			bPlacing = true;
		}
	}
}

void APlayer_Tool_Object::PrimaryActionReleased()
{
	if (bPlacing) {
		// Take the money from the player
		PC->UpdateMoney(-ObjectDataTable->FindRow<FObjectData>(SelectedID, "", false)->Price);
		PC->GetCurrentBuildingLevel()->AddObjectToLevel(SelectedID, *ObjectDataTable->FindRow<FObjectData>(SelectedID, "", false), SelectedObjectMeshComponent->GetComponentTransform());
		ToggleRotationMode();
		bPlacing = false;
	}

}

void APlayer_Tool_Object::SecondaryActionPressed()
{
	if (bPlacing) {
		ToggleRotationMode();
		bPlacing = false;
	}
}

void APlayer_Tool_Object::SecondaryActionReleased()
{

}

void APlayer_Tool_Object::ToolTick()
{
	// Check what mode the Object tool is in
	// If they are in Normal Mode
	if (!bInRotationMode) {
		// Fire a trace to the mouse's position, updating the tools location where the trace hits if it is inside of the WorldBounds
		// Also snap it to the building snapping distance (half of a wall size)
		FVector MouseLocation = FireTraceToActor().Location;

		MouseLocation.X = GetNearestMultiple(MouseLocation.X, PC->GetCurrentGridSnapValue());
		MouseLocation.Y = GetNearestMultiple(MouseLocation.Y, PC->GetCurrentGridSnapValue());
		MouseLocation.Z = 1.0f;
		SetActorLocation(MouseLocation);

		// Update the LastPosition and call any related functions
		if (MouseLocation != LastPosition) {
			LastPosition = MouseLocation;
		}
	}
	// Else, if they are in Rotation Mode
	else {
		FRotator ObjectRotation = UKismetMathLibrary::FindLookAtRotation(SelectedObjectMeshComponent->GetComponentLocation(), FireTraceToActor().Location);

		// Add the rotation offset if any
		ObjectRotation += FRotator(0.0f, -90.0f, 0.0f);

		// Snap the rotation's yaw to the bounds, while also nullifying the roll and pitch
		ObjectRotation.Yaw = GetNearestMultiple(ObjectRotation.Yaw, PC->GetCurrentRotationSnapValue());
		ObjectRotation.Pitch = 0.0f;
		ObjectRotation.Roll = 0.0f;

		// Then rotate the mesh
		SelectedObjectMeshComponent->SetWorldRotation(ObjectRotation);
	}
}

void APlayer_Tool_Object::OnEquip()
{
}

void APlayer_Tool_Object::OnUnequip()
{
	SelectedMesh = nullptr;
	SelectedID = "";
	SelectedObjectMeshComponent->SetStaticMesh(nullptr);
}

void APlayer_Tool_Object::SetupTool(APlayer_Character* NewPC)
{
	Super::SetupTool(NewPC);

	PC = NewPC;

	ObjectWidget = PC->GetUI()->ObjectState;
	ObjectWidget->ObjectTool = this;

	ObjectWidget->UpdateObjectTileView();
}

void APlayer_Tool_Object::UpdateToolRotation()
{
}

void APlayer_Tool_Object::UpdateObjectMesh(FName NewID)
{
	SelectedID = NewID;

	if (NewID == "") {
		SelectedMesh = nullptr;
		
		SelectedObjectMeshComponent->SetStaticMesh(nullptr);
	}
	else {
		FObjectData NewObject = *ObjectDataTable->FindRow<FObjectData>(NewID, "");
		SelectedMesh = NewObject.Mesh;
		SelectedObjectMeshComponent->SetStaticMesh(NewObject.Mesh);
	}
}

void APlayer_Tool_Object::ToggleRotationMode()
{
	bInRotationMode = !bInRotationMode;
}
