// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IDontUnderstandUE4Projectile.generated.h"

UCLASS(config=Game)
class AIDontUnderstandUE4Projectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	/** Damage projectile deals on impact */
	UPROPERTY(EditAnywhere)
		int Damage = 200;

	/** Whether projectile sticks to other things */
	UPROPERTY(EditAnywhere)
		bool bSticky;

	/** Whether to explode after a set time */
	UPROPERTY(EditAnywhere)
		bool bExplodes;

	/** Time until exploding (in seconds) */
	UPROPERTY(EditAnywhere)
		int SecondsToExplode = 3;

	FTimerHandle ExplosionTimer;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> ActorToSpawn;

protected:
	virtual void BeginPlay();

public:
	AIDontUnderstandUE4Projectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void OnExplode();

	void ExplosionTimerFunction();

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};