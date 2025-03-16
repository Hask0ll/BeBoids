#include "Boids.h"
#include "Kismet/GameplayStatics.h"
#include "BeBoids/Entities/Manager/SpatialGridManager.h"

ABoids::ABoids()
{
    PrimaryActorTick.bCanEverTick = true;

    BoidsMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoidsMesh"));
    BoidsMesh->SetSimulatePhysics(false);
}

void ABoids::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize the previous location
    PreviousLocation = GetActorLocation();
}

void ABoids::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Store the current location before we move
    PreviousLocation = GetActorLocation();

    // Find neighbors using the appropriate method
    FindNeighbors();
    
    // Apply boid behaviors
    ApplySeparation();
    BoidsMovement(DeltaTime);
    
    ApplyAlignment();
    BoidsMovement(DeltaTime);
    
    ApplyCohesion();
    BoidsMovement(DeltaTime);
    
    ApplyObstacleAvoidance();
    BoidsMovement(DeltaTime);
    
    // Update the spatial grid if we've moved and we're using it
    if (bUseSpatialGrid && ASpatialGridManager::GetInstance())
    {
        ASpatialGridManager::GetInstance()->UpdateBoidPosition(this, PreviousLocation);
    }
}

void ABoids::BoidsMovement(float Deltatime)
{
    m_Velocity = m_Velocity.GetClampedToSize(m_MinSpeed, m_MaxSpeed);
    AddActorWorldOffset(m_Velocity * Deltatime);
    SetActorRotation(m_Velocity.Rotation());
}

void ABoids::ApplyObstacleAvoidance()
{
    FVector Direction = m_Velocity.GetSafeNormal();
    MaxDistance = 200.0f;
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
    
    for (ABoids* Neighbor : Neighbors)
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
    
    if (Neighbors.Num() > 0)
    {
        FVector AverageDirection = FVector::ZeroVector;
        for (ABoids* Neighbor : Neighbors)
        {
            AverageDirection += Neighbor->m_Velocity.GetSafeNormal();
        }
        AverageDirection /= Neighbors.Num();
        
        Direction += AverageDirection * m_AlignmentWeight;
        Direction.Normalize();
        
        float CurrentSpeed = m_Velocity.Size();
        m_Velocity = Direction * CurrentSpeed;
    }
}

void ABoids::ApplyCohesion()
{
    if (Neighbors.Num() == 0)
        return;
        
    FVector Centroid = FVector::ZeroVector;
    
    for (AActor* Neighbor : Neighbors)
    {
        if (Neighbor)
        {
            Centroid += Neighbor->GetActorLocation();
        }
    }
    
    Centroid /= Neighbors.Num();
    
    FVector ToCentroid = Centroid - GetActorLocation();
    
    if (!ToCentroid.IsNearlyZero())
    {
        ToCentroid.Normalize();
        ToCentroid *= m_CohesionWeight;
    }
    
    m_Velocity += ToCentroid;
}

void ABoids::FindNeighbors()
{
    // Clear the current neighbors
    Neighbors.Empty();

    // Use the spatial grid if available, otherwise fall back to the brute force method
    if (bUseSpatialGrid && ASpatialGridManager::GetInstance())
    {
        // Get nearby boids using the spatial grid
        ASpatialGridManager::GetInstance()->GetNearbyBoids(this, Neighbors, m_PerceptionRadius);
    }
    else
    {
        // Original brute force method
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
                    Neighbors.Add(BoidRef);
                }
            }
        }
    }
}