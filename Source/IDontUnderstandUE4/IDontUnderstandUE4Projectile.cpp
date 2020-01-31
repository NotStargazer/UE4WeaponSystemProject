// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "IDontUnderstandUE4Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "IDontUnderstandUE4/Enemy.h"

AIDontUnderstandUE4Projectile::AIDontUnderstandUE4Projectile() 
{
	/* I had thought this would work. bExplodes is set in the blueprint for each ammo type, so i thought it would be able to check this for individual actors. Guess not.
	if (bExplodes)
	{
		UE_LOG(LogTemp, Display, TEXT("can explode"));
		PrimaryActorTick.bCanEverTick = true;
	}
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
	/*
	if (!bSticky)
	{
		ProjectileMovement->bShouldBounce = true;
	}
	else
	{
		ProjectileMovement->bShouldBounce = false;
	}
	*/
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 10.0f;

}

void AIDontUnderstandUE4Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SecondsToExplode -= DeltaTime;
	UE_LOG(LogTemp, Warning, TEXT("hi, %d"), SecondsToExplode);
	if (SecondsToExplode <= 0.0)
	{
		OnExplode();
	}
}

void AIDontUnderstandUE4Projectile::OnExplode()
{
	UE_LOG(LogTemp, Warning, TEXT("should explode now!"));
	//Destroy();
}

void AIDontUnderstandUE4Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		AEnemy* HitActor = Cast<AEnemy>(OtherActor);

		if (HitActor)
		{
			HitActor->ApplyDamage(Damage);
			Destroy();
		}
	}

	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}