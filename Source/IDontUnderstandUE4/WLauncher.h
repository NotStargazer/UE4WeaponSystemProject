// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "WLauncher.generated.h"

/**
 *
 */
UCLASS()
class IDONTUNDERSTANDUE4_API AWLauncher : public AWeapon
{
	GENERATED_BODY()

public:
	//Props
	UPROPERTY(EditAnywhere)
		float MaxRange = 100000;

	UPROPERTY(EditAnywhere)
		float MaxDamageFalloffRange = 100000;

	UPROPERTY(EditAnywhere)
		float MinDamageFalloffRange = 100000;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "100"))
		float DamageFalloffPercentage = 0;

	UPROPERTY()
		AController* PlayerController;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AIDontUnderstandUE4Projectile> ProjectileClass;

	//Funcs
	virtual void Fire(FVector Location, FRotator Direction) override;


};
