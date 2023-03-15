// Fill out your copyright notice in the Description page of Project Settings.


#include "ParentFurnishing.h"

// Sets default values
AParentFurnishing::AParentFurnishing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the static mesh component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Furnishing Mesh"));

}

// Called when the game starts or when spawned
void AParentFurnishing::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AParentFurnishing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FString AParentFurnishing::GetObjectData()
{
	return Data.Name;
}

