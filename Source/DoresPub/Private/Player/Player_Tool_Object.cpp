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
	SelectedObjectMeshComponent->SetCollisionProfileName(FName("ObjectTool"));
	SelectedObjectMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void APlayer_Tool_Object::BeginPlay()
{
	Super::BeginPlay();

	// Add the overlap functions for the SelectedObjectMeshComponent
	SelectedObjectMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &APlayer_Tool_Object::OnSOMCBeginOverlap);
	SelectedObjectMeshComponent->OnComponentEndOverlap.AddDynamic(this, &APlayer_Tool_Object::OnSOMCEndOverlap);
}

void APlayer_Tool_Object::OnSOMCBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// If the overlapped actor is valid, then...
	if (OtherActor) {
		// Add the actor to the OverlappedActors array
		OverlappedActors.Add(OtherActor);

		// If the array is now greater than 0 (overlapping at least one object), then change the material of the SelectedObject to the InvalidMaterial
		if (OverlappedActors.Num() > MaxOverlaps) {
			bValidOverlap = false;
			if (!GetPlacementIsValid()) {
				for (int i = 0; i < SelectedObjectMaterials.Num(); i++) {
					SelectedObjectMeshComponent->SetMaterial(i, InvalidMaterial);
				}
			}
		}
	}
}

void APlayer_Tool_Object::OnSOMCEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Remove the actor from the OverlappedActors array
	OverlappedActors.Remove(OtherActor);

	// If the array is now empty, then change the material of the SelectedObject back to it's original material
	if (OverlappedActors.Num() <= MaxOverlaps) {
		bValidOverlap = true;
		if (GetPlacementIsValid()) {
			for (int i = 0; i < SelectedObjectMaterials.Num(); i++) {
				SelectedObjectMeshComponent->SetMaterial(i, SelectedObjectMaterials[i]);
			}
		}
	}
}

void APlayer_Tool_Object::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayer_Tool_Object::PrimaryActionPressed()
{
	// If the player doesn't have either an SelectedObject or a SelectedID, there is no possible way to be in RotationMode, so set it to be false
	if (!SelectedObjectMeshComponent->GetStaticMesh()) {
		bInRotationMode = false;
	}

	// Check if the player actually has an selected id.  If so, then
	if (SelectedID != "") {
		// Check where the player has released is inside of the WorldBounds
		FVector testClickPos = FireTraceToActor().Location;
		if (PC->GetIsPointInsideBound(testClickPos)) {
			// Check that the player can afford the item
			if (GetObjectDataRow(SelectedID)->Price <= PC->GetCurrentMoney()) {
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
					SelectedObject = Cast<AObject_Parent>(FindObjectTrace.GetActor());
					ObjectPlacementType = "";
					ObjectSnapType = "";

					// Hide the object
					SelectedObject->SetActorHiddenInGame(true);
					SelectedObject->SetActorEnableCollision(false);

					// Set the SOMC to the SelectedObject's mesh and match the rotation
					SelectedObjectMeshComponent->SetStaticMesh(SelectedObject->GetObjectMesh());
					SelectedObjectMeshComponent->SetWorldRotation(SelectedObject->GetActorRotation());

					// Store the materials of the object
					for (int i = 0; i < SelectedObject->GetObjectMesh()->GetStaticMaterials().Num(); i++) {
						SelectedObjectMaterials.Add(SelectedObject->GetObjectMesh()->GetMaterial(i));
						SelectedObjectMeshComponent->SetMaterial(i, SelectedObjectMaterials[i]);
					}

					// Also check if the object 'picked up' can be placed on other objects
					for (FStringValuePair i : GetObjectDataRow(SelectedObject->GetID())->ObjectTags) {
						if (i.Key == "placementType") {
							ObjectPlacementType = i.Value;
						}
						else if (i.Key == "snapType") {
							ObjectSnapType = i.Value;
							SnappingTransforms = GetObjectDataRow(SelectedObject->GetID())->SnapLocations;
						}
					}
				}
			}
		}
	}
}

