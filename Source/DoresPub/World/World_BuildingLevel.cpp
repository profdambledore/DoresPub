// Fill out your copyright notice in the Description page of Project Settings.


#include "World/World_BuildingLevel.h"
#include "Player/Player_BuildToolDisplay.h"

// Sets default values
AWorld_BuildingLevel::AWorld_BuildingLevel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = Root;
}

// Called when the game starts or when spawned
void AWorld_BuildingLevel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWorld_BuildingLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/// -- Building Functions --
// Called to add an array of building meshes to the level
void AWorld_BuildingLevel::AddBuildingObjects(TArray<struct FBuildToolData> DataToBuild)
{
	// Start by calculating the amount of SMC's required for the current build + the new build data
	int Required; int Current = 0;
	for (UStaticMeshComponent* i : SMCPool) {
		if (i->GetStaticMesh() != nullptr) {
			Current++;
		}
	}
	Required = Current + DataToBuild.Num();

	//UE_LOG(LogTemp, Warning, TEXT("Current = %i / Required (Current + AmnNew) = %i"), Current, Required);

	// Spawn in any required SMC's
	if (Required > SMCPool.Num()) {
		AddNewStaticMeshComponent(Required);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Spawned = %i"), SMCPool.Num());

	// Then add the data to the building
	for (int i = 0; i < DataToBuild.Num(); i++) {
		SMCPool[Current + i]->SetStaticMesh(DataToBuild[i].Mesh);
		SMCPool[Current + i]->SetRelativeLocation(DataToBuild[i].Location);
		SMCPool[Current + i]->SetRelativeRotation(DataToBuild[i].Rotation);

		// Finally, update the build data with the new SMC
		AddToBuildData(SMCPool[Current + i], DataToBuild[i].ID);
	}
}

// Called to remove an array of building meshes from the level
TArray<FName> AWorld_BuildingLevel::RemoveBuildingObjects(TArray<struct FBuildToolData> DataToRemove)
{
	// Init local variables
	int BuildDataIndex; UStaticMeshComponent* PTB = nullptr;

	// Also init the return array
	TArray<FName> out;

	// For each data to be removed...
	for (FBuildToolData i : DataToRemove) {
		// First, check that the data to be removes actually exists in the BuildData
		BuildDataIndex = GetBuildDataAtLocation(i.Location);
		if (BuildDataIndex != -1) {
			// Check if the building object to remove is a floor mesh
			if (i.ID == "floor") {
				// Check if a floor mesh exists at the location
				if (BuildData[BuildDataIndex].FloorData.StaticMeshComponent) {
					// If so, clear it
					BuildData[BuildDataIndex].FloorData.StaticMeshComponent->SetStaticMesh(nullptr);

					// Then clear the FloorData struct
					BuildData[BuildDataIndex].FloorData = FFloorData();

					if (!BuildData[BuildDataIndex].UpPoint && !BuildData[BuildDataIndex].LeftPoint && !BuildData[BuildDataIndex].DownPoint && !BuildData[BuildDataIndex].RightPoint && !BuildData[BuildDataIndex].FloorData.StaticMeshComponent) {
						BuildData.RemoveAt(BuildDataIndex);
					}
				}
			}
			// Next, check that a mesh exists on the rotation specified
			else if (i.Rotation == FRotator(0, 0, 0)) {
				if (BuildData[BuildDataIndex].XWallData.StaticMeshComponent) {
					// If a mesh does exist on the X axis, clear the SMC
					BuildData[BuildDataIndex].XWallData.StaticMeshComponent->SetStaticMesh(nullptr);

					// Then add the ID to the output array and clear the ID in the XID
					out.Add(BuildData[BuildDataIndex].XWallData.ID);
					BuildData[BuildDataIndex].XWallData.ID = "";

					// Next, remove the pointer in the BuildDataIndex and update the BuildData on the up point
					PTB = BuildData[BuildDataIndex].XWallData.StaticMeshComponent;
					BuildData[BuildDataIndex].XWallData.StaticMeshComponent = nullptr;
					SMCPool.Remove(PTB);
					SMCPool.Add(PTB);
					BuildData[BuildDataIndex].UpPoint->DownPoint = nullptr;
					BuildData[BuildDataIndex].UpPoint = nullptr;

					if (!BuildData[BuildDataIndex].UpPoint && !BuildData[BuildDataIndex].LeftPoint && !BuildData[BuildDataIndex].DownPoint && !BuildData[BuildDataIndex].RightPoint && !BuildData[BuildDataIndex].FloorData.StaticMeshComponent) {
						BuildData.RemoveAt(BuildDataIndex);
					}
				}
			}
			else {
				if (BuildData[BuildDataIndex].YWallData.StaticMeshComponent) {
					// If a mesh does exist on the Y axis, clear the SMC, remove the pointer and update the BuildData on the right point
					BuildData[BuildDataIndex].YWallData.StaticMeshComponent->SetStaticMesh(nullptr);

					// Then add the ID to the output array and clear the ID in the YID
					out.Add(BuildData[BuildDataIndex].YWallData.ID);
					BuildData[BuildDataIndex].YWallData.ID = "";

					// Next, remove the pointer in the BuildDataIndex and update the BuildData on the right point
					PTB = BuildData[BuildDataIndex].YWallData.StaticMeshComponent;
					BuildData[BuildDataIndex].YWallData.StaticMeshComponent = nullptr;
					SMCPool.Remove(PTB);
					SMCPool.Add(PTB);
					BuildData[BuildDataIndex].RightPoint->LeftPoint = nullptr;
					BuildData[BuildDataIndex].RightPoint = nullptr;

					if (!BuildData[BuildDataIndex].UpPoint && !BuildData[BuildDataIndex].LeftPoint && !BuildData[BuildDataIndex].DownPoint && !BuildData[BuildDataIndex].RightPoint && !BuildData[BuildDataIndex].FloorData.StaticMeshComponent) {
						BuildData.RemoveAt(BuildDataIndex);
					}
				}
			}
		}
	}
	return out;
}

// Called to add to the current BuildData
void AWorld_BuildingLevel::AddToBuildData(UStaticMeshComponent* ComponentToAdd, FName ID)
{
	if (ComponentToAdd->GetStaticMesh()) {
		int j = GetBuildDataAtLocation(ComponentToAdd->GetComponentLocation());
		if (j != -1) {
				UpdateBuildData(ComponentToAdd, j, ID);
		}
		else {
				AddNewBuildData(ComponentToAdd, ID);
		}
	}
}

void AWorld_BuildingLevel::UpdateWallID(FVector Location, FRotator Rotation, FName NewID)
{
	// First, find the BuildData at the location
	int DataIndex = GetBuildDataAtLocation(Location);

	UE_LOG(LogTemp, Warning, TEXT("DataIndex = %i"), DataIndex);

	// Next, figure out if the wall is an X or Y wall (X has a default rotator, while Y has a 0, 90, 0 rotator)
	if (Rotation == FRotator(0, 0, 0)) {
		BuildData[DataIndex].XWallData.SetNewID(NewID);
	}
	else {
		BuildData[DataIndex].YWallData.SetNewID(NewID);
	}
}

// Called to regenerate a BuildData array from scratch
void AWorld_BuildingLevel::ReGenerateBuildData()
{
	// Clear the BuildData array
	BuildData.Empty();

	// Now repeat for the remaining SMC's
	for (int i = 0; i < SMCPool.Num(); i++) {
		if (SMCPool[i]->GetStaticMesh()) {
			int j = GetBuildDataAtLocation(SMCPool[i]->GetComponentLocation());
			if (j != -1) {
				UE_LOG(LogTemp, Warning, TEXT("Updating, there are %i indecies currently"), BuildData.Num())
					UpdateBuildData(SMCPool[i], j);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Adding, there are %i indecies currently"), BuildData.Num())
					AddNewBuildData(SMCPool[i]);
			}
		}
	}
}

// Called in GenerateBuildData to insert a new FBuildData struct
void AWorld_BuildingLevel::AddNewBuildData(UStaticMeshComponent* SMC, FName ID)
{
	// Initialize the variables for adding a new build data
	FBuildData NextBuildData; bool bIsHalfWall = false;
	NextBuildData.Origin = SMC->GetComponentLocation();

	FVector f = NextBuildData.Origin;
	bool bY = false;

	// Check if the data we are trying to insert is a floor data
	if (ID == "floor") {
		NextBuildData.FloorData.StaticMeshComponent = SMC;
		//NextBuildData.FloorData.Material = SMC->GetMaterial(0)->GetMaterial();

		BuildData.Add(NextBuildData);
	}
	// Else, insert the data as a wall
	else {
		// Next, figure out if the wall is an X or Y wall (x has a default rotator, while y has a 0, 90, 0 rotator)
		if (SMC->GetComponentRotation() == FRotator(0, 0, 0)) {
			NextBuildData.XWallData.StaticMeshComponent = SMC;
			f.X += 250;
			NextBuildData.XWallData.ID = ID;
			//SetupDefaultMaterials(NextBuildData.XWallData);
		}
		else {
			NextBuildData.YWallData.StaticMeshComponent = SMC;
			f.Y += 250;
			NextBuildData.YWallData.ID = ID;
			bY = true;
			//SetupDefaultMaterials(NextBuildData.YWallData);
		}
		BuildData.Add(NextBuildData);

		// Then figure out if we need to create a new FBuildData for the other connecting point
		int j = GetBuildDataAtLocation(f);
		if (j != -1) {
			if (!bY) {
				// If it does exist, update it
				BuildData[j].DownPoint = &BuildData[BuildData.Num() - 1];
				BuildData[BuildData.Num() - 1].UpPoint = &BuildData[j];
			}
			else {
				// If it does exist, update it
				BuildData[j].LeftPoint = &BuildData[BuildData.Num() - 1];
				BuildData[BuildData.Num() - 1].RightPoint = &BuildData[j];
			}
		}
		else {
			if (!bY) {
				// Else, create a new one
				NextBuildData = FBuildData();
				NextBuildData.Origin = f;
				NextBuildData.DownPoint = &BuildData[BuildData.Num() - 1];
				BuildData.Add(NextBuildData);
				BuildData[BuildData.Num() - 2].UpPoint = &BuildData[BuildData.Num() - 1];
			}
			else {
				// Else, create a new one
				NextBuildData = FBuildData();
				NextBuildData.Origin = f;
				NextBuildData.LeftPoint = &BuildData[BuildData.Num() - 1];
				BuildData.Add(NextBuildData);
				BuildData[BuildData.Num() - 2].RightPoint = &BuildData[BuildData.Num() - 1];
			}
		}
	}
}

// Called in GenerateBuildData to update a previous FBuildData struct
void AWorld_BuildingLevel::UpdateBuildData(UStaticMeshComponent* SMC, int Index, FName ID)
{
	bool bY = false;
	FVector f = BuildData[Index].Origin;

	// Check if the data we are trying to insert is a floor data
	if (ID == "floor") {
		BuildData[Index].FloorData.StaticMeshComponent = SMC;
		//BuildData[Index].FloorData.Material = SMC->GetMaterial(0)->GetMaterial();
	}
	// Else, insert the data as a wall
	else {
		// Next, figure out if the wall is an X or Y wall (X has a default rotator, while Y has a 0, 90, 0 rotator)
		if (SMC->GetComponentRotation() == FRotator(0, 0, 0)) {
			BuildData[Index].XWallData.StaticMeshComponent = SMC;
			f.X += 250;
			BuildData[Index].XWallData.ID = ID;
			//SetupDefaultMaterials(BuildData[Index].XWallData);
		}
		else {
			BuildData[Index].YWallData.StaticMeshComponent = SMC;
			f.Y += 250;
			BuildData[Index].YWallData.ID = ID;
			bY = true;
			//SetupDefaultMaterials(BuildData[Index].YWallData);
		}

		// Then figure out if we need to create a new FBuildData for the other connecting point
		int j = GetBuildDataAtLocation(f);
		if (j != -1) {
			if (!bY) {
				// If it does exist, update it
				BuildData[j].DownPoint = &BuildData[Index];
				BuildData[Index].UpPoint = &BuildData[j];
			}
			else {
				// If it does exist, update it
				BuildData[j].LeftPoint = &BuildData[Index];
				BuildData[Index].RightPoint = &BuildData[j];
			}
		}
		else {
			if (!bY) {
				// Else, create a new one
				FBuildData NextBuildData = FBuildData();
				NextBuildData.Origin = f;
				NextBuildData.DownPoint = &BuildData[Index];
				BuildData.Add(NextBuildData);
				BuildData[Index].UpPoint = &BuildData[BuildData.Num() - 1];
			}
			else {
				// Else, create a new one
				FBuildData NextBuildData = FBuildData();
				NextBuildData.Origin = f;
				NextBuildData.LeftPoint = &BuildData[Index];
				BuildData.Add(NextBuildData);
				BuildData[Index].RightPoint = &BuildData[BuildData.Num() - 1];
			}
		}
	}
}

void AWorld_BuildingLevel::SetupDefaultMaterials(FWallData& WallDataToUpdate)
{
	int mats = WallDataToUpdate.StaticMeshComponent->GetNumMaterials();
	UE_LOG(LogTemp, Warning, TEXT("mats amn = %i"), mats);
	for (int i = 0; i < mats; i++) {
		WallDataToUpdate.Materials.Add(WallDataToUpdate.StaticMeshComponent->GetMaterial(i)->GetMaterial());
	}
}

/// -- Object Functions --
// Called to add a new object at a position
// TO:DO - Update this to use Actors instead, not implemented yet so do that first
void AWorld_BuildingLevel::AddObjectToLevel(UStaticMesh* MeshToSpawn, FTransform MeshTransform)
{
	UE_LOG(LogTemp, Warning, TEXT("Spawning a new object at %s"), *MeshTransform.GetLocation().ToString());
	UStaticMeshComponent* NewMeshComp = NewObject<UStaticMeshComponent>(this, FName(*FString::Printf(TEXT("Object Mesh"))));
	NewMeshComp->RegisterComponent();
	NewMeshComp->SetStaticMesh(MeshToSpawn);
	NewMeshComp->SetWorldTransform(MeshTransform);
}

/// -- Uitlity Functions --
// Called to return the mesh of a static mesh component at a location and rotation
// If returned nullptr, then none was found
UStaticMeshComponent* AWorld_BuildingLevel::GetWallObjectMeshAtPosition(FVector Location, bool bOnXAxis)
{
	// Check if a BuildData exists at the inputted location
	int i = GetBuildDataAtLocation(Location);
	// If one does, check if the ouptut is wanted on the X or Y axis
	if (i != -1) {
		if (bOnXAxis) {
			// If on X, check if there is a SMC in the XStaticMeshComponent property
			if (BuildData[i].XWallData.StaticMeshComponent) {
				// If so, return it
				return BuildData[i].XWallData.StaticMeshComponent;
			}
		}
		else {
			// If on Y, check if there is a SMC in the YStaticMeshComponent property
			if (BuildData[i].YWallData.StaticMeshComponent) {
				// If so, return it
				return BuildData[i].YWallData.StaticMeshComponent;
			}
		}
	}
	// If none of the above are true, return nullptr
	return nullptr;
}

UStaticMeshComponent* AWorld_BuildingLevel::GetFloorObjectMeshAtPosition(FVector Location)
{
	// Check if a BuildData exists at the inputted location
	int i = GetBuildDataAtLocation(Location);
	// If one does,...
	if (i != -1) {
		// Check if there is a SMC in the FloorData static mesh property
		if (BuildData[i].FloorData.StaticMeshComponent) {
			// If so, return it
			return BuildData[i].FloorData.StaticMeshComponent;
		}
	}

	return nullptr;
}

// Called to add a new StaticMeshComponent to the BuildToolDisplay
void AWorld_BuildingLevel::AddNewStaticMeshComponent(int Target)
{
	int NewNumber = SMCPool.Num() + 1;

	UStaticMeshComponent* NewMeshComp = NewObject<UStaticMeshComponent>(this, FName(*FString::Printf(TEXT("Wall Mesh %i"), NewNumber)));
	NewMeshComp->RegisterComponent();
	NewMeshComp->SetCollisionProfileName(FName("Building"));
	SMCPool.Add(NewMeshComp);

	// Check if there is now enough component sets added.  If not, recurse
	if (SMCPool.Num() < Target) {
		AddNewStaticMeshComponent(Target);
	}
}

// Called to find the BuildData struct index based on an inputted FVector.
// Returns an empty struct if one doesn't exist
int AWorld_BuildingLevel::GetBuildDataAtLocation(FVector Location)
{
	for (int i = 0; i < BuildData.Num(); i++) {
		if (BuildData[i].Origin == Location) {
			return i;
		}
	}
	return -1;
}



