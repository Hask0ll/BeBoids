// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidsManager.h"


// Sets default values
ABoidsManager::ABoidsManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABoidsManager::BeginPlay()
{
	Super::BeginPlay();

	if (!BoidClass)
	{
		UE_LOG(LogTemp, Error, TEXT("BoidClass is not set in BoidsManager."));
		return;
	}

	if (m_SpawnVolume.IsNearlyZero())
	{
		m_SpawnVolume = FVector(500.0f, 500.0f, 200.0f);
		UE_LOG(LogTemp, Warning, TEXT("Spawn volume initialyse at value : (500,500,200)."));
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
            
			// Initialisation supplémentaire si besoin
			// NewBoid->SetInitialVelocity(...);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Spawn Boids error at location: %s"), *Position.ToString());
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Spawned %d Boids on %d Given"), SpawnedBoids.Num(), m_NumBoids);
}

// Called every frame
void ABoidsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

