#include "Boids.h"
#include "Kismet/GameplayStatics.h"

ABoids::ABoids()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
    
	CollisionComponent->SetSphereRadius(100.0f);
    
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_Pawn);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);

	BoidsMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoidsMesh"));
	BoidsMesh->SetupAttachment(CollisionComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube"));
	if (CubeMeshAsset.Succeeded())
	{
		BoidsMesh->SetStaticMesh(CubeMeshAsset.Object);
		BoidsMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
        
		BoidsMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		BoidsMesh->SetCollisionObjectType(ECC_WorldDynamic);
		BoidsMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	}

	BoidsMesh->SetSimulatePhysics(false);
    
	OnActorBeginOverlap.AddDynamic(this, &ABoids::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ABoids::OnEndOverlap);
	
	CollisionComponent->SetCollisionObjectType(ECC_GameTraceChannel1);
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
	ApplySeparation();
	ApplyObstacleAvoidance();
	ApplyAlignment();
	//ApplyCohesion();
	//ApplyWander();
    
	m_Velocity = m_Velocity.GetClampedToSize(m_MinSpeed, m_MaxSpeed);
	AddActorWorldOffset(m_Velocity * DeltaTime);
    
	if (!m_Velocity.IsNearlyZero())
	{
		SetActorRotation(m_Velocity.Rotation());
	}

	FVector SteeringForce = CalculateSteeringForces();
	m_Velocity += SteeringForce * DeltaTime;
	m_Velocity = m_Velocity.GetClampedToSize(m_MinSpeed, m_MaxSpeed);
	AddActorWorldOffset(m_Velocity * DeltaTime);
	SetActorRotation(m_Velocity.Rotation());
}

void ABoids::ApplyObstacleAvoidance()
{
    FVector Direction = m_Velocity.GetSafeNormal();
    float MaxDistance = 200.0f;
    bool ObstacleDetected = false;
    
    TArray<FVector> RayDirections;
    RayDirections.Add(GetActorForwardVector());
    
    FRotator SlightLeftRot(0, -15, 0);
    FRotator SlightRightRot(0, 15, 0);
    FRotator MoreLeftRot(0, -30, 0);
    FRotator MoreRightRot(0, 30, 0);
    
    RayDirections.Add(SlightLeftRot.RotateVector(GetActorForwardVector()));
    RayDirections.Add(SlightRightRot.RotateVector(GetActorForwardVector()));
    RayDirections.Add(MoreLeftRot.RotateVector(GetActorForwardVector()));
    RayDirections.Add(MoreRightRot.RotateVector(GetActorForwardVector()));
    
    for (const FVector& RayDir : RayDirections)
    {
        FHitResult HitResult;
        FCollisionQueryParams CollisionParams;
        CollisionParams.AddIgnoredActor(this);
        
        FVector Start = GetActorLocation();
        FVector End = Start + RayDir * MaxDistance;
        
        if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
        {
            FVector AvoidanceVector = Start - HitResult.ImpactPoint;
            float Distance = AvoidanceVector.Size();
            
            float Ratio = 1.0f - (Distance / MaxDistance);
            
            Direction += AvoidanceVector.GetSafeNormal() * Ratio * m_AvoidanceWeight;
            ObstacleDetected = true;
            
            /*if (GetWorld()->IsPlayInEditor())
            {
                DrawDebugLine(GetWorld(), Start, HitResult.ImpactPoint, FColor::Red, false, -1.0f, 0, 2.0f);
            }*/
        }
    }
    
    if (ObstacleDetected && !Direction.IsNearlyZero())
    {
        Direction.Normalize();
        
        float CurrentSpeed = m_Velocity.Size();
        m_Velocity = Direction * CurrentSpeed;
    }
}

void ABoids::ApplySeparation()
{
	FVector Direction = m_Velocity.GetSafeNormal();
	float MaxDistance = 100.0f;
    
	for (ABoids* Neighbor : m_Neighbors)
	{
		FVector SeparationVector = GetActorLocation() - Neighbor->GetActorLocation();
		float Distance = SeparationVector.Size();
        
		if (Distance > 0.0f && Distance < MaxDistance)
		{
			float Ratio = Distance / MaxDistance;
			Direction += SeparationVector * Ratio * m_SeparationWeight;
		}
	}
    
	if (!Direction.IsNearlyZero())
	{
		Direction.Normalize();
	}
    
	float CurrentSpeed = m_Velocity.Size();
	m_Velocity = Direction * CurrentSpeed;
}

void ABoids::ApplyAlignment()
{
	FVector Direction = m_Velocity.GetSafeNormal();
    
	if (m_Neighbors.Num() > 0)
	{
		FVector AverageDirection = FVector::ZeroVector;
		for (ABoids* Neighbor : m_Neighbors)
		{
			AverageDirection += Neighbor->m_Velocity.GetSafeNormal();
		}
		AverageDirection /= m_Neighbors.Num();
        
		Direction += AverageDirection * m_AlignmentWeight;
		Direction.Normalize();
        
		float CurrentSpeed = m_Velocity.Size();
		m_Velocity = Direction * CurrentSpeed;
	}
}

