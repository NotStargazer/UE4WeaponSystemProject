// Fill out your copyright notice in the Description page of Project Settings.


#include "HitscanWeapon.h"
#include "Engine/World.h"
#include "IDontUnderstandUE4/Enemy.h"

//TODO Find viewport and get it's center
void AHitscanWeapon::Fire(FVector Location, FRotator Direction)
{
	FHitResult Hit;

	FVector End;

	End = Location + (Direction.Vector() * MaxRange);

	FVector Start = MuzzleLocation->GetComponentLocation();

	//Line trace
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
	{
		float HitDistance = (Start - Hit.ImpactPoint).Size();

		AEnemy* HitActor = (AEnemy*)Hit.GetActor();
		if (IsValid(HitActor))
		{
			if (HitActor->IsA<AEnemy>())
			{
				//Calculate damage falloff
				float DamageFalloff = ((float)DamageFalloffPercentage / 100 * ((FMath::Clamp(HitDistance, MinDamageFalloffRange, MaxDamageFalloffRange)
					- MinDamageFalloffRange) / (float)(MaxDamageFalloffRange - MinDamageFalloffRange)));

				int AppliedDamage = Damage * (1 - DamageFalloff);
				HitActor->ApplyDamage(AppliedDamage);
			}
		}
	}

	CurrentAmmo--;
}