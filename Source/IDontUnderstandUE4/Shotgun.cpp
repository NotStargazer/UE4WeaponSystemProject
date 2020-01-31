// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/World.h"
#include "IDontUnderstandUE4Projectile.h"

void AShotgun::Fire(FVector Location, FRotator Direction)
{
	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FRotator OldDirection = Direction;
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		
		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		//ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		Location = ((MuzzleLocation != nullptr) ? MuzzleLocation->GetComponentLocation() : GetActorLocation()) + Direction.RotateVector(FVector(100.0f, 0.0f, 10.0f));
		World->SpawnActor<AIDontUnderstandUE4Projectile>(ProjectileClass, Location, Direction, ActorSpawnParams);
		CurrentAmmo--;

		for (size_t i = 0; i < ProjectilesExtra; i++)
		{
			Direction.Pitch += rand() % 6 + -3;
			Direction.Yaw += rand() % 6 + -3;
			Direction.Roll += rand() % 6 + -3;
			World->SpawnActor<AIDontUnderstandUE4Projectile>(ProjectileClass, Location, Direction, ActorSpawnParams);
			Direction = OldDirection;
			CurrentAmmo--;
		}
	}
}
