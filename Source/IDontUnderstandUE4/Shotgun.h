// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Shotgun.generated.h"

UCLASS()
class IDONTUNDERSTANDUE4_API AShotgun : public AWeapon
{
	GENERATED_BODY()

public:
	/** Max range the weapon can do damage */
	UPROPERTY(EditAnywhere)
		float MaxRange = 100000;

	/** Max range for damage falloff*/
	UPROPERTY(EditAnywhere)
		float MaxDamageFalloffRange = 100000;

	/** Min range for damage falloff */
	UPROPERTY(EditAnywhere)
		float MinDamageFalloffRange = 100000;

	/** % damage loss from distance */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "100"))
		float DamageFalloffPercentage = 0;

	/** Amount of extra shots to fire */
	UPROPERTY(EditAnywhere)
		float ProjectilesExtra = 7;

	UPROPERTY()
		AController* PlayerController;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AIDontUnderstandUE4Projectile> ProjectileClass;

	//Funcs
	virtual void Fire(FVector Location, FRotator Direction) override;

	
};
