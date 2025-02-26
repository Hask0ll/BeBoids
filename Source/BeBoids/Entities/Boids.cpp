#include "Boids.h"
#include "Kismet/GameplayStatics.h"

ABoids::ABoids()
{
	PrimaryActorTick.bCanEverTick = true;

	// 1. Configuration du composant de collision
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
    
	// Important: définir le rayon du SphereComponent
	CollisionComponent->SetSphereRadius(100.0f); // Rayon de perception pour les règles des Boids
    
	// Configuration de la collision pour la détection des autres Boids
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // Pas besoin de physique
	CollisionComponent->SetCollisionObjectType(ECC_Pawn); // ou un canal personnalisé
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Détection sans blocage
	CollisionComponent->SetGenerateOverlapEvents(true);

	// 2. Configuration du mesh visuel
	BoidsMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoidsMesh"));
	BoidsMesh->SetupAttachment(CollisionComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube"));
	if (CubeMeshAsset.Succeeded())
	{
		BoidsMesh->SetStaticMesh(CubeMeshAsset.Object);
		BoidsMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
        
		// Configuration du mesh pour les collisions avec l'environnement
		BoidsMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // Sans physique
		BoidsMesh->SetCollisionObjectType(ECC_WorldDynamic);
		BoidsMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // Bloquer les obstacles
	}

	// Pas de simulation physique pour les Boids
	BoidsMesh->SetSimulatePhysics(false);
    
	// Enregistrer les fonctions de détection de collision
	OnActorBeginOverlap.AddDynamic(this, &ABoids::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ABoids::OnEndOverlap);
	
	CollisionComponent->SetCollisionObjectType(ECC_GameTraceChannel1); // Le premier canal personnalisé
	CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
}

void ABoids::BeginPlay()
{
	Super::BeginPlay();
}

void ABoids::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FindNeighbors();

	FVector SteeringForce = CalculateSteeringForces();
	m_Velocity += SteeringForce * DeltaTime;
	m_Velocity = m_Velocity.GetClampedToSize(m_MinSpeed, m_MaxSpeed);
	AddActorWorldOffset(m_Velocity * DeltaTime);
	SetActorRotation(m_Velocity.Rotation());
}

void ABoids::FindNeighbors()
{
	m_Neighbors.Empty();

	TArray<AActor*> AllBoids;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoids::StaticClass(), AllBoids);

	for (AActor* Boid : AllBoids)
	{
		ABoids* BoidRef = Cast<ABoids>(Boid);

		if (BoidRef != this)
		{
			float Distance = FVector::Dist(GetActorLocation(), BoidRef->GetActorLocation());

			if (Distance <= m_PerceptionRadius)
			{
				
				// Par ceci:
				m_Neighbors.Add(BoidRef);
			}
		}
	}
}

FVector ABoids::CalculateSteeringForces()
{
	FVector SeparationForce = CalculateSeparation();
	FVector AlignmentForce = CalculateAlignment();
	FVector CohesionForce = CalculateCohesion();
	FVector AvoidanceForce = CalculateObstacleAvoidance();
	
	return SeparationForce * m_SeparationWeight + AlignmentForce * m_AlignmentWeight + CohesionForce * m_CohesionWeight + AvoidanceForce * m_AvoidanceWeight;
}

FVector ABoids::CalculateSeparation()
{
	FVector SeparationForce = FVector::ZeroVector;

	for (ABoids* Neighbor : m_Neighbors)
	{
		FVector Direction = GetActorLocation() - Neighbor->GetActorLocation();
		float Distance = Direction.Size();

		if (Distance > 0)
		{
			SeparationForce += Direction.GetSafeNormal() / Distance;
		}
	}

	return SeparationForce;
}

FVector ABoids::CalculateAlignment()
{
	FVector AlignmentForce = FVector::ZeroVector;

	for (ABoids* Neighbor : m_Neighbors)
	{
		AlignmentForce += Neighbor->m_Velocity;
	}

	if (m_Neighbors.Num() > 0)
	{
		AlignmentForce /= m_Neighbors.Num();
		AlignmentForce -= m_Velocity;
	}

	return AlignmentForce;
}

FVector ABoids::CalculateCohesion()
{
	FVector CohesionForce = FVector::ZeroVector;

	for (ABoids* Neighbor : m_Neighbors)
	{
		CohesionForce += Neighbor->GetActorLocation();
	}

	if (m_Neighbors.Num() > 0)
	{
		CohesionForce /= m_Neighbors.Num();
		CohesionForce -= GetActorLocation();
	}

	return CohesionForce;
}

FVector ABoids::CalculateObstacleAvoidance()
{
	FVector AvoidanceForce = FVector::ZeroVector;

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * 100.0f;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		AvoidanceForce = HitResult.ImpactNormal;
	}

	return AvoidanceForce;
}

void ABoids::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ABoids* OtherBoid = Cast<ABoids>(OtherActor);
	if (OtherBoid && OtherBoid != this)
	{
		m_Neighbors.AddUnique(OtherBoid);
	}
}

void ABoids::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ABoids* OtherBoid = Cast<ABoids>(OtherActor);
	if (OtherBoid)
	{
		m_Neighbors.Remove(OtherBoid);
	}
}

