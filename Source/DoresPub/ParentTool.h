// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ToolLibrary.h"
#include "ToolInterface.h"

#include "ParentTool.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DORESPUB_API UParentTool : public UActorComponent, public IToolInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UParentTool();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		virtual void PressToolPrimary();

	UFUNCTION(BlueprintCallable)
		virtual void ReleaseToolPrimary();

	UFUNCTION(BlueprintCallable)
		virtual void ToolSecondary();

	UFUNCTION(BlueprintCallable)
		virtual void ClearTool();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// The tools type
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TEnumAsByte<EToolType> ToolType;

	class AEditorPlayer* ToolOwner;

protected:
	//
	
	
};
