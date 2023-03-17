// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParentTool.h"
#include "Materials/Material.h"
#include "ItemDataLibrary.h"
#include "ItemTool.generated.h"

/**
 * 
 */
UCLASS()
class DORESPUB_API UItemTool : public UParentTool
{
	GENERATED_BODY()

public:
	UItemTool();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void ToolPrimary();
	virtual void ToolSecondary();
	virtual void ClearTool();

	UFUNCTION(BlueprintCallable)
		void SetFurnishingToPlace(FName ID, FFurnishingItem ItemToPlace);

	UFUNCTION(BlueprintCallable)
		void ClearFurnishingToPlace();

	UFUNCTION(BlueprintCallable)
		void FlipObject();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterial* FurnishingMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterial* ErrorMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* FurnishingMesh;

	FName CurrentID = FName("");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FFurnishingItem CurrentItemData;

	FVector	MouseWorldLocation;
	FVector MouseWorldDirection;
	FHitResult TraceHit = FHitResult(ForceInit);
	ETraceTypeQuery TraceChannel = TraceTypeQuery1;
	float PlacementDistance = 3000;

	bool bHasItem = false;
	bool bRotateMode = false;
	bool bObjectFlipped = false;
	
};
