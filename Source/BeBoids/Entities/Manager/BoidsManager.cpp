#include "BoidsManager.h"

// Sets default values
ABoidsManager::ABoidsManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ABoidsManager::BeginPlay()
{
    Super::BeginPlay();

    // First, set up the spatial grid if we're using it
    if (bUseSpatialGrid && SpatialGridManagerClass)
    {
        // Spawn the spatial grid manager
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpatialGridManager = GetWorld()->SpawnActor<ASpatialGridManager>(SpatialGridManagerClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
        
        if (!SpatialGridManager)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn SpatialGridManager!"));
            bUseSpatialGrid = false;
        }
    }

    if (!BoidClass)
    {
        UE_LOG(LogTemp, Error, TEXT("BoidClass is not set in BoidsManager."));
        return;
    }

    if (m_SpawnVolume.IsNearlyZero())
    {
        m_SpawnVolume = FVector(500.0f, 500.0f, 200.0f);
        UE_LOG(LogTemp, Warning, TEXT("Spawn volume initialized at value: (500,500,200)."));
    }

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
            
            // Set the use spatial grid flag
            NewBoid->bUseSpatialGrid = bUseSpatialGrid;
            
            // Register with the spatial grid manager if we're using it
            if (bUseSpatialGrid && SpatialGridManager)
            {
                SpatialGridManager->RegisterBoid(NewBoid);
            }
            
            // Initialize with random velocity
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

// Called every frame
void ABoidsManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}