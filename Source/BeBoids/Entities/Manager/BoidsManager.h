#pragma once

#include "CoreMinimal.h"
#include "BeBoids/Entities/Boids.h"
#include "GameFramework/Actor.h"
#include "SpatialGridManager.h"
#include "BoidsManager.generated.h"

UCLASS()
class BEBOIDS_API ABoidsManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABoidsManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids")
	TArray<ABoids*> SpawnedBoids;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids")
	int m_NumBoids = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids")
	FVector m_SpawnVolume;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids")
	TSubclassOf<ABoids> BoidClass;
    
	// Reference to the spatial grid manager
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids")
	TSubclassOf<ASpatialGridManager> SpatialGridManagerClass;
    
	UPROPERTY()
	ASpatialGridManager* SpatialGridManager;
    
	// Whether to use the spatial grid optimization
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids")
	bool bUseSpatialGrid = true;
};