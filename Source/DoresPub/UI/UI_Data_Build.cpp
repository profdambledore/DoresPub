// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UI_Data_Build.h"
#include "UI/UI_Player_Build.h"

UWallButtonData::UWallButtonData()
{
}

UWallButtonData::~UWallButtonData()
{
}

void UWallButtonData::SetupData(UUI_Player_Build* UI, FName NewName, UTexture2D* NewWallIcon)
{
	BuildUIState = UI;
	RowName = NewName;
	WallIcon = NewWallIcon;
}
