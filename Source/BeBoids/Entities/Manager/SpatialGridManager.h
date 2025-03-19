#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BeBoids/Entities/Boids.h"
#include "SpatialGridManager.generated.h"


UCLASS()
class BEBOIDS_API ASpatialGridManager : public AActor
{
    GENERATED_BODY()

public:
    ASpatialGridManager();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    void RegisterBoid(ABoids* Boid);
    void UnregisterBoid(ABoids* Boid);
    void UpdateBoidPosition(ABoids* Boid, const FVector& OldPosition);
    
    void GetNearbyBoids(ABoids* Boid, TArray<ABoids*>& OutNeighbors, float PerceptionRadius);

    static ASpatialGridManager* GetInstance() { return Instance; }

private:
    // Cell/chunk size for the grid (should be >= the largest perception radius for optimal performance)
    UPROPERTY(EditAnywhere, Category = "Spatial Grid", meta = (AllowPrivateAccess = "true"))
    float CellSize;

    struct FGridPosition
    {
        int X;
        int Y;
        int Z;

        bool operator==(const FGridPosition& Other) const
        {
            return X == Other.X && Y == Other.Y && Z == Other.Z;
        }

        FString ToString() const
        {
            return FString::Printf(TEXT("(%d,%d,%d)"), X, Y, Z);
        }
    };

    FGridPosition GetGridPosition(const FVector& WorldPosition) const;
    FString static GetGridKey(const FGridPosition& GridPos);
    TMap<FString, TArray<ABoids*>> Grid;
    TMap<ABoids*, FGridPosition> BoidPositions;
    static ASpatialGridManager* Instance;
};