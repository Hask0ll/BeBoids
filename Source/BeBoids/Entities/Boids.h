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
	// Constructor
	ABoids();

	// Mesh component for the boid
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Mesh)
	UStaticMeshComponent* BoidsMesh;

	// Collision component for the boid
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Collision)
	USphereComponent* CollisionComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Finds neighboring boids within a certain radius
	UFUNCTION()
	void FindNeighbors();

	// Called when another actor begins to overlap with this actor
	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// Called when another actor ends overlap with this actor
	UFUNCTION()
	void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// Array of neighboring boids
	UPROPERTY()
	TArray<ABoids*> m_Neighbors;

private:
	// Applies separation behavior to the boid
	void ApplySeparation();

	// Applies obstacle avoidance behavior to the boid
	void ApplyObstacleAvoidance();

	// Applies alignment behavior to the boid
	void ApplyAlignment();

	// Applies cohesion behavior to the boid
	void ApplyCohesion();

	// Applies wandering behavior to the boid
	void ApplyWander();

	// Calculates the wander force for the boid
	FVector CalculateWanderForce();

	// Calculates the steering forces for the boid
	FVector CalculateSteeringForces();

	// Calculates the separation force for the boid
	FVector CalculateSeparation();

	// Calculates the alignment force for the boid
	FVector CalculateAlignment();

	// Calculates the cohesion force for the boid
	FVector CalculateCohesion();

	// Calculates the obstacle avoidance force for the boid
	FVector CalculateObstacleAvoidance() const;

	// Current velocity of the boid
	FVector m_Velocity;

	// Maximum speed of the boid
	float m_MaxSpeed = 500.0f;

	// Minimum speed of the boid
	float m_MinSpeed = 200.0f;

	// Perception radius for detecting neighbors
	float m_PerceptionRadius = 500.0f;

	// Weight for alignment behavior
	float m_AlignmentWeight = 1.0f;

	// Weight for cohesion behavior
	float m_CohesionWeight = 1.0f;

	// Weight for separation behavior
	float m_SeparationWeight = 1.0f;

	// Radius for separation behavior
	float m_SeparationRadius = 150.0f;

	// Weight for obstacle avoidance behavior
	float m_AvoidanceWeight = 1.0f;

	// Weight for wandering behavior
	FVector m_WanderWeight;
};