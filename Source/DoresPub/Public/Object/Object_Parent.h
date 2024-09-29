// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Dataflow/DataflowEngineTypes.h"

#include "Object_Parent.generated.h"

UCLASS()
class DORESPUB_API AObject_Parent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObject_Parent();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// -- Object Functions --
	// Called to update the mesh and ID of the object
	void SetupObject(FName NewID, UStaticMesh* NewMesh, TArray<FStringValuePair> NewTags);

	/// -- Utility Functions --
	// Called to get the ID of this object
	FName GetID() { return ObjectID; }

	// Called to get the objects tags
	TArray<FStringValuePair> GetObjectTags() { return ObjectTags; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

protected:
	/// -- Object Components --
	// SceneComponent acting as the tool's root
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parent Components")
	USceneComponent* Root = nullptr;

	// StaticMeshComponent of the object's mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Object Components")
	UStaticMeshComponent* ObjectMesh;

	/// -- Object Properties --
	// FName denoting the object's ID
	FName ObjectID = "";

	// TArray of FStrings of the objects Tags
	TArray<FStringValuePair> ObjectTags;
};