void APlayer_Tool_Object::PrimaryActionReleased()
{
	// If the object can be placed at the location
	if (GetPlacementIsValid()) {
		// Check if the player is currently in placing mode
		if (bPlacing) {
			// Take the money from the player, then add the object to the world via the current BuildingLevel
			PC->UpdateMoney(-GetObjectDataRow(SelectedID)->Price);
			AObject_Parent* Object = PC->GetCurrentBuildingLevel()->AddObjectToLevel(SelectedID, *GetObjectDataRow(SelectedID), SelectedObjectMeshComponent->GetComponentTransform());

			// If the object was placed on another object, attach it to that other object
			if (ObjectPlacementType == "Object") {
				AActor* HitActor = FireTraceToActor(Object).GetActor();
				if (HitActor) {
					Object->AttachToActor(HitActor, FAttachmentTransformRules::KeepWorldTransform);
				}
				// If the trace didn't hit anything, then unattach it
				else {
					Object->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				}
			}

			// Then exit rotation mode
			ToggleRotationMode();
			bPlacing = false;
		}
		// Check if the player is in rotation mode while having a selected object
		else if (bInRotationMode && SelectedObject) {
			// If they do, stop moving/rotating the object and clear the SelectedObjectPrevious
			SelectedObject->SetActorHiddenInGame(false);
			SelectedObject->SetActorEnableCollision(true);

			for (int i = 0; i < SelectedObjectMaterials.Num(); i++) {
				SelectedObjectMeshComponent->SetMaterial(i, SelectedObjectMaterials[i]);
			}

			// If the object was placed on another object, attach it to that other object
			if (ObjectPlacementType == "object") {
				AActor* HitActor = FireTraceToActor(SelectedObject).GetActor();
				if (HitActor) {
					SelectedObject->AttachToActor(HitActor, FAttachmentTransformRules::KeepWorldTransform);
				}
				else {
					SelectedObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				}
			}

			SelectedObject = nullptr;
			SelectedObjectPrevious = FTransform();
			SelectedObjectMaterials.Empty();

			ToggleRotationMode();
			SelectedObjectMeshComponent->SetStaticMesh(nullptr);
		}
	}
	// If the item cannot be placed at this location, stop rotating and placing
	else {
		bInRotationMode = false;
		bPlacing = false;
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

		for (int i = 0; i < SelectedObjectMaterials.Num(); i++) {
			SelectedObjectMeshComponent->SetMaterial(i, SelectedObjectMaterials[i]);
		}

		// Unhide the object
		SelectedObject->SetActorHiddenInGame(false);
		SelectedObject->SetActorEnableCollision(true);
		SelectedObjectMeshComponent->SetStaticMesh(nullptr);
		SelectedObject = nullptr;
		SelectedObjectMaterials.Empty();
	}
	// Finally, check if the player has a selected ID but isnt currently placing it.  If so, then clear the current selected ID
	if (SelectedID != "" && !bPlacing) {
		for (int i = 0; i < SelectedObjectMaterials.Num(); i++) {
			SelectedObjectMeshComponent->SetMaterial(i, SelectedObjectMaterials[i]);
		}

		SelectedMesh = nullptr;
		SelectedID = "";
		SelectedObjectMeshComponent->SetStaticMesh(nullptr);
		SelectedObjectMaterials.Empty();
	}
}

void APlayer_Tool_Object::SecondaryActionReleased()
{

}

void APlayer_Tool_Object::DeleteAction()
{
	// If the player has picked up a placed item, get it's ID
	if (SelectedObject) {
		FName IDToRefund = Cast<AObject_Parent>(SelectedObject)->GetID();

		// Find the data table row corrisponding with the ID and refund the player that amount * the refund multiplier
		int RefundAmount = GetObjectDataRow(IDToRefund)->Price * RefundMultiplier;
		PC->UpdateMoney(RefundAmount);
		SelectedObject->Destroy();

		SelectedObjectMeshComponent->SetStaticMesh(nullptr);
		SelectedObjectMaterials.Empty();
		SelectedObject = nullptr;
	}
}

