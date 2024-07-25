// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/Player_BuildToolDisplay.h"

#include "Kismet/GameplayStatics.h"

#include "World/World_BuildingLevel.h"

FBuildingData::FBuildingData()
{
}

FBuildingData::FBuildingData(UStaticMesh* NewMesh, FVector NewLocation, FRotator NewRotation)
{
	Mesh = NewMesh;
	Location = NewLocation;
	Rotation = NewRotation;
}

FBuildingData::~FBuildingData()
{
}

//// ------
// Sets default values
APlayer_BuildToolDisplay::APlayer_BuildToolDisplay()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
		
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = Root;

}

// Called when the game starts or when spawned
void APlayer_BuildToolDisplay::BeginPlay()
{
	Super::BeginPlay();

	InitializeMaterial();

	// TEMP -- Find the ground floor and store a pointer to it
	GroundFloor = Cast<AWorld_BuildingLevel>(UGameplayStatics::GetActorOfClass(GetWorld(), AWorld_BuildingLevel::StaticClass()));
}

// Called every frame
void APlayer_BuildToolDisplay::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayer_BuildToolDisplay::UpdateDisplayValidity(bool bIsValid)
{
	if (bIsValid) {
		UpdateMaterial(ValidColour);
	}
	else {
		UpdateMaterial(InvalidColour);
	}
}

int APlayer_BuildToolDisplay::GetDisplayWallsInUse()
{
	int amn = 0;
	for (int i = 0; i < Total; i++) {
		if (SMCPool[i]->GetStaticMesh() == WallMesh) {
			amn += 2;
		}
		else if (SMCPool[i]->GetStaticMesh() == HalfWallMesh) {
			amn++;
		}
	}
	return amn;
}

TArray<FBuildingData> APlayer_BuildToolDisplay::GetDisplayData()
{
	TArray<FBuildingData> out; FBuildingData curr;

	for (UStaticMeshComponent* i : SMCPool) {
		if (i->GetStaticMesh() != nullptr) {
			curr.Mesh = i->GetStaticMesh();
			curr.Location = i->GetComponentLocation();
			curr.Rotation = i->GetComponentRotation();
			out.Add(curr);
		}
	}

	return out;
}

void APlayer_BuildToolDisplay::GenerateNewBuildDisplay(FVector StartPosition, FVector EndPosition)
{
	// Move the BuildToolDisplay to the StartPosition
	SetActorLocation(StartPosition);

	// Start by calculating how many static mesh components are needed
	int XRequires = 0;	int YRequires = 0;

	// Calculate how many are needed for a single x edge (will double for a full box)
	// By calculating the difference between the start and end position, making the output absolute (always pos)
	// Then dividing that by the wall length (250 uu) and rounding up with ceil
	XRequires = ceil(fabs(EndPosition.X - StartPosition.X) / WallSize);

	// Do the same to the Y axis
	YRequires = ceil(fabs(EndPosition.Y - StartPosition.Y) / WallSize);

	// Then calculate the total needed + 4 for the pillar corner blocks
	// If both X and Y require no walls, then don't add any to the required total
	Total = (XRequires * ((YRequires != 0) ? 2 : 1)) + (YRequires * ((XRequires != 0) ? 2 : 1));
	Total = ((Total != 0) ? Total + 4 : 0);

	//UE_LOG(LogTemp, Warning, TEXT("XReq =  %i, YReq =  %i, Total =  %i, Spawned = %i, Need to clear = %i"), XRequires, YRequires, Total, SMCPool.Num(), SMCPool.Num() - Total);

	// Add enough static mesh components to match the amount needed 
	if (Total > SMCPool.Num()) {
		AddNewStaticMeshComponent(Total);
	}

	// Check if the X axis requires 0 SMC while Y requires some (build a wall on the Y axis only)
	if (XRequires == 0 && YRequires != 0) {
		CreateYWall(StartPosition.Y, EndPosition.Y, StartPosition.X, YRequires, 0);
	}

	// Check if the Y axis requires 0 SMC while X requires some (build a wall on the X axis only)
	else if (XRequires != 0 && YRequires == 0) {
		CreateXWall(StartPosition.X, EndPosition.X, StartPosition.Y, XRequires, 0);
	}

	// Else, check that both are not 0
	else if (XRequires != 0 && YRequires != 0) {
		// Start with the X walls, then the Y walls
		CreateXWall(StartPosition.X, EndPosition.X, StartPosition.Y, XRequires, 0);
		CreateXWall(StartPosition.X, EndPosition.X, EndPosition.Y, XRequires, XRequires);

		CreateYWall(StartPosition.Y, EndPosition.Y, StartPosition.X, YRequires, (XRequires * 2));
		CreateYWall(StartPosition.Y, EndPosition.Y, EndPosition.X, YRequires, (XRequires * 2) + YRequires);
	}

	if (Total != 0) {
		CreatePillars(StartPosition, EndPosition);
	}

	// Finally, clear the excees SMC
	for (int j = Total; j < SMCPool.Num(); j++) {
		SMCPool[j]->SetStaticMesh(nullptr);
	}
}

