// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Button.h"
#include "Components/TileView.h"

#include "Data/BuildToolData.h"

#include "UI_Player_Build.generated.h"

UCLASS()
class DORESPUB_API UUI_Player_Build : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void SynchronizeProperties() override;

	// Called to add a new selectable wall to the list view
	void AddSelectableWallToList(FName ID, FSelectableWallData WallData);

	// Called to update the selected wall in the BuildToolDisplay
	UFUNCTION(BlueprintCallable)
	void UpdateSelectedWall(FName ObjectID);

protected:
	/// -- Button Functions --
	// Button event to swap to erase mode
	UFUNCTION()
	void OnWallButtonReleased();

	// Button event to swap to erase mode
	UFUNCTION()
	void OnEraseButtonReleased();

	// Button event to swap to erase mode
	UFUNCTION()
	void OnFloorButtonReleased();

public:
	/// -- Components --
	// Button to swap to wall mode
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Grid Snapping")
	UButton* WallModeButton = nullptr;

	// Button to swap to erase mode
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Grid Snapping")
	UButton* EraseModeButton = nullptr;

	// Button to swap to floor mode
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Grid Snapping")
	UButton* FloorModeButton = nullptr;

	// Tile View Object for displaying the available walls
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTileView* WallSelectionTileView = nullptr;

	/// -- Pointers --
	// Pointer back to the Player_Master UI class
	class UUI_Player_Master* MUI = nullptr;

protected:
	// Pointer to the selected static mesh
	FName SelectedID = "";
	
};
