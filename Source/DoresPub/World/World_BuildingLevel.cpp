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

void AWorld_BuildingLevel::AddBuildingObjects(TArray<struct FBuildingData> DataToBuild)
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
	}

	ReGenerateBuildData();
}

void AWorld_BuildingLevel::AddObjectToLevel(UStaticMesh* MeshToSpawn, FTransform MeshTransform)
{
	UE_LOG(LogTemp, Warning, TEXT("Spawning a new object at %s"), *MeshTransform.GetLocation().ToString());
	UStaticMeshComponent* NewMeshComp = NewObject<UStaticMeshComponent>(this, FName(*FString::Printf(TEXT("Object Mesh"))));
	NewMeshComp->RegisterComponent();
	NewMeshComp->SetStaticMesh(MeshToSpawn);
	NewMeshComp->SetWorldTransform(MeshTransform);
}

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

UStaticMeshComponent* AWorld_BuildingLevel::GetWallObjectMeshAtPosition(FVector Location, bool bOnXAxis)
{
	int i = GetBuildDataAtLocation(Location);
	if (i != -1) {
		if (bOnXAxis) {
			if (BuildData[i].XStaticMeshComponent) {
				return BuildData[i].XStaticMeshComponent;
			}
		}
		else {
			if (BuildData[i].YStaticMeshComponent) {
				return BuildData[i].YStaticMeshComponent;
			}
		}
	}
	return nullptr;
}

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

void AWorld_BuildingLevel::AddNewBuildData(UStaticMeshComponent* SMC)
{
	// Initialize the variables for adding a new build data
	FBuildData NextBuildData; bool bIsHalfWall = false;
	NextBuildData.Origin = SMC->GetComponentLocation();

	FVector f = NextBuildData.Origin;
	bool bY = false;

	// Next, figure out if the wall is an X or Y wall (x has a default rotator, while y has a 0, 90, 0 rotator)
	if (SMC->GetComponentRotation() == FRotator(0, 0, 0)) {
		NextBuildData.XStaticMeshComponent = SMC;
		f.X += 125;
		
	}
	else {
		NextBuildData.YStaticMeshComponent = SMC;
		f.Y += 125;
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
			BuildData[j].RightPoint = &BuildData[BuildData.Num() - 1];
			BuildData[BuildData.Num() - 1].LeftPoint = &BuildData[j];
		}
	}
	else {
		if (!bY) {
			// Else, create a new one
			NextBuildData = FBuildData();
			NextBuildData.Origin = f;
			NextBuildData.RightPoint = &BuildData[BuildData.Num() - 1];
			BuildData.Add(NextBuildData);
			BuildData[BuildData.Num() - 2].LeftPoint = &BuildData[BuildData.Num() - 1];
		}
		else {
			// Else, create a new one
			NextBuildData = FBuildData();
			NextBuildData.Origin = f;
			NextBuildData.DownPoint = &BuildData[BuildData.Num() - 1];
			BuildData.Add(NextBuildData);
			BuildData[BuildData.Num() - 2].UpPoint = &BuildData[BuildData.Num() - 1];
			
		}
	}
}

void AWorld_BuildingLevel::UpdateBuildData(UStaticMeshComponent* SMC, int Index)
{
	bool bIsHalfWall = false;
	bool bY = false;
	FVector f = BuildData[Index].Origin;

	// Figure out if the wall is a full or half wall
	if (round(SMC->GetStaticMesh()->GetBoundingBox().GetSize().X) == 125) {
		bIsHalfWall = true;
	}

	// Next, figure out if the wall is an X or Y wall (X has a default rotator, while Y has a 0, 90, 0 rotator)
	if (SMC->GetComponentRotation() == FRotator(0, 0, 0)) {
		BuildData[Index].XStaticMeshComponent = SMC;
		f.X += bIsHalfWall ? 125 : 250;
		
	}
	else {
		BuildData[Index].YStaticMeshComponent = SMC;
		f.Y += bIsHalfWall ? 125 : 250;
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
			BuildData[j].RightPoint = &BuildData[BuildData.Num() - 1];
			BuildData[BuildData.Num() - 1].LeftPoint = &BuildData[j];
		}
	}
	else {
		if (!bY) {
			// Else, create a new one
			FBuildData NextBuildData = FBuildData();
			NextBuildData.Origin = f;
			NextBuildData.RightPoint = &BuildData[Index];
			BuildData.Add(NextBuildData);
			BuildData[Index].LeftPoint = &BuildData[BuildData.Num() - 1];
		}
		else {
			// Else, create a new one
			FBuildData NextBuildData = FBuildData();
			NextBuildData.Origin = f;
			NextBuildData.DownPoint = &BuildData[Index];
			BuildData.Add(NextBuildData);
			BuildData[Index].UpPoint = &BuildData[BuildData.Num() - 1];
			
		}
	}
}

int AWorld_BuildingLevel::GetBuildDataAtLocation(FVector Location)
{
	for (int i = 0; i < BuildData.Num(); i++) {
		if (BuildData[i].Origin == Location) {
			return i;
		}
	}
	return -1;
}

