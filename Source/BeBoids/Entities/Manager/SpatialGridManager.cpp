#include "SpatialGridManager.h"

ASpatialGridManager* ASpatialGridManager::Instance = nullptr;

ASpatialGridManager::ASpatialGridManager()
{
    PrimaryActorTick.bCanEverTick = true;
    
    CellSize = 1000.0f;
}

void ASpatialGridManager::BeginPlay()
{
    Super::BeginPlay();
    
    Instance = this;
    
    Grid.Empty();
    BoidPositions.Empty();
}

void ASpatialGridManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
}

void ASpatialGridManager::RegisterBoid(ABoids* Boid)
{
    if (!Boid)
        return;

    FGridPosition GridPos = GetGridPosition(Boid->GetActorLocation());
    FString GridKey = GetGridKey(GridPos);
    
    if (!Grid.Contains(GridKey))
    {
        Grid.Add(GridKey, TArray<ABoids*>());
    }
    
    Grid[GridKey].Add(Boid);
    BoidPositions.Add(Boid, GridPos);
}

void ASpatialGridManager::UnregisterBoid(ABoids* Boid)
{
    if (!Boid || !BoidPositions.Contains(Boid))
        return;

    FGridPosition GridPos = BoidPositions[Boid];
    FString GridKey = GetGridKey(GridPos);
    
    if (Grid.Contains(GridKey))
    {
        Grid[GridKey].Remove(Boid);
        
        if (Grid[GridKey].Num() == 0)
        {
            Grid.Remove(GridKey);
        }
    }
    
    BoidPositions.Remove(Boid);
}

void ASpatialGridManager::UpdateBoidPosition(ABoids* Boid, const FVector& OldPosition)
{
    if (!Boid || !BoidPositions.Contains(Boid))
        return;

    FGridPosition OldGridPos = GetGridPosition(OldPosition);
    FGridPosition NewGridPos = GetGridPosition(Boid->GetActorLocation());
    
    if (OldGridPos == NewGridPos)
        return;
    
    FString OldGridKey = GetGridKey(OldGridPos);
    if (Grid.Contains(OldGridKey))
    {
        Grid[OldGridKey].Remove(Boid);
        
        if (Grid[OldGridKey].Num() == 0)
        {
            Grid.Remove(OldGridKey);
        }
    }
    
    FString NewGridKey = GetGridKey(NewGridPos);
    if (!Grid.Contains(NewGridKey))
    {
        Grid.Add(NewGridKey, TArray<ABoids*>());
    }
    
    Grid[NewGridKey].Add(Boid);
    BoidPositions[Boid] = NewGridPos;
}

void ASpatialGridManager::GetNearbyBoids(ABoids* Boid, TArray<ABoids*>& OutNeighbors, float PerceptionRadius)
{
    OutNeighbors.Empty();
    
    if (!Boid || !BoidPositions.Contains(Boid))
        return;

    FGridPosition GridPos = BoidPositions[Boid];
    int32 CellRadius = FMath::CeilToInt(PerceptionRadius / CellSize);
    
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

ASpatialGridManager::FGridPosition ASpatialGridManager::GetGridPosition(const FVector& WorldPosition) const
{
    FGridPosition GridPos;
    
    GridPos.X = FMath::FloorToInt(WorldPosition.X / CellSize);
    GridPos.Y = FMath::FloorToInt(WorldPosition.Y / CellSize);
    GridPos.Z = FMath::FloorToInt(WorldPosition.Z / CellSize);
    
    return GridPos;
}

FString ASpatialGridManager::GetGridKey(const FGridPosition& GridPos)
{
    return GridPos.ToString();
}