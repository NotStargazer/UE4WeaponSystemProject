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
	/**Maximum linetrace range, since unreal doesn't support infinite raytracing */
	UPROPERTY(EditAnywhere)
		float MaxRange = 100000;

	/**Maximum range before weapon damage is minimum*/
	UPROPERTY(EditAnywhere)
		float MaxDamageFalloffRange = 1500;

	/**Minmum range before weapon falloff begins*/
	UPROPERTY(EditAnywhere)
		float MinDamageFalloffRange = 3000;

	/**Maximum % damage falloff at max falloff range*/
	UPROPERTY(EditAnywhere, meta=(ClampMin="0", ClampMax="100"))
		float DamageFalloffPercentage = 50;

	/**Fire weapon*/
	virtual void Fire(FVector Location, FRotator Direction) override;
	
};
