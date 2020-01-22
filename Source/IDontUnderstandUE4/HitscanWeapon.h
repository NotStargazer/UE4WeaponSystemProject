// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitscanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class IDONTUNDERSTANDUE4_API AHitscanWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	//Props
	UPROPERTY(EditAnywhere)
		float MaxRange = 100000;

	UPROPERTY(EditAnywhere)
		float MaxDamageFalloffRange = 1500;

	UPROPERTY(EditAnywhere)
		float MinDamageFalloffRange = 3000;

	UPROPERTY(EditAnywhere, meta=(ClampMin="0", ClampMax="100"))
		float DamageFalloffPercentage = 50;

	UPROPERTY()
		AController* PlayerController;

	//Funcs
	virtual void Fire(FVector Location, FRotator Direction) override;
	
};
