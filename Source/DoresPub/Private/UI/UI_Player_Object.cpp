// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/UI_Player_Object.h"

#include "UI/UI_Data_Object.h"
#include "UI/UI_Player_Object_CatButton.h"
#include "UI/UI_Player_Object_ItemButton.h"
#include "UI/UI_Player_Master.h"
#include "Player/Player_Tool_Object.h"

#include "Engine/Texture2D.h"

void UUI_Player_Object::NativeConstruct()
{
	Super::NativeConstruct();

	// Also update the CategoryButtonTileView
	// Ignore Default
	for (EObjectType i : TEnumRange<EObjectType>()) {
		if (i != DefaultType) {
			UCategoryButtonData* NewCatObj = NewObject<UCategoryButtonData>();
			NewCatObj->SetupData(this, i);
			CategoryTileView->AddItem(NewCatObj);
		}
	}

	// Bind button events
	//RotationModeButton->OnReleased.AddDynamic(this, &UUI_Player_Object::OnRMButtonReleased);
}

void UUI_Player_Object::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UUI_Player_Object::UpdateObjectTileView()
{
	// Start by clearing the TileView
	ObjectTileView->ClearListItems();

	// Get the object data table from the Object Tool
	UDataTable* ObjectsAvailable = ObjectTool->GetObjectDataTable();

	// Store an array for all row names of the data table
	TArray<FName> RowNames = ObjectsAvailable->GetRowNames();

	// Next, figure out what items should be shown.
	// If no category is selected, show all items
	if (CurrentSelectedObjectType == EObjectType::DefaultType) {
		for (FName i : RowNames) {
			AddItemToObjectTileView(*ObjectsAvailable->FindRow<FObjectData>(i, ""), i);
		}
	}

	// If a category is selected, but a sub-category is not selected, show all of the category items only
	else if (CurrentSelectedSubCategory == "") {
		for (FName i : RowNames) {
			FObjectData* Data = ObjectsAvailable->FindRow<FObjectData>(i, "");
			if (Data->Category == CurrentSelectedObjectType) {
				AddItemToObjectTileView(*ObjectsAvailable->FindRow<FObjectData>(i, ""), i);
			}
		}
	}

	// If both a Category and SubCategory are selected, then only show items of the sub-category
	else {
		for (FName i : RowNames) {
			FObjectData* Data = ObjectsAvailable->FindRow<FObjectData>(i, "");
			if (Data->Category == CurrentSelectedObjectType) {
				if (Data->SubCategory == CurrentSelectedSubCategory) {
					AddItemToObjectTileView(*ObjectsAvailable->FindRow<FObjectData>(i, ""), i);
				}
			}
		}
	}
}

void UUI_Player_Object::UpdateSubCategoryButtons()
{
	// Start by clearing the SubCategoryTileView
	SubCategoryTileView->ClearListItems();

	// Next, find the matching sub category index in the map
	TArray<FString> FoundSubCats = SubCategories.FindRef(CurrentSelectedObjectType).X;

	// For each sub category (plus the blank all category), create a USubCategoryButtonData and add it to the list view
	for (FString i : FoundSubCats) {
		USubCategoryButtonData* NewSubCatObj = NewObject<USubCategoryButtonData>();
		NewSubCatObj->SetupData(this, i);
		SubCategoryTileView->AddItem(NewSubCatObj);
	}
}

void UUI_Player_Object::AddItemToObjectTileView(FObjectData Data, FName ID)
{
	UItemButtonData* ItemDataObject = NewObject<UItemButtonData>();
	ItemDataObject->SetupData(this, ID, Data.Icon);
	ObjectTileView->AddItem(ItemDataObject);
}

void UUI_Player_Object::UpdateSelectedCategory(TEnumAsByte<EObjectType> NewCategory, class UUI_Player_Object_CatButton* SelectedButton)
{
	// Update CurrentSelectedObjectType and upate the SubCatButtons
	if (CurrentSelectedObjectType == NewCategory) {
		CurrentSelectedObjectType = EObjectType::DefaultType;
		CurrentSelectedSubCategory = "";
	}
	else {
		CurrentSelectedObjectType = NewCategory;
	}

	UpdateSubCategoryButtons();

	// Then update the ObjectTileView
	UpdateObjectTileView();
}

void UUI_Player_Object::UpdateSelectedSubCategory(FString NewSubCategory, class UUI_Player_Object_CatButton* SelectedButton)
{
	// Update the selected SubCategory
	if (CurrentSelectedSubCategory == NewSubCategory) {
		CurrentSelectedSubCategory = "";
	}
	else {
		CurrentSelectedSubCategory = NewSubCategory;
	}
	

	// Then update the ObjectTileView
	UpdateObjectTileView();
}

void UUI_Player_Object::UpdateSelectedObject(FName ObjectID)
{
	ObjectTool->UpdateObjectMesh(ObjectID);
}

void UUI_Player_Object::OnRMButtonReleased()
{
	ObjectTool->ToggleRotationMode();
}
