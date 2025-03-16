#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BeBoids/Entities/Boids.h"
#include "SpatialGridManager.generated.h"

/**
 * A spatial grid manager for boids simulation optimization.
 * Divides the world into a grid of chunks and manages which boids are in which chunks.
 * This allows for efficient neighbor queries by only checking boids in nearby chunks.
 */
UCLASS()
class BEBOIDS_API ASpatialGridManager : public AActor
{
    GENERATED_BODY()

public:
    ASpatialGridManager();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    // Register a boid with the spatial grid
    void RegisterBoid(ABoids* Boid);
    
    // Unregister a boid from the spatial grid
    void UnregisterBoid(ABoids* Boid);
    
    // Update a boid's position in the grid
    void UpdateBoidPosition(ABoids* Boid, const FVector& OldPosition);
    
    // Find nearby boids within perception radius
    void GetNearbyBoids(ABoids* Boid, TArray<ABoids*>& OutNeighbors, float PerceptionRadius);

    // Get the singleton instance
    static ASpatialGridManager* GetInstance() { return Instance; }

private:
    // Cell/chunk size for the grid (should be >= the largest perception radius for optimal performance)
    UPROPERTY(EditAnywhere, Category = "Spatial Grid", meta = (AllowPrivateAccess = "true"))
    float CellSize;

    // Structure to hold a 3D grid position
    struct FGridPosition
    {
        int32 X;
        int32 Y;
        int32 Z;

        bool operator==(const FGridPosition& Other) const
        {
            return X == Other.X && Y == Other.Y && Z == Other.Z;
        }

        FString ToString() const
        {
            return FString::Printf(TEXT("(%d,%d,%d)"), X, Y, Z);
        }
    };

    // Get the grid cell index for a world position
    FGridPosition GetGridPosition(const FVector& WorldPosition);

    // Retrieve a unique string key for a grid position
    FString GetGridKey(const FGridPosition& GridPos);

    // Map of grid cells to boids contained within them
    TMap<FString, TArray<ABoids*>> Grid;

    // Map of boids to their current grid positions
    TMap<ABoids*, FGridPosition> BoidPositions;

    // Static instance for singleton pattern
    static ASpatialGridManager* Instance;
};