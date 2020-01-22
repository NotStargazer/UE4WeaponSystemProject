// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class IDONTUNDERSTANDUE4_API AWeapon : public AActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay();

public:	
	AWeapon();

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* MuzzleLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UBoxComponent* PickupCollider;

	UPROPERTY(EditAnywhere)
		int MaxAmmo = 0;

		int CurrentAmmo;

	UPROPERTY(EditAnywhere)
		float Damage = 0;

	UPROPERTY(EditAnywhere)
		float Recoil = 0;

	UPROPERTY(EditAnywhere)
		bool IsSingleFire = true;

	UPROPERTY(EditAnywhere)
		float FireRateInterval;

	UPROPERTY(EditAnywhere, meta=(EditCondition="IsSingleFire", ClampMin="1"))
		float BulletsFiredPerClick;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "IsSingleFire"))
		float FireCooldown;
	
	UFUNCTION()
		virtual void Fire(FVector Location, FRotator Direction) {}
};
