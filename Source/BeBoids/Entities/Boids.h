#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Boids.generated.h"

UCLASS()
class BEBOIDS_API ABoids : public AActor
{
	GENERATED_BODY()

public:
	ABoids();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Mesh)
	UStaticMeshComponent* BoidsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Collision)
	USphereComponent* CollisionComponent;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void FindNeighbors();

	UFUNCTION()
	FVector CalculateSteeringForces();

	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);
	
	UPROPERTY()
	TArray<ABoids*> m_Neighbors;

private:

	FVector CalculateSeparation();
	FVector CalculateAlignment();
	FVector CalculateCohesion();
	FVector CalculateObstacleAvoidance();
	
	
	FVector m_Velocity;
	float m_MaxSpeed = 500.0f;
	float m_MinSpeed = 200.0f;
	float m_PerceptionRadius = 500.0f;
	float m_AlignmentWeight = 1.0f;
	float m_CohesionWeight = 1.0f;
	float m_SeparationWeight = 1.5f;
	float m_SeparationRadius = 150.0f;
	float m_AvoidanceWeight = 1.0f;

};