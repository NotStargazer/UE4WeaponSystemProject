// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Weapon.h"

AWeapon::AWeapon()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	WeaponMesh->SetSimulatePhysics(false);

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(WeaponMesh);
	MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	PickupCollider = CreateDefaultSubobject<UBoxComponent>(FName("PickupCollider"));
	PickupCollider->SetupAttachment(WeaponMesh);
	PickupCollider->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmo = MaxAmmo;
}