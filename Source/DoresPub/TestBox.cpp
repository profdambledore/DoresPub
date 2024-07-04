// Fill out your copyright notice in the Description page of Project Settings.


#include "TestBox.h"

#include "DrawDebugHelpers.h"

// Sets default values
ATestBox::ATestBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestBox::BeginPlay()
{
	Super::BeginPlay();
	
	// Create a FBox2D
	FBox2D testbox = FBox2D(FVector2D(0.0F, 0.0f), FVector2D(1000.0F, 1000.0F));

	// Draw the box
	DrawDebugBox(GetWorld(), FVector(testbox.GetCenter(), 0.0f), FVector(testbox.GetExtent(), 0.0f), FQuat(), FColor::Blue, true, -1.0f, 0U, 3.0f);

	a = 493.0 / 5;
	UE_LOG(LogTemp, Warning, TEXT("493 rounded to the nearest 5 = %f"), round(a));

}

// Called every frame
void ATestBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

