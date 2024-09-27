// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/TileView.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"

#include "Data/ObjectData.h"

#include "UI_Player_Object.generated.h"

UCLASS()
class DORESPUB_API UUI_Player_Object : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	virtual void SynchronizeProperties() override;

	// Called to update the objects shown in the ObjectTileView 
	void UpdateObjectTileView();

protected:
	/// -- State Functions --
	// Called to update the sub category buttons based on the selected category
	void UpdateSubCategoryButtons();

	// Called to add a new object to the ObjectTileView
	void AddItemToObjectTileView(FObjectData Data, FName ID);

	// Called to update the current selected category to the input
	UFUNCTION(BlueprintCallable)
	void UpdateSelectedCategory(TEnumAsByte<EObjectType> NewCategory, class UUI_Player_Object_CatButton* SelectedButton);

	// Called to update the current selected sub category to the input
	UFUNCTION(BlueprintCallable)
	void UpdateSelectedSubCategory(FString NewSubCategory, class UUI_Player_Object_CatButton* SelectedButton);

	// Called to update the shown object in the Player_Tools ObjectToolMeshComponent
	UFUNCTION(BlueprintCallable)
	void UpdateSelectedObject(FName ObjectID);

	/// -- Button Functions --
	// Button event to swap to/from rotation mode
	UFUNCTION()
	void OnRMButtonReleased();

public:
	/// -- Components --
	// Tile View Object for displaying the category buttons
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTileView* CategoryTileView = nullptr;

	// Tile View Object for displaying the sub category buttons
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTileView* SubCategoryTileView = nullptr;

	// Tile View Object for displaying the available objects
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTileView* ObjectTileView = nullptr;

	// Button to swap to/from rotation mode
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Rotation Mode")
	UButton* RotationModeButton = nullptr;

	// TMap of all available sub categories
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Category Data")
	TMap<TEnumAsByte<EObjectType>, FSubCats> SubCategories;

	/// -- Pointers --
	// Pointer back to the Player_Master UI class
	class UUI_Player_Master* MUI = nullptr;

	// Pointer to the Build Tool owned by the player
	class APlayer_Tool_Object* ObjectTool = nullptr;

protected:
	/// -- Actives --
	TEnumAsByte<EObjectType> CurrentSelectedObjectType;

	FString CurrentSelectedSubCategory = "";

	class UUI_Player_Object_CatButton* CurrentSelectedCatButton = nullptr;
	class UUI_Player_Object_CatButton* CurrentSelectedSubCatButton = nullptr;
};
