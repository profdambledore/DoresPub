// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/Player_BuildToolDisplay.h"

#include "Kismet/GameplayStatics.h"

#include "World/World_BuildingLevel.h"

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

// Called to update the display's validity
void APlayer_BuildToolDisplay::UpdateDisplayValidity(bool bIsValid)
{
	// If the tool display is vailid,
	if (bIsValid) {
		// Set the material instance to use the valid colour
		UpdateMaterial(ValidColour);
	}
	// Else, the tool is invalid, so
	else {
		// Set the material instance to use the invalid colour
		UpdateMaterial(InvalidColour);
	}
}

// Called to return how many walls are currently being used
// Return an int of how many SMC's match the correct criteria
int APlayer_BuildToolDisplay::GetDisplayWallsInUse()
{
	// Init local variable
	int amn = 0;

	// Go through the array checking if...
	for (int i = 0; i < Total; i++) {
		// Either the current SMC has the selected mesh and that the selected mesh is valid (not nullptr), OR
		// That there is a mesh set if erase mode is enabled
		if (SMCPool[i]->GetStaticMesh() == SelectedMesh && SelectedMesh || SMCPool[i]->GetStaticMesh() != nullptr && bInEraseMode) {
			// If so, add one to the total count
			amn += 1;
		}
	}
	// Return the total count
	return amn;
}

// Called to return the wall data
// Returns a TArray of FBuildToolData structs for each valid SMC
TArray<FBuildToolData> APlayer_BuildToolDisplay::GetDisplayData()
{
	// Init local variables
	TArray<FBuildToolData> out; FBuildToolData curr;

	// For each SMC in the StaticMeshComponent pool...
	for (UStaticMeshComponent* i : SMCPool) {
		// Check if the mesh is valid.  If so...
		if (i->GetStaticMesh() != nullptr) {
			// Set the curr FBuildToolData to the correct properties of the current SMC (Mesh, Location and Rotation)
			curr.Mesh = i->GetStaticMesh();
			curr.Location = i->GetComponentLocation();
			curr.Rotation = i->GetComponentRotation();
			curr.ID = SelectedWallID;

			// And add it to the total array
			out.Add(curr);
		}
	}

	// Return the total array
	return out;
}

// Called to generate a new building display, based on a start and end location
void APlayer_BuildToolDisplay::GenerateNewBuildDisplay(FVector StartPosition, FVector EndPosition)
{
	// Check that there is a mesh selected, or we are in erase mode.  If so,
	if (SelectedMesh || bInEraseMode) {
		// Move the BuildToolDisplay to the StartPosition
		SetActorLocation(StartPosition);

		// Start by calculating how many static mesh components are needed
		int XRequires = 0;	int YRequires = 0;

		// Calculate how many are needed for a single x edge (will double for a full box)
		// By calculating the difference between the start and end position, making the output absolute (always pos)
		// Then dividing that by the wall length (125 uu) and rounding up with ceil
		XRequires = ceil(fabs(EndPosition.X - StartPosition.X) / WallSize);

		// Do the same to the Y axis
		YRequires = ceil(fabs(EndPosition.Y - StartPosition.Y) / WallSize);

		// Then calculate the total needed + 4 for the pillar corner blocks
		// If both X and Y require no walls, then don't add any to the required total
		Total = (XRequires * ((YRequires != 0) ? 2 : 1)) + (YRequires * ((XRequires != 0) ? 2 : 1));
		//Total = ((Total != 0) ? Total + 0 : 0);

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
			//CreatePillars(StartPosition, EndPosition);
		}

		// Finally, clear the excees SMC
		for (int j = Total; j < SMCPool.Num(); j++) {
			SMCPool[j]->SetStaticMesh(nullptr);
		}
	}
}


