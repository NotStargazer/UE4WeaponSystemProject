// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "IDontUnderstandUE4Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "IDontUnderstandUE4/Enemy.h"
#include "TimerManager.h"

AIDontUnderstandUE4Projectile::AIDontUnderstandUE4Projectile()
{
	/* I had thought this would work. bExplodes is set in the blueprint for each ammo type, so i thought it would be able to check this for individual actors. Guess not.

	*/

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AIDontUnderstandUE4Projectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;

	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 10.0f;

}

void AIDontUnderstandUE4Projectile::BeginPlay()
{
	Super::BeginPlay();
	if (bSticky)
	{
		ProjectileMovement->bShouldBounce = false;
	}
	else
	{
		ProjectileMovement->bShouldBounce = true;
	}

	if (bExplodes)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(ExplosionTimer))
		{
			UE_LOG(LogTemp, Display, TEXT("ExplosionTimerStart"));

			GetWorld()->GetTimerManager().SetTimer(ExplosionTimer, this, &AIDontUnderstandUE4Projectile::ExplosionTimerFunction, 1, false, SecondsToExplode);
		}
	}
}

void AIDontUnderstandUE4Projectile::ExplosionTimerFunction()
{
	GetWorld()->GetTimerManager().ClearTimer(ExplosionTimer);

	OnExplode();
}

void AIDontUnderstandUE4Projectile::OnExplode()
{
	FTransform Transform = this->GetActorTransform();
	FVector Vector = Transform.GetTranslation();
	FRotator Rotation = Transform.Rotator();

	AActor* SpawnedActorRef = GetWorld()->SpawnActor<AActor>(ActorToSpawn, Vector, Rotation);

	Destroy();
}

void AIDontUnderstandUE4Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{

		AEnemy* HitActor = Cast<AEnemy>(OtherActor);

		if (HitActor)
		{
			HitActor->ApplyDamage(Damage);
			if (!bSticky)
			{
				if (!bExplodes)
				{
					Destroy();
				}
				else
				{
					OnExplode();
				}
			}
		}
	}

	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}