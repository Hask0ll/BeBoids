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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids", meta = (ExposeOnSpawn="true"))
	int m_NumBoids = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids", meta = (ExposeOnSpawn="true"))
	FVector m_SpawnVolume;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids", meta = (ExposeOnSpawn="true"))
	TSubclassOf<ABoids> BoidClass;
    
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids")
	TSubclassOf<ASpatialGridManager> SpatialGridManagerClass;
    
	UPROPERTY()
	ASpatialGridManager* SpatialGridManager;
    
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boids")
	bool bUseSpatialGrid = true;

private:

	void SpawnBoids();
	void SpawnSpatialGrid();
};