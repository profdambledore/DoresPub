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

	//ReGenerateBuildData();
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
			// Next, check that a mesh exists on the rotation specified
			if (i.Rotation == FRotator(0, 0, 0)) {
				if (BuildData[BuildDataIndex].XStaticMeshComponent) {
					// If a mesh does exist on the X axis, clear the SMC
					BuildData[BuildDataIndex].XStaticMeshComponent->SetStaticMesh(nullptr);

					// Then add the ID to the output array and clear the ID in the XID
					out.Add(BuildData[BuildDataIndex].XID);
					BuildData[BuildDataIndex].XID = "";

					// Next, remove the pointer in the BuildDataIndex and update the BuildData on the up point
					PTB = BuildData[BuildDataIndex].XStaticMeshComponent;
					BuildData[BuildDataIndex].XStaticMeshComponent = nullptr;
					SMCPool.Remove(PTB);
					SMCPool.Add(PTB);
					BuildData[BuildDataIndex].UpPoint->DownPoint = nullptr;
					BuildData[BuildDataIndex].UpPoint = nullptr;

					if (!BuildData[BuildDataIndex].UpPoint && !BuildData[BuildDataIndex].LeftPoint && !BuildData[BuildDataIndex].DownPoint && !BuildData[BuildDataIndex].RightPoint) {
						BuildData.RemoveAt(BuildDataIndex);
					}
				}
			}
			else {
				if (BuildData[BuildDataIndex].YStaticMeshComponent) {
					// If a mesh does exist on the Y axis, clear the SMC, remove the pointer and update the BuildData on the right point
					BuildData[BuildDataIndex].YStaticMeshComponent->SetStaticMesh(nullptr);

					// Then add the ID to the output array and clear the ID in the YID
					out.Add(BuildData[BuildDataIndex].YID);
					BuildData[BuildDataIndex].YID = "";

					// Next, remove the pointer in the BuildDataIndex and update the BuildData on the right point
					PTB = BuildData[BuildDataIndex].YStaticMeshComponent;
					BuildData[BuildDataIndex].YStaticMeshComponent = nullptr;
					SMCPool.Remove(PTB);
					SMCPool.Add(PTB);
					BuildData[BuildDataIndex].RightPoint->LeftPoint = nullptr;
					BuildData[BuildDataIndex].RightPoint = nullptr;

					if (!BuildData[BuildDataIndex].UpPoint && !BuildData[BuildDataIndex].LeftPoint && !BuildData[BuildDataIndex].DownPoint && !BuildData[BuildDataIndex].RightPoint) {
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

	// Next, figure out if the wall is an X or Y wall (x has a default rotator, while y has a 0, 90, 0 rotator)
	if (SMC->GetComponentRotation() == FRotator(0, 0, 0)) {
		NextBuildData.XStaticMeshComponent = SMC;
		f.X += 250;
		NextBuildData.XID = ID;
	}
	else {
		NextBuildData.YStaticMeshComponent = SMC;
		f.Y += 250;
		NextBuildData.YID = ID;
		bY = true;
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

// Called in GenerateBuildData to update a previous FBuildData struct
void AWorld_BuildingLevel::UpdateBuildData(UStaticMeshComponent* SMC, int Index, FName ID)
{
	bool bY = false;
	FVector f = BuildData[Index].Origin;

	// Next, figure out if the wall is an X or Y wall (X has a default rotator, while Y has a 0, 90, 0 rotator)
	if (SMC->GetComponentRotation() == FRotator(0, 0, 0)) {
		BuildData[Index].XStaticMeshComponent = SMC;
		f.X += 250;
		BuildData[Index].XID = ID;
	}
	else {
		BuildData[Index].YStaticMeshComponent = SMC;
		f.Y += 250;
		BuildData[Index].YID = ID;
		bY = true;
	}

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
			if (BuildData[i].XStaticMeshComponent) {
				// If so, return it
				return BuildData[i].XStaticMeshComponent;
			}
		}
		else {
			// If on Y, check if there is a SMC in the YStaticMeshComponent property
			if (BuildData[i].YStaticMeshComponent) {
				// If so, return it
				return BuildData[i].YStaticMeshComponent;
			}
		}
	}
	// If none of the above are true, return nullptr
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



