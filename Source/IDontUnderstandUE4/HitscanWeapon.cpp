// Fill out your copyright notice in the Description page of Project Settings.


#include "HitscanWeapon.h"
#include "Engine/World.h"

//TODO Find viewport and get it's center
void AHitscanWeapon::Fire(FVector Location, FRotator Direction)
{
	FHitResult Hit;

	FVector End;

	End = Location + (Direction.Vector() * MaxRange);

	FVector Start = MuzzleLocation->GetComponentLocation();

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
	{
		float HitDistance = (Start - Hit.ImpactPoint).Size();

		//TODO Enable when enemy exists
		//AEnemy* HitActor = (AEnemy*)Hit.GetActor()
		//if (IsValid(HitActor))
		//{
		//	if (HitActor->IsA<AEnemy>())
		//	{
		//		int AppliedDamage = Damage * (DamageFalloffPercentage * (1 - ((FMath::Clamp(HitDistance, MinDamageFalloffRange, MaxDamageFalloffRange)
		//			- MinDamageFalloffRange) / (MaxDamageFalloffRange - MinDamageFalloffRange))));
		//		HitActor->ApplyDamage(AppliedDamage);
		//	}
		//}

		
		float DamageFalloff = ((float)DamageFalloffPercentage / 100 * ((FMath::Clamp(HitDistance, MinDamageFalloffRange, MaxDamageFalloffRange)
			- MinDamageFalloffRange) / (float)(MaxDamageFalloffRange - MinDamageFalloffRange)));

		int AppliedDamage = Damage * (1 - DamageFalloff);

		UE_LOG(LogTemp, Display, TEXT("Falloff: %f"), DamageFalloff);
		UE_LOG(LogTemp, Display, TEXT("Distance: %f"), HitDistance);
		UE_LOG(LogTemp, Display, TEXT("Damage: %d"), AppliedDamage);
	}

	CurrentAmmo--;
}