// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UI_Player_Object_ItemButton.h"

void UUI_Player_Object_ItemButton::NativeConstruct()
{
	Super::NativeConstruct();

	ItemButton->OnReleased.AddDynamic(this, &UUI_Player_Object_ItemButton::OnItemButtonReleased);
}

void UUI_Player_Object_ItemButton::ClearSelectedCategory()
{
}