void APlayer_Tool_Object::ToolTick()
{
	// If the player has either a SelectedID or a SelectedObject, continue
	if (SelectedID != "" || SelectedObject) {
		// If they are in Normal Mode
		if (!bInRotationMode) {
			// Fire a trace to the mouse's position, updating the tools location where the trace hits if it is inside of the WorldBounds
			// Also snap it to the building snapping distance (half of a wall size)
			FHitResult TraceFired = FireTraceToActor(SelectedObject);
			FVector TargetLocation = TraceFired.Location;
			FVector TargetAdditions = FVector(0, 0, 0);

			TargetLocation.X = GetNearestMultiple(TargetLocation.X, PC->GetCurrentGridSnapValue());
			TargetLocation.Y = GetNearestMultiple(TargetLocation.Y, PC->GetCurrentGridSnapValue());

			// First, check if the object can only be placed on walls
			if (ObjectPlacementType == "wall" || ObjectPlacementType == "fill") {
				// If the trace hit a placed wall, get it's index from the BuildingLevel
				if (TraceFired.GetComponent()->GetOwner()->IsA<AWorld_BuildingLevel>()) {
					LastWallHitByTrace = PC->GetCurrentBuildingLevel()->GetBuildDataAtLocation(TraceFired.GetComponent()->GetComponentLocation());
				}
				else {
					LastWallHitByTrace = -1;
					bValidPlacement = false;
					bSnapping = false;
					CurrentSnap = "";
				}

				if (LastWallHitByTrace != -1) {	
					bool bSnapFound = false;
					if (ObjectPlacementType == "fill") {
						// Also get a pointer to the cell of the wall
						FName HitWallID = (TraceFired.GetComponent()->GetComponentRotation() == FRotator(0.0f, 0.0f, 0.0f)) ? PC->GetCurrentBuildingLevel()->GetBuildDataFromIndex(LastWallHitByTrace)->XWallData.ID : PC->GetCurrentBuildingLevel()->GetBuildDataFromIndex(LastWallHitByTrace)->YWallData.ID;

						// Next, check if the wall has a object snap type.  If it does, and it matches the other object's snap type then move it to that object.
						if (WallDataTable->FindRow<FSelectableWallData>(HitWallID, "", false)->Tags.Contains(FStringValuePair("snapType", ObjectSnapType))) { //ObjectDataTable->FindRow<FObjectData>(ID, "", false)
							// Check each snap point, converting to world space, and see if it is within the snap distance
							for (FObjectSnappingData i : WallDataTable->FindRow<FSelectableWallData>(HitWallID, "", false)->SnapLocations) {
								if (FVector::Dist2D(TraceFired.Location, TraceFired.GetComponent()->GetComponentLocation() + (TraceFired.GetComponent()->GetForwardVector() * (i.Transform.GetLocation().X + i.HoverOffset.X) + (TraceFired.GetComponent()->GetRightVector() * (i.Transform.GetLocation().Y + i.HoverOffset.Y)))) <= SnapDistance) {
									// Check if the current object contains a snap matching this point
									for (FObjectSnappingData j : SnappingTransforms) {
										if (j.SnapWith == i.SnapType) {
											// Finally, check if the math has already been calculated
											if (CurrentSnap != i.SnapType) {
												// If one does match, update the location and rotation
												// Start with rotation, 
												SelectedObjectMeshComponent->SetRelativeRotation(TraceFired.GetComponent()->GetComponentRotation());
												SelectedObjectMeshComponent->AddRelativeRotation(i.Transform.GetRotation().Rotator());

												FVector VecA = UKismetMathLibrary::GetForwardVector(TraceFired.GetComponent()->GetComponentRotation() + i.Transform.GetRotation().Rotator());
												VecA.Normalize();

												FVector VecB = UKismetMathLibrary::GetForwardVector(SelectedObjectMeshComponent->GetComponentRotation() + j.Transform.GetRotation().Rotator());
												VecB.Normalize();

												float OutAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(VecA, VecB)));

												if (FVector::CrossProduct(VecA, VecB).Z > 0)
												{
													OutAngle = -OutAngle;
												}

												//SelectedObjectMeshComponent->AddLocalRotation(FRotator(0.0f, 180.0f, 0.0f));
												SelectedObjectMeshComponent->AddLocalRotation(FRotator(0.0f, OutAngle, 0.0f));

												TargetLocation = TraceFired.GetComponent()->GetComponentLocation();

												FVector iVec = (UKismetMathLibrary::GetForwardVector(TraceFired.GetComponent()->GetComponentRotation()) * i.Transform.GetLocation().X) + (UKismetMathLibrary::GetRightVector(TraceFired.GetComponent()->GetComponentRotation()) * i.Transform.GetLocation().Y);
												FVector jVec = (UKismetMathLibrary::GetForwardVector(SelectedObjectMeshComponent->GetComponentRotation()) * j.Transform.GetLocation().X) + (UKismetMathLibrary::GetRightVector(SelectedObjectMeshComponent->GetComponentRotation()) * j.Transform.GetLocation().Y);

												// Move the tool to the location
												SetActorLocation(TargetLocation);

												UE_LOG(LogTemp, Warning, TEXT("iVec = %s"), *iVec.ToString());
												AddActorLocalOffset(iVec);
												//AddActorLocalOffset(jVec * -1);

												CurrentSnap = i.SnapType;
												MaxOverlaps = 1;
											}
											bSnapFound = true;
											bValidPlacement = true;
										}
									}
								}
							}
						}
					}
					else{
						bValidPlacement = true;
					}
					bSnapping = bSnapFound;
				}
			}


			// Next, check if the object can be placed on the floor or other objects.
			else if (ObjectPlacementType == "floor" || ObjectPlacementType == "object") {
				bValidPlacement = true;

				if (ObjectPlacementType == "floor") {
					TargetLocation.Z = 1.0f;
				}

				// If the trace hit a placed object, cast to it and store it if it hasn't already been stored
				if (TraceFired.GetActor()) {
					if (!LastObjectHitByTrace) {
						// If it is an object, then
						if (TraceFired.GetActor()->IsA<AObject_Parent>()) {
							LastObjectHitByTrace = Cast<AObject_Parent>(TraceFired.GetActor());
						}
					}
					else if (TraceFired.GetActor()->GetName() != LastObjectHitByTrace->GetName()) {
						if (TraceFired.GetActor()->IsA<AObject_Parent>()) {
							LastObjectHitByTrace = Cast<AObject_Parent>(TraceFired.GetActor());
						}
						else {
							LastObjectHitByTrace = nullptr;
						}
					}
				}
				else {
					LastObjectHitByTrace = nullptr;
				}

				bool bSnapFound = false;
				if (LastObjectHitByTrace) {
					// Next, check if it has a object snap type.  If it does, and it matches the other object's snap type then move it to that object.
					if (GetObjectDataRow(LastObjectHitByTrace->GetID())->ObjectTags.Contains(FStringValuePair("snapType", ObjectSnapType))) {
						// Check each snap point, converting to world space, and see if it is within the snap distance
						for (FObjectSnappingData i : GetObjectDataRow(LastObjectHitByTrace->GetID())->SnapLocations) {
							if (FVector::Dist2D(TraceFired.Location, LastObjectHitByTrace->GetActorLocation() + (LastObjectHitByTrace->GetActorForwardVector() * (i.Transform.GetLocation().X + i.HoverOffset.X) + (LastObjectHitByTrace->GetActorRightVector() * (i.Transform.GetLocation().Y + i.HoverOffset.Y)))) <= SnapDistance) {
								// Check if the current object contains a snap matching this point
								for (FObjectSnappingData j : SnappingTransforms) {
									if (j.SnapWith == i.SnapType) {
										// Finally, check if the math has already been calculated
										if (CurrentSnap != i.SnapType) {
											// If one does match, update the location and rotation
											// Start with rotation, 
											SelectedObjectMeshComponent->SetRelativeRotation(LastObjectHitByTrace->GetActorRotation());
											SelectedObjectMeshComponent->AddRelativeRotation(i.Transform.GetRotation().Rotator());

											FVector VecA = UKismetMathLibrary::GetForwardVector(LastObjectHitByTrace->GetActorRotation() + i.Transform.GetRotation().Rotator());
											VecA.Normalize();

											FVector VecB = UKismetMathLibrary::GetForwardVector(SelectedObjectMeshComponent->GetComponentRotation() + j.Transform.GetRotation().Rotator());
											VecB.Normalize();

											float OutAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(VecA, VecB)));

											if (FVector::CrossProduct(VecA, VecB).Z > 0)
											{
												OutAngle = -OutAngle;
											}

											SelectedObjectMeshComponent->AddLocalRotation(FRotator(0.0f, 180.0f, 0.0f));
											SelectedObjectMeshComponent->AddLocalRotation(FRotator(0.0f, OutAngle, 0.0f));

											TargetLocation = LastObjectHitByTrace->GetActorLocation();

											FVector iVec = (UKismetMathLibrary::GetForwardVector(LastObjectHitByTrace->GetActorRotation()) * i.Transform.GetLocation().X) + (UKismetMathLibrary::GetRightVector(LastObjectHitByTrace->GetActorRotation()) * i.Transform.GetLocation().Y);
											FVector jVec = (UKismetMathLibrary::GetForwardVector(SelectedObjectMeshComponent->GetComponentRotation()) * j.Transform.GetLocation().X) + (UKismetMathLibrary::GetRightVector(SelectedObjectMeshComponent->GetComponentRotation()) * j.Transform.GetLocation().Y);

											// Move the tool to the location
											SetActorLocation(TargetLocation);
											AddActorLocalOffset(iVec); //(i.Transform.GetLocation() * (UKismetMathLibrary::GetForwardVector(LastObjectHitByTrace->GetActorRotation()) + UKismetMathLibrary::GetRightVector(LastObjectHitByTrace->GetActorRotation())))
											AddActorLocalOffset(jVec * -1);
											
											CurrentSnap = i.SnapType;
											MaxOverlaps = 1;
										}
										bSnapFound = true;
									}
								}
							}
						}
						
					}
				}
				bSnapping = bSnapFound;
			}

			// If a snap wasn't found, clear the CurrentSnap
			if (!bSnapping) {
				CurrentSnap = "";
				MaxOverlaps = 0;
				SetActorLocation(TargetLocation);
			}

			// Update the materials TODO - Convert to function
			if (!GetPlacementIsValid()) {
				for (int i = 0; i < SelectedObjectMaterials.Num(); i++) {
					SelectedObjectMeshComponent->SetMaterial(i, InvalidMaterial);
				}
			}
			else {
				for (int i = 0; i < SelectedObjectMaterials.Num(); i++) {
					SelectedObjectMeshComponent->SetMaterial(i, SelectedObjectMaterials[i]);
				}
			}
			
			// Check if the tool has a selected object.  If so, then move it as well
			if (SelectedObject) {
				SelectedObject->SetActorLocation(SelectedObjectMeshComponent->GetComponentLocation());
				SelectedObject->SetActorRotation(SelectedObjectMeshComponent->GetComponentRotation());
			}

			// Update the LastPosition and call any related functions
			if (TargetLocation != LastPosition) {
				LastPosition = TargetLocation;
			}
		}
		// Else, if they are in Rotation Mode
		else {
			if (!bSnapping) {
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
					SelectedObjectMeshComponent->SetWorldRotation(ObjectRotation);
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
	}
}

