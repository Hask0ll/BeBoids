#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Boids.generated.h"

/**
 * ABoids class represents a boid entity in the simulation.
 * It inherits from AActor and contains components and methods
 * to handle boid behaviors such as separation, alignment, cohesion,
 * obstacle avoidance, and wandering.
 */
UCLASS()
class BEBOIDS_API ABoids : public AActor
{
	GENERATED_BODY()

public:
	ABoids();

	// Mesh component for the boid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Mesh)
	UStaticMeshComponent* BoidsMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void FindNeighbors();

	UPROPERTY()
	TArray<ABoids*> Neighbors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids", meta = (ExposeOnSpawn="true"))
	float m_PerceptionRadius = 400.0f;
    
	void ApplySeparation();
	void ApplyObstacleAvoidance();
	void ApplyAlignment();
	void ApplyCohesion();
	void BoidsMovement(float Deltatime);
    
	FVector m_Velocity;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids", meta = (ExposeOnSpawn="true"))
	float MaxDistance = 100.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids", meta = (ExposeOnSpawn="true"))
	float m_MaxSpeed = 300.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids")
	float m_MinSpeed = 200.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids")
	float m_AlignmentWeight = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids")
	float m_CohesionWeight = 20.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids")
	float m_SeparationWeight = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids")
	float m_AvoidanceWeight = 5.0f;
    
	UPROPERTY(BlueprintReadWrite, Category = "Boids")
	bool bUseSpatialGrid = true;
    
	FVector PreviousLocation;
};