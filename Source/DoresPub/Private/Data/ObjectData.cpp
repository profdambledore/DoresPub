// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/ObjectData.h"

// Default Constructor
FObjectData::FObjectData()
{
	ObjectTags.Add(FStringValuePair("placementType", "object"));
}

// Default Deconstructor
FObjectData::~FObjectData()
{
}

// Default Constructor
FSubCats::FSubCats()
{
	//X.Add("");
}

// Default Deconstructor
FSubCats::~FSubCats()
{
}

FObjectSnappingData::FObjectSnappingData()
{
}

FObjectSnappingData::~FObjectSnappingData()
{
}
