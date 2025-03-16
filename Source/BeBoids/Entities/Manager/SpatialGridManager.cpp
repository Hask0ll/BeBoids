#include "SpatialGridManager.h"

// Initialize the static instance pointer
ASpatialGridManager* ASpatialGridManager::Instance = nullptr;

ASpatialGridManager::ASpatialGridManager()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Default cell size - can be adjusted in the editor
    CellSize = 500.0f;  // Default to match the default perception radius
}

void ASpatialGridManager::BeginPlay()
{
    Super::BeginPlay();
    
    // Set the singleton instance
    Instance = this;
    
    // Clear the grid at the beginning
    Grid.Empty();
    BoidPositions.Empty();
}

void ASpatialGridManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // We don't need any per-tick logic here as boids will update their positions
    // in the grid when they move
}

void ASpatialGridManager::RegisterBoid(ABoids* Boid)
{
    if (!Boid)
        return;

    // Get the grid position for this boid
    FGridPosition GridPos = GetGridPosition(Boid->GetActorLocation());
    
    // Get the key for this grid position
    FString GridKey = GetGridKey(GridPos);
    
    // Add the boid to this grid cell
    if (!Grid.Contains(GridKey))
    {
        Grid.Add(GridKey, TArray<ABoids*>());
    }
    
    Grid[GridKey].Add(Boid);
    
    // Store the boid's current grid position
    BoidPositions.Add(Boid, GridPos);
}

void ASpatialGridManager::UnregisterBoid(ABoids* Boid)
{
    if (!Boid || !BoidPositions.Contains(Boid))
        return;

    // Get the grid position for this boid
    FGridPosition GridPos = BoidPositions[Boid];
    
    // Get the key for this grid position
    FString GridKey = GetGridKey(GridPos);
    
    // Remove the boid from this grid cell
    if (Grid.Contains(GridKey))
    {
        Grid[GridKey].Remove(Boid);
        
        // If the cell is now empty, we can remove it from the grid
        if (Grid[GridKey].Num() == 0)
        {
            Grid.Remove(GridKey);
        }
    }
    
    // Remove the boid from our position tracking
    BoidPositions.Remove(Boid);
}

void ASpatialGridManager::UpdateBoidPosition(ABoids* Boid, const FVector& OldPosition)
{
    if (!Boid || !BoidPositions.Contains(Boid))
        return;

    // Get the old and new grid positions
    FGridPosition OldGridPos = GetGridPosition(OldPosition);
    FGridPosition NewGridPos = GetGridPosition(Boid->GetActorLocation());
    
    // If the boid hasn't changed grid cells, we don't need to do anything
    if (OldGridPos == NewGridPos)
        return;
    
    // Remove from the old cell
    FString OldGridKey = GetGridKey(OldGridPos);
    if (Grid.Contains(OldGridKey))
    {
        Grid[OldGridKey].Remove(Boid);
        
        // If the cell is now empty, we can remove it from the grid
        if (Grid[OldGridKey].Num() == 0)
        {
            Grid.Remove(OldGridKey);
        }
    }
    
    // Add to the new cell
    FString NewGridKey = GetGridKey(NewGridPos);
    if (!Grid.Contains(NewGridKey))
    {
        Grid.Add(NewGridKey, TArray<ABoids*>());
    }
    
    Grid[NewGridKey].Add(Boid);
    
    // Update the boid's stored grid position
    BoidPositions[Boid] = NewGridPos;
}

void ASpatialGridManager::GetNearbyBoids(ABoids* Boid, TArray<ABoids*>& OutNeighbors, float PerceptionRadius)
{
    OutNeighbors.Empty();
    
    if (!Boid || !BoidPositions.Contains(Boid))
        return;

    // Get the current grid position for this boid
    FGridPosition GridPos = BoidPositions[Boid];
    
    // Calculate how many cells we need to check in each direction
    int32 CellRadius = FMath::CeilToInt(PerceptionRadius / CellSize);
    
    // Check all cells within the cell radius
    for (int32 x = -CellRadius; x <= CellRadius; ++x)
    {
        for (int32 y = -CellRadius; y <= CellRadius; ++y)
        {
            for (int32 z = -CellRadius; z <= CellRadius; ++z)
            {
                FGridPosition CheckPos = { GridPos.X + x, GridPos.Y + y, GridPos.Z + z };
                FString CheckKey = GetGridKey(CheckPos);
                
                if (Grid.Contains(CheckKey))
                {
                    for (ABoids* OtherBoid : Grid[CheckKey])
                    {
                        if (OtherBoid != Boid)
                        {
                            float Distance = FVector::Dist(Boid->GetActorLocation(), OtherBoid->GetActorLocation());
                            if (Distance <= PerceptionRadius)
                            {
                                OutNeighbors.Add(OtherBoid);
                            }
                        }
                    }
                }
            }
        }
    }
}

ASpatialGridManager::FGridPosition ASpatialGridManager::GetGridPosition(const FVector& WorldPosition)
{
    FGridPosition GridPos;
    
    // Convert world position to grid position
    GridPos.X = FMath::FloorToInt(WorldPosition.X / CellSize);
    GridPos.Y = FMath::FloorToInt(WorldPosition.Y / CellSize);
    GridPos.Z = FMath::FloorToInt(WorldPosition.Z / CellSize);
    
    return GridPos;
}

FString ASpatialGridManager::GetGridKey(const FGridPosition& GridPos)
{
    return GridPos.ToString();
}