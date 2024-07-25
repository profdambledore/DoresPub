// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UI_Player_ObjectData.h"
#include "UI/UI_Player_Object.h"


UCategoryButtonData::UCategoryButtonData()
{
}

UCategoryButtonData::~UCategoryButtonData()
{
}

void UCategoryButtonData::SetupData(UUI_Player_Object* UI, TEnumAsByte<EObjectType> NewCategory)
{
	ObjectUIState = UI;
	Category = NewCategory;
}

USubCategoryButtonData::USubCategoryButtonData()
{
}

USubCategoryButtonData::~USubCategoryButtonData()
{
}

void USubCategoryButtonData::SetupData(UUI_Player_Object* UI, FString SubCatName)
{
	ObjectUIState = UI;
	SubCategory = SubCatName;
}

UItemButtonData::UItemButtonData()
{
}

UItemButtonData::~UItemButtonData()
{
}

void UItemButtonData::SetupData(UUI_Player_Object* UI, FName IDName, UTexture2D* NewItemIcon)
{
	ObjectUIState = UI;
	ID = IDName;
	ItemIcon = NewItemIcon;
}