// Called to clear the building display
void APlayer_BuildToolDisplay::ClearBuildDisplay()
{
	// Simply clear each spawned SMC in the SMCPool
	for (UStaticMeshComponent* i : SMCPool) {
		i->SetStaticMesh(nullptr);
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
		SMCPool[StartWallSegment + i]->SetStaticMesh(SelectedMesh);

		// If in default mode, check if there is a mesh already at the position
		if (!bInEraseMode) {
			if (GroundFloor->GetWallObjectMeshAtPosition(FVector(WallStart + (WallSize * i), Y, 1.0f), true)) {
				// If there is, clear the mesh
				SMCPool[StartWallSegment + i]->SetStaticMesh(nullptr);
			}
		}
		// Else, check if there isn't a mesh at the position
		else {
			if (!GroundFloor->GetWallObjectMeshAtPosition(FVector(WallStart + (WallSize * i), Y, 1.0f), true)) {
				// If there isn't, clear the mesh
				SMCPool[StartWallSegment + i]->SetStaticMesh(nullptr);
			}
			// Else, match the meshes
			else {
				SMCPool[StartWallSegment + i]->SetStaticMesh(GroundFloor->GetWallObjectMeshAtPosition(FVector(WallStart + (WallSize * i), Y, 1.0f), true)->GetStaticMesh());
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
		SMCPool[StartWallSegment + i]->SetStaticMesh(SelectedMesh);

		// If in default mode, check if there is a mesh already at the position
		if (!bInEraseMode) {
			if (GroundFloor->GetWallObjectMeshAtPosition(FVector(X, WallStart + (WallSize * i), 1.0f), false)) {
				SMCPool[StartWallSegment + i]->SetStaticMesh(nullptr);
			}
		}
		// Else, check if there isn't a mesh at the position
		else {
			if (!GroundFloor->GetWallObjectMeshAtPosition(FVector(X, WallStart + (WallSize * i), 1.0f), false)) {
				SMCPool[StartWallSegment + i]->SetStaticMesh(nullptr);
			}
			// Else, match the meshes
			else {
				SMCPool[StartWallSegment + i]->SetStaticMesh(GroundFloor->GetWallObjectMeshAtPosition(FVector(X, WallStart + (WallSize * i), 1.0f), false)->GetStaticMesh());
			}
		}

		
	}
}

void APlayer_BuildToolDisplay::CreatePillars(FVector Start, FVector End)
{
	int used = 4;

	//UStaticMeshComponent* overlapSMC;

	// Create a pillar at the start point
	SMCPool[Total - used]->SetRelativeLocation(FVector(Start.X, Start.Y, 1.0f));
	SMCPool[Total - used]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	//SMCPool[Total - used]->SetStaticMesh(PillarWallMesh);

	//overlapSMC = GroundFloor->GetWallObjectMeshAtPosition(FVector(Start.X, Start.Y, 1.0f), SMCPool[Total - used]->GetForwardVector(), PillarWallMesh);
	//if (overlapSMC) {
		//if (overlapSMC->GetStaticMesh() == SMCPool[Total - used]->GetStaticMesh()) {
		//	SMCPool[Total - used]->SetStaticMesh(nullptr);
		//}
	//}

	//used--;

	// If End.X != Start.X, then create a pillar at End.X, Start.Y
	if (End.X != Start.X) {
		SMCPool[Total - used]->SetRelativeLocation(FVector(End.X, Start.Y, 1.0f));
		SMCPool[Total - used]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		//SMCPool[Total - used]->SetStaticMesh(PillarWallMesh);

		//overlapSMC = GroundFloor->GetWallObjectMeshAtPosition(FVector(End.X, Start.Y, 1.0f), SMCPool[Total - used]->GetForwardVector(), PillarWallMesh);
		//if (overlapSMC) {
			//if (overlapSMC->GetStaticMesh() == SMCPool[Total - used]->GetStaticMesh()) {
			//	SMCPool[Total - used]->SetStaticMesh(nullptr);
			//}
		//}

		//used--;
	}

	// If End.Y != Start.Y, then create a pillar at End.Y, Start.X
	if (Start.Y != End.Y) {
		SMCPool[Total - used]->SetRelativeLocation(FVector(Start.X, End.Y, 1.0f));
		SMCPool[Total - used]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		//SMCPool[Total - used]->SetStaticMesh(PillarWallMesh);

		//overlapSMC = GroundFloor->GetWallObjectMeshAtPosition(FVector(Start.X, End.Y, 1.0f), SMCPool[Total - used]->GetForwardVector(), PillarWallMesh);
		//if (overlapSMC) {
			//if (overlapSMC->GetStaticMesh() == SMCPool[Total - used]->GetStaticMesh()) {
				//SMCPool[Total - used]->SetStaticMesh(nullptr);
			//}
		//}

		//used--;
	}

	// Finally, if used is now 1 (3 pillars have been placed), then place the final pillar at the End
	if (used == 1) {
		SMCPool[Total - used]->SetRelativeLocation(FVector(End.X, End.Y, 1.0f));
		SMCPool[Total - used]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		//SMCPool[Total - used]->SetStaticMesh(PillarWallMesh);

		//overlapSMC = GroundFloor->GetWallObjectMeshAtPosition(FVector(End.X, End.Y, 1.0f), SMCPool[Total - used]->GetForwardVector(), PillarWallMesh);
		//if (overlapSMC) {
			//if (overlapSMC->GetStaticMesh() == SMCPool[Total - used]->GetStaticMesh()) {
			//	SMCPool[Total - used]->SetStaticMesh(nullptr);
			//}
		//}

		used--;
	}
	// Else, clear the remaining pillar SMC
	else {
		//SMCPool[Total - 2]->SetStaticMesh(nullptr);
		//SMCPool[Total - 1]->SetStaticMesh(nullptr);
	}
}

/// -- Utility Functions --
	// Called to add a new StaticMeshComponent to the BuildToolDisplay
void APlayer_BuildToolDisplay::AddNewStaticMeshComponent(int Target)
{
	// Init local variable
	int NewNumber = SMCPool.Num() + 1;

	// Create a new StaticMeshComponent with the new number, register it so it can be used and set the mesh's materials and collision
	UStaticMeshComponent* NewMeshComp = NewObject<UStaticMeshComponent>(this, FName(*FString::Printf(TEXT("Wall Mesh %i"), NewNumber)));
	NewMeshComp->RegisterComponent();
	NewMeshComp->SetMaterial(0, BuildToolMaterial);
	NewMeshComp->SetMaterial(1, BuildToolMaterial);
	NewMeshComp->SetCollisionProfileName(FName("BuildingTool"));

	// Then add it to the total pool (SMCPool)
	SMCPool.Add(NewMeshComp);

	// Check if there is now enough component sets added.  If not, recurse
	if (SMCPool.Num() < Target) {
		AddNewStaticMeshComponent(Target);
	}
}