void APlayer_BuildToolDisplay::ClearBuildDisplay()
{
	// Simply clear each spawned SMC in the SMCPoo
	for (UStaticMeshComponent* i : SMCPool) {
		i->SetStaticMesh(nullptr);
	}
}

void APlayer_BuildToolDisplay::AddNewStaticMeshComponent(int Target)
{
	int NewNumber = SMCPool.Num() + 1;

	UStaticMeshComponent* NewMeshComp = NewObject<UStaticMeshComponent>(this, FName(*FString::Printf(TEXT("Wall Mesh %i"), NewNumber)));
	NewMeshComp->RegisterComponent();
	NewMeshComp->SetMaterial(0, BuildToolMaterial);
	NewMeshComp->SetMaterial(1, BuildToolMaterial);
	NewMeshComp->SetCollisionProfileName(FName("BuildingTool"));
	SMCPool.Add(NewMeshComp);

	// Check if there is now enough component sets added.  If not, recurse
	if (SMCPool.Num() < Target) {
		AddNewStaticMeshComponent(Target);
	}
}

void APlayer_BuildToolDisplay::CreateXWall(float Start, float End, float Y, int WallSegments, int StartWallSegment)
{
	// Calculate which point is the start
	int WallStart = (End - Start < -1) ? End : Start;
	int WallEnd = (End - Start < -1) ? Start : End;

	// For the amount X requires, place a SMC every 250 uu
	for (int i = 0; i < WallSegments; i++) {
		SMCPool[StartWallSegment + i]->SetRelativeLocation(FVector(WallStart + (WallSize * i), Y, 1.0f));
		SMCPool[StartWallSegment + i]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		SMCPool[StartWallSegment + i]->SetStaticMesh(WallMesh);

		if (i == WallSegments - 1) {
			// For the final wall, check if it needs to be a half wall
			// If it hasent been cleared 
			if (fabs((WallEnd - WallStart) - ((WallSegments - 1) * (WallSize))) == WallSize / 2) {
				SMCPool[StartWallSegment + WallSegments - 1]->SetStaticMesh(HalfWallMesh);
			}
		}

		UStaticMeshComponent* overlapSMC = GroundFloor->GetWallObjectMeshAtPosition(FVector(WallStart + (WallSize * i), Y, 1.0f), SMCPool[StartWallSegment + i]->GetForwardVector(), SMCPool[StartWallSegment + i]->GetStaticMesh());

		if (overlapSMC) {
			if (overlapSMC->GetStaticMesh() == WallMesh) {
				SMCPool[StartWallSegment + i]->SetStaticMesh(nullptr);
			}
		}
	}

	
}

