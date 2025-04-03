#include "BoidsManager.h"

// Sets default values
ABoidsManager::ABoidsManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ABoidsManager::BeginPlay()
{
    Super::BeginPlay();

    SpawnSpatialGrid();
    SpawnBoids();
}

void ABoidsManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABoidsManager::SpawnSpatialGrid()
{
    if (bUseSpatialGrid && SpatialGridManagerClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpatialGridManager = GetWorld()->SpawnActor<ASpatialGridManager>(SpatialGridManagerClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
        
        if (!SpatialGridManager)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn SpatialGridManager!"));
            bUseSpatialGrid = false;
        }
    }
}

void ABoidsManager::SpawnBoids()
{
    for (int i = 0; i < m_NumBoids; i++)
    {
        FVector Position = GetActorLocation() + FVector(
            FMath::RandRange(-m_SpawnVolume.X, m_SpawnVolume.X),
            FMath::RandRange(-m_SpawnVolume.Y, m_SpawnVolume.Y),
            FMath::RandRange(-m_SpawnVolume.Z, m_SpawnVolume.Z)
        );

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        ABoids* NewBoid = GetWorld()->SpawnActor<ABoids>(BoidClass, Position, FRotator::ZeroRotator, SpawnParams);
        
        if (NewBoid)
        {
            SpawnedBoids.Add(NewBoid);
            
            NewBoid->bUseSpatialGrid = bUseSpatialGrid;
            
            if (bUseSpatialGrid && SpatialGridManager)
            {
                SpatialGridManager->RegisterBoid(NewBoid);
            }
            
            FVector RandomDir = FVector(
                FMath::RandRange(-1.0f, 1.0f),
                FMath::RandRange(-1.0f, 1.0f),
                FMath::RandRange(-1.0f, 1.0f)
            ).GetSafeNormal();
            
            NewBoid->m_Velocity = RandomDir * FMath::RandRange(NewBoid->m_MinSpeed, NewBoid->m_MaxSpeed);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Spawn Boids error at location: %s"), *Position.ToString());
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Spawned %d Boids out of %d requested"), SpawnedBoids.Num(), m_NumBoids);
}