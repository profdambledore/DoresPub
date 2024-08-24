// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Button.h"
#include "Components/TileView.h"
#include "Components/WidgetSwitcher.h"

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

	// Called to add a new selectable wall to the list view
	void AddSelectableWindowToList(FName ID, FSelectableWallData WallData);

	// Called to update the selected wall in the BuildToolDisplay
	UFUNCTION(BlueprintCallable)
	void UpdateSelectedWall(FName ObjectID, class UUI_Player_Build_WallButton* NewSelectedButton);

	// Called to update the selected wall in the BuildToolDisplay
	UFUNCTION(BlueprintCallable)
	void UpdateSelectedWindow(FName ObjectID, class UUI_Player_Build_WallButton* NewSelectedButton);

	// Called to update the colour of the selected sub tool button if it's enabled or not
	void UpdateSelectedSubToolButton(TEnumAsByte<EBuildToolSubType> NewSubType);

	// Called to update the colour of the erase button if it's enabled or not
	void UpdateEraseModeEnabled(bool bEnabled);

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

	// Button event to swap to window mode
	UFUNCTION()
	void OnWindowButtonReleased();

public:
	/// -- Components --
	// Widget switcher that allows the UI to swap between erase mode enabled and disabled
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Global Components")
	UWidgetSwitcher* EraseModeSwitcher = nullptr;

	// Button to swap to wall mode
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sub Mode Buttons")
	UButton* WallModeButton = nullptr;

	// Button to swap to erase mode
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sub Mode Buttons")
	UButton* EraseModeButton = nullptr;

	// Button to swap to floor mode
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sub Mode Buttons")
	UButton* FloorModeButton = nullptr;

	// Button to swap to window mode
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sub Mode Buttons")
	UButton* WindowModeButton = nullptr;

	// Widget switcher that allows the UI to swap between different sub states
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Global Components")
	UWidgetSwitcher* SubStateSwitcher = nullptr;

	// Tile View Object for displaying the available walls
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Global Components")
	UTileView* WallSelectionTileView = nullptr;

	// Tile View Object for displaying the available windows
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Global Components")
	UTileView* WindowSelectionTileView = nullptr;

	/// -- Pointers --
	// Pointer back to the Player_Master UI class
	class UUI_Player_Master* MUI = nullptr;
	
	/// -- Colours --
	// FLinearColor of the button when selected
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FLinearColor SelectedButtonColour;

	// FLinearColor of the button when un-selected
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FLinearColor UnselectedButtonColour;

protected:
	// FName to the selected wall ID
	FName SelectedID = "";

	// Pointer to the selected wall button
	class UUI_Player_Build_WallButton* SelectedItemButton = nullptr;

private:
	// Pointer to the current selected button
	UButton* SelectedButton = nullptr;

	
};
