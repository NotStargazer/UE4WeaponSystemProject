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

	UPROPERTY()
		AController* PlayerController;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AIDontUnderstandUE4Projectile> ProjectileClass;

	//Funcs
	virtual void Fire(FVector Location, FRotator Direction) override;


};
