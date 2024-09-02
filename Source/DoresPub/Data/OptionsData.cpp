// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/OptionsData.h"

FEraseOptions::FEraseOptions()
{
}

FEraseOptions::~FEraseOptions()
{
}

void FEraseOptions::UpdateIncludeWalls(bool bNowTrue)
{
	bIncludeWalls = bNowTrue;
}

void FEraseOptions::UpdateIncludeFloors(bool bNowTrue)
{
	bIncludeFloors = bNowTrue;
}

void FEraseOptions::UpdateOverlapMethod(TEnumAsByte<EEraseOverlapMethod> NewSetting)
{
	OverlapMethod = NewSetting;
}
