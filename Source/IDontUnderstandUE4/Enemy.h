// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.generated.h"

UCLASS()
class IDONTUNDERSTANDUE4_API AEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemy();

	void ApplyDamage(int Damage);

	void ChangeTextDetails();

	void DPSTimer();

	UPROPERTY(EditAnywhere, Category = Mesh)
	class UStaticMeshComponent* Target;

	class UTextRenderComponent* DamageDetails;

	int LastDamage;

	int DamagePerSecond;

	int DamagePerSecondOutput;

	FTimerHandle DpsTimer;
};
