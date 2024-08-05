// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/BuildToolData.h"

// Default Constructor
FBuildData::FBuildData()
{
}

// Default Deconstructor
FBuildData::~FBuildData()
{
}

// Called to check if a location argument matches the origin of this struct
// Returns true if they match, otherwise returns false
bool FBuildData::GetLocationMatchesOrigin(FVector Location)
{
	if (Origin == Location) {
		return true;
	}
	return false;
}

// Default Constructor
FSelectableWallData::FSelectableWallData()
{
}

// Default Deconstructor
FSelectableWallData::~FSelectableWallData()
{
}

// Default Constructor
FBuildToolData::FBuildToolData()
{
}

// Constructor which allows the data to be setup on construction
FBuildToolData::FBuildToolData(UStaticMesh* NewMesh, FVector NewLocation, FRotator NewRotation, int NewCost)
{
	Mesh = NewMesh;
	Location = NewLocation;
	Rotation = NewRotation;
	Cost = NewCost;
}

// Default Deconstructor
FBuildToolData::~FBuildToolData()
{
}