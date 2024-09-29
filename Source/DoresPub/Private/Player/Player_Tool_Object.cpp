// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Player_Tool_Object.h"

#include "Kismet/KismetMathLibrary.h"

#include "Player/Player_Character.h"
#include "World/World_BuildingLevel.h"
#include "Object/Object_Parent.h"

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
	// Check if the player actually has an selected id.  If so, then
	if (SelectedID != "") {
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
	// If they don't have a selected ID, check if they have a selected object. 
	else {
		FHitResult FindObjectTrace = FireTraceToActor();
		// If they have a selected object, check if they clicked inside the bounds
		if (SelectedObject) {
			// If they did, then act like they are placing the object and swap to rotation mode
			if (PC->GetIsPointInsideBound(FindObjectTrace.Location)) {
				ToggleRotationMode();
			}
		}
		// If they have neither a SelectedID or SelectedObject, then check if the trace hit an object
		else {
			if (PC->GetIsPointInsideBound(FindObjectTrace.Location)) {
				// Check if the trace has hit a AObject_Parent
				if (FindObjectTrace.GetActor()->IsA<AObject_Parent>()) {
					// If so, then record it's last position and 'pick up' the object
					SelectedObjectPrevious = FindObjectTrace.GetActor()->GetTransform();
					SelectedObject = FindObjectTrace.GetActor();
					bPlacedOnObjects = false;

					// Also check if the object 'picked up' can be placed on other objects
					for (FStringValuePair i : Cast<AObject_Parent>(FindObjectTrace.GetActor())->GetObjectTags()) {
						if (i.Key == "placeableOnObjects") {
							if (i.Value == "true") {
								bPlacedOnObjects = true;
							}
						}
					}
				}
			}
		}
	}
}

void APlayer_Tool_Object::PrimaryActionReleased()
{
	if (bPlacing) {
		// Take the money from the player, then add the object to the world via the current BuildingLevel
		PC->UpdateMoney(-ObjectDataTable->FindRow<FObjectData>(SelectedID, "", false)->Price);
		PC->GetCurrentBuildingLevel()->AddObjectToLevel(SelectedID, *ObjectDataTable->FindRow<FObjectData>(SelectedID, "", false), SelectedObjectMeshComponent->GetComponentTransform());

		// Then exit rotation mode
		ToggleRotationMode();
		bPlacing = false;
	}
	// Check if the player is in rotation mode while having a selected object
	else if (bInRotationMode && SelectedObject) {
		// If they do, stop moving/rotating the object and clear the SelectedObjectPrevious
		SelectedObject = nullptr;
		SelectedObjectPrevious = FTransform();
		ToggleRotationMode();
	}

}

void APlayer_Tool_Object::SecondaryActionPressed()
{
	// Check if the player is currently placing a new object.  If they are, stop
	if (bPlacing) {
		ToggleRotationMode();
		bPlacing = false;
	}
	// Next, check if the player is moving a placed object.  If they are, move it back to it's previous transform
	if (SelectedObject) {
		SelectedObject->SetActorTransform(SelectedObjectPrevious);
		SelectedObject = nullptr;
	}
	// Finally, check if the player has a selected ID but isnt currently placing it.  If so, then clear the current selected ID
	if (SelectedID != "" && !bPlacing) {
		SelectedMesh = nullptr;
		SelectedID = "";
		SelectedObjectMeshComponent->SetStaticMesh(nullptr);
	}
}

void APlayer_Tool_Object::SecondaryActionReleased()
{

}

void APlayer_Tool_Object::ToolTick()
{
	// If they are in Normal Mode
	if (!bInRotationMode) {
		// Fire a trace to the mouse's position, updating the tools location where the trace hits if it is inside of the WorldBounds
		// Also snap it to the building snapping distance (half of a wall size)
		FVector MouseLocation = FireTraceToActor(SelectedObject).Location;

		MouseLocation.X = GetNearestMultiple(MouseLocation.X, PC->GetCurrentGridSnapValue());
		MouseLocation.Y = GetNearestMultiple(MouseLocation.Y, PC->GetCurrentGridSnapValue());

		// Check if the object can be placed on other object.  If it cant, set it to the BuildingLevel's Z position.  TODO - Currently, only one building level is used so it defaults to 1
		if (!bPlacedOnObjects) {
			MouseLocation.Z = 1.0f;
		}
		
		// Move the tool to the location
		SetActorLocation(MouseLocation);

		// Check if the tool has a selected object.  If so, then move it as well
		if (SelectedObject) {
			SelectedObject->SetActorLocation(MouseLocation);
		}

		// Update the LastPosition and call any related functions
		if (MouseLocation != LastPosition) {
			LastPosition = MouseLocation;
		}
	}
	// Else, if they are in Rotation Mode
	else {
		if (SelectedObject) {
			FRotator ObjectRotation = UKismetMathLibrary::FindLookAtRotation(SelectedObject->GetActorLocation(), FireTraceToActor().Location);

			// Add the rotation offset if any
			ObjectRotation += FRotator(0.0f, -90.0f, 0.0f);

			// Snap the rotation's yaw to the bounds, while also nullifying the roll and pitch
			ObjectRotation.Yaw = GetNearestMultiple(ObjectRotation.Yaw, PC->GetCurrentRotationSnapValue());
			ObjectRotation.Pitch = 0.0f;
			ObjectRotation.Roll = 0.0f;

			// Then rotate the mesh
			SelectedObject->SetActorRotation(ObjectRotation);
		}
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
}

void APlayer_Tool_Object::OnEquip()
{
}

void APlayer_Tool_Object::OnUnequip()
{
	// Clear any selected objects
	SelectedMesh = nullptr;
	SelectedID = "";
	SelectedObjectMeshComponent->SetStaticMesh(nullptr);

	// If they are moving an object, place it back in its previous transform
	if (SelectedObject) {
		SelectedObject->SetActorTransform(SelectedObjectPrevious);
		SelectedObject = nullptr;
	}
}

void APlayer_Tool_Object::SetupTool(APlayer_Character* NewPC)
{
	Super::SetupTool(NewPC);

	// Connect pointers
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
	// If the NewID matches the current ID OR it is blank, clear the selected item
	if (NewID == SelectedID || NewID == "") {
		SelectedMesh = nullptr;
		SelectedID = "";
		SelectedObjectMeshComponent->SetStaticMesh(nullptr);

	}
	else {
		// Find the item in the Data table, setting up the mesh and ID to the data found
		FObjectData NewObject = *ObjectDataTable->FindRow<FObjectData>(NewID, "");
		SelectedMesh = NewObject.Mesh;
		SelectedID = NewID;
		SelectedObjectMeshComponent->SetStaticMesh(NewObject.Mesh);
		bPlacedOnObjects = false;

		// Also check if the item has the tag placeableOnObjects
		for (FStringValuePair i : NewObject.ObjectTags) {
			if (i.Key == "placeableOnObjects") {
				if (i.Value == "true") {
					bPlacedOnObjects = true;
				}
			}
		}
	}
}

void APlayer_Tool_Object::ToggleRotationMode()
{
	// Flip bInRotationMode
	bInRotationMode = !bInRotationMode;
}