void APlayer_BuildToolDisplay::CreateYWall(float Start, float End, float X, int WallSegments, int StartWallSegment)
{
	// Calculate which point is the start
	int WallStart = (End - Start < -1) ? End : Start;
	int WallEnd = (End - Start < -1) ? Start : End;

	// For the amount X requires, place a SMC every 250 uu
	for (int i = 0; i < WallSegments; i++) {
		SMCPool[StartWallSegment + i]->SetRelativeLocation(FVector(X, WallStart + (WallSize * i), 1.0f));
		SMCPool[StartWallSegment + i]->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
		SMCPool[StartWallSegment + i]->SetStaticMesh(WallMesh);

		if (i == WallSegments - 1) {
			// For the final wall, check if it needs to be a half wall
			if (fabs((WallEnd - WallStart) - ((WallSegments - 1) * WallSize)) == WallSize / 2) {
				SMCPool[StartWallSegment + WallSegments - 1]->SetStaticMesh(HalfWallMesh);
			}
		}

		UStaticMeshComponent* overlapSMC = GroundFloor->GetWallObjectMeshAtPosition(FVector(X, WallStart + (WallSize * i), 1.0f), SMCPool[StartWallSegment + i]->GetForwardVector(), SMCPool[StartWallSegment + i]->GetStaticMesh());
		if (overlapSMC) {
			if (overlapSMC->GetStaticMesh() == SMCPool[StartWallSegment + i]->GetStaticMesh()) {
				SMCPool[StartWallSegment + i]->SetStaticMesh(nullptr);
			}
		}
	}
}

void APlayer_BuildToolDisplay::CreatePillars(FVector Start, FVector End)
{
	int used = 4;

	UStaticMeshComponent* overlapSMC;

	// Create a pillar at the start point
	SMCPool[Total - used]->SetRelativeLocation(FVector(Start.X, Start.Y, 1.0f));
	SMCPool[Total - used]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	SMCPool[Total - used]->SetStaticMesh(PillarWallMesh);

	overlapSMC = GroundFloor->GetWallObjectMeshAtPosition(FVector(Start.X, Start.Y, 1.0f), SMCPool[Total - used]->GetForwardVector(), PillarWallMesh);
	if (overlapSMC) {
		if (overlapSMC->GetStaticMesh() == SMCPool[Total - used]->GetStaticMesh()) {
			SMCPool[Total - used]->SetStaticMesh(nullptr);
		}
	}

	used--;

	// If End.X != Start.X, then create a pillar at End.X, Start.Y
	if (End.X != Start.X) {
		SMCPool[Total - used]->SetRelativeLocation(FVector(End.X, Start.Y, 1.0f));
		SMCPool[Total - used]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		SMCPool[Total - used]->SetStaticMesh(PillarWallMesh);

		overlapSMC = GroundFloor->GetWallObjectMeshAtPosition(FVector(End.X, Start.Y, 1.0f), SMCPool[Total - used]->GetForwardVector(), PillarWallMesh);
		if (overlapSMC) {
			if (overlapSMC->GetStaticMesh() == SMCPool[Total - used]->GetStaticMesh()) {
				SMCPool[Total - used]->SetStaticMesh(nullptr);
			}
		}

		used--;
	}

	// If End.Y != Start.Y, then create a pillar at End.Y, Start.X
	if (Start.Y != End.Y) {
		SMCPool[Total - used]->SetRelativeLocation(FVector(Start.X, End.Y, 1.0f));
		SMCPool[Total - used]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		SMCPool[Total - used]->SetStaticMesh(PillarWallMesh);

		overlapSMC = GroundFloor->GetWallObjectMeshAtPosition(FVector(Start.X, End.Y, 1.0f), SMCPool[Total - used]->GetForwardVector(), PillarWallMesh);
		if (overlapSMC) {
			if (overlapSMC->GetStaticMesh() == SMCPool[Total - used]->GetStaticMesh()) {
				SMCPool[Total - used]->SetStaticMesh(nullptr);
			}
		}

		used--;
	}

	// Finally, if used is now 1 (3 pillars have been placed), then place the final pillar at the End
	if (used == 1) {
		SMCPool[Total - used]->SetRelativeLocation(FVector(End.X, End.Y, 1.0f));
		SMCPool[Total - used]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		SMCPool[Total - used]->SetStaticMesh(PillarWallMesh);

		overlapSMC = GroundFloor->GetWallObjectMeshAtPosition(FVector(End.X, End.Y, 1.0f), SMCPool[Total - used]->GetForwardVector(), PillarWallMesh);
		if (overlapSMC) {
			if (overlapSMC->GetStaticMesh() == SMCPool[Total - used]->GetStaticMesh()) {
				SMCPool[Total - used]->SetStaticMesh(nullptr);
			}
		}

		used--;
	}
	// Else, clear the remaining pillar SMC
	else {
		SMCPool[Total - 2]->SetStaticMesh(nullptr);
		SMCPool[Total - 1]->SetStaticMesh(nullptr);
	}
}
