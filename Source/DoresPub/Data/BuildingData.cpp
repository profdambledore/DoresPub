// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/BuildingData.h"

FBuildData::FBuildData()
{
}

FBuildData::~FBuildData()
{
}

bool FBuildData::GetLocationMatchesOrigin(FVector Location)
{
	if (Origin == Location) {
		return true;
	}
	return false;
}