void ABoids::ApplyCohesion()
{
    if (m_Neighbors.Num() == 0)
        return;
    
    FVector Direction = m_Velocity.GetSafeNormal();
    
    FVector CenterOfMass = FVector::ZeroVector;
    for (ABoids* Neighbor : m_Neighbors)
    {
        CenterOfMass += Neighbor->GetActorLocation();
    }
    CenterOfMass /= m_Neighbors.Num();
    
    FVector ToCenterVector = CenterOfMass - GetActorLocation();
    float Distance = ToCenterVector.Size();
    
    float MaxDistance = 300.0f;
    
    if (Distance > 0.0f && Distance < MaxDistance)
    {
        float Ratio = Distance / MaxDistance;
        
        Direction += ToCenterVector.GetSafeNormal() * Ratio * m_CohesionWeight;
        
        if (!Direction.IsNearlyZero())
        {
            Direction.Normalize();
        }
        
        float CurrentSpeed = m_Velocity.Size();
        m_Velocity = Direction * CurrentSpeed;
    }
    
    /*if (GetWorld()->IsPlayInEditor())
    {
        DrawDebugLine(GetWorld(), GetActorLocation(), CenterOfMass, 
                      FColor::Blue, false, -1.0f, 0, 1.0f);
        DrawDebugSphere(GetWorld(), CenterOfMass, 10.0f, 8, 
                      FColor::Blue, false, -1.0f, 0, 1.0f);
    }*/
}

void ABoids::ApplyWander()
{
    FVector Direction = m_Velocity.GetSafeNormal();
    
    float WanderStrength = 0.1f;
    float WanderRate = 0.3f;
    
    if (FMath::FRand() < WanderRate)
    {
        float RandomAngleYaw = FMath::RandRange(-20.0f, 20.0f);
        float RandomAnglePitch = FMath::RandRange(-10.0f, 10.0f);
        
        FRotator RandomRotation(RandomAnglePitch, RandomAngleYaw, 0.0f);
        
        FVector WanderDirection = RandomRotation.RotateVector(Direction);
        
        Direction += WanderDirection * WanderStrength * m_WanderWeight;
        
        if (!Direction.IsNearlyZero())
        {
            Direction.Normalize();
        }
        
        float CurrentSpeed = m_Velocity.Size();
        m_Velocity = Direction * CurrentSpeed;
        
        /*if (GetWorld()->IsPlayInEditor())
        {
            DrawDebugLine(GetWorld(), GetActorLocation(), 
                          GetActorLocation() + WanderDirection * 50.0f, 
                          FColor::Yellow, false, 0.2f, 0, 1.0f);
        }*/
    }
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
	FVector WanderForce = CalculateWanderForce();
    
	return SeparationForce * m_SeparationWeight + 
		   AlignmentForce * m_AlignmentWeight + 
		   CohesionForce * m_CohesionWeight + 
		   AvoidanceForce * m_AvoidanceWeight + WanderForce * m_WanderWeight;
}

FVector ABoids::CalculateSeparation()
{
	FVector SeparationDirection = FVector::ZeroVector;
	float MaxDistance = m_PerceptionRadius;
    
	if (m_Neighbors.Num() == 0)
		return SeparationDirection;
    
	for (ABoids* Neighbor : m_Neighbors)
	{
		FVector DifferenceVector = GetActorLocation() - Neighbor->GetActorLocation();
		float Distance = DifferenceVector.Size();
        
		if (Distance > 0.0f && Distance < MaxDistance)
		{
			float Ratio = Distance / MaxDistance;
			SeparationDirection += DifferenceVector * Ratio;
		}
	}
    
	if (!SeparationDirection.IsNearlyZero())
	{
		SeparationDirection.Normalize();
	}
    
	return SeparationDirection;
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

FVector ABoids::CalculateObstacleAvoidance() const
{
    FVector AvoidanceDirection = FVector::ZeroVector;
    float MaxDistance = 200.0f;
    
    TArray<FVector> RayDirections;
    
    RayDirections.Add(GetActorForwardVector());
    
    FRotator LeftRot(0, -30, 0);
    FRotator RightRot(0, 30, 0);
    RayDirections.Add(LeftRot.RotateVector(GetActorForwardVector()));
    RayDirections.Add(RightRot.RotateVector(GetActorForwardVector()));
    
    for (const FVector& RayDir : RayDirections)
    {
        FHitResult HitResult;
        FCollisionQueryParams CollisionParams;
        CollisionParams.AddIgnoredActor(this);
        
        FVector Start = GetActorLocation();
        FVector End = Start + RayDir * MaxDistance;
        
        bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
        
        if (bHit)
        {
            FVector DifferenceVector = Start - HitResult.ImpactPoint;
            float Distance = DifferenceVector.Size();
            
            float Ratio = 1.0f - (Distance / MaxDistance);
            
            AvoidanceDirection += DifferenceVector.GetSafeNormal() * Ratio * Ratio;
            
            /*if (GetWorld()->IsPlayInEditor())
            {
                DrawDebugLine(GetWorld(), Start, HitResult.ImpactPoint, FColor::Red, false, -1.0f, 0, 2.0f);
                DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 8, FColor::Orange, false, -1.0f, 0, 1.0f);
            }*/
        }
    }
    
    if (!AvoidanceDirection.IsNearlyZero())
    {
        AvoidanceDirection.Normalize();
    }
    
    return AvoidanceDirection;
}

FVector ABoids::CalculateWanderForce()
{
	float RandomAngle = FMath::RandRange(-30.0f, 30.0f);
	FRotator Rotation = FRotator(0, RandomAngle, 0);
	FVector WanderDirection = Rotation.RotateVector(m_Velocity.GetSafeNormal());
    
	return WanderDirection * 0.1f;
}

void ABoids::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ABoids* OtherBoids = Cast<ABoids>(OtherActor);
	if (OtherBoids && OtherBoids != this)
	{
		m_Neighbors.AddUnique(OtherBoids);
	}
}

void ABoids::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (ABoids* OtherBoids = Cast<ABoids>(OtherActor))
	{
		m_Neighbors.Remove(OtherBoids);
	}
}