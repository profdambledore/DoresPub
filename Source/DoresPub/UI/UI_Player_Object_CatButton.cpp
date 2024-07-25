// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UI_Player_Object_CatButton.h"

void UUI_Player_Object_CatButton::NativeConstruct()
{
	Super::NativeConstruct();

	CategoryButton->OnReleased.AddDynamic(this, &UUI_Player_Object_CatButton::OnCategoryButtonReleased);
}

void UUI_Player_Object_CatButton::ClearSelectedCategory()
{
}
