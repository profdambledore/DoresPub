// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UI_Player_Build_WallButton.h"

void UUI_Player_Build_WallButton::NativeConstruct()
{
	Super::NativeConstruct();

	WallButton->OnReleased.AddDynamic(this, &UUI_Player_Build_WallButton::OnItemButtonReleased);
}

void UUI_Player_Build_WallButton::ClearSelectedCategory()
{
}