void APlayer_Tool_Object::OnEquip()
{
}

void APlayer_Tool_Object::OnUnequip()
{
	// Clear any selected objects
	for (int i = 0; i < SelectedObjectMaterials.Num(); i++) {
		SelectedObjectMeshComponent->SetMaterial(i, SelectedObjectMaterials[i]);
	}

	SelectedMesh = nullptr;
	SelectedID = "";
	SelectedObjectMeshComponent->SetStaticMesh(nullptr);
	SelectedObjectMaterials.Empty();

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
	// If the player has picked up an item, place it back where it was
	if (SelectedObject) {
		SelectedObject->SetActorTransform(SelectedObjectPrevious);

		for (int i = 0; i < SelectedObjectMaterials.Num(); i++) {
			SelectedObjectMeshComponent->SetMaterial(i, SelectedObjectMaterials[i]);
		}

		// Unhide the object
		SelectedObject->SetActorHiddenInGame(false);
		SelectedObject->SetActorEnableCollision(true);
		SelectedObjectMeshComponent->SetStaticMesh(nullptr);
		SelectedObject = nullptr;
		SelectedObjectMaterials.Empty();
	}

	// If the NewID matches the current ID OR it is blank, clear the selected item
	if (NewID == SelectedID || NewID == "") {
		for (int i = 0; i < SelectedObjectMaterials.Num(); i++) {
			SelectedObjectMeshComponent->SetMaterial(i, SelectedObjectMaterials[i]);
		}

		SelectedMesh = nullptr;
		SelectedID = "";
		SelectedObjectMeshComponent->SetStaticMesh(nullptr);
		SelectedObjectMaterials.Empty();
	}
	else {
		// Find the item in the Data table, setting up the mesh and ID to the data found
		FObjectData NewObject = *GetObjectDataRow(NewID);
		SelectedMesh = NewObject.Mesh;
		SelectedID = NewID;
		SelectedObjectMeshComponent->SetStaticMesh(NewObject.Mesh);
		ObjectPlacementType = "";
		ObjectSnapType = "";
		SelectedObjectMaterials.Empty();

		// Store the material of the object
		for (int i = 0; i < NewObject.Mesh->GetStaticMaterials().Num(); i++) {
			SelectedObjectMaterials.Add(SelectedMesh->GetMaterial(i));
			SelectedObjectMeshComponent->SetMaterial(i, SelectedObjectMaterials[i]);
		}

		// Also query important tags
		for (FStringValuePair i : NewObject.ObjectTags) {
			// Check if the item has the tag placeableOnObjects
			if (i.Key == "placementType") {
				ObjectPlacementType = i.Value;
			}
			else if (i.Key == "snapType") {
				ObjectSnapType = i.Value;
				SnappingTransforms = NewObject.SnapLocations;
			}
		}
	}
}

void APlayer_Tool_Object::ToggleRotationMode()
{
	// Flip bInRotationMode
	bInRotationMode = !bInRotationMode;
}