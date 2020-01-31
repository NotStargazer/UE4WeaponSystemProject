// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AEnemy::AEnemy()
{
	Target = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Target"));

	DamageDetails = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Details"));
	DamageDetails->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	DamageDetails->SetupAttachment(Target);
	DamageDetails->SetRelativeLocation(Target->GetRelativeLocation() + FVector(0.0f, 0.0f, 100.0f));

	ChangeTextDetails();
}

// Called when the game starts or when spawned

void AEnemy::ApplyDamage(int Damage)
{
	LastDamage = Damage;

	DamagePerSecond += Damage;

	ChangeTextDetails();

	if (!GetWorld()->GetTimerManager().IsTimerActive(DpsTimer))
	{
		UE_LOG(LogTemp, Display, TEXT("TimerStart"));
		GetWorld()->GetTimerManager().SetTimer(DpsTimer, this, &AEnemy::DPSTimer, 1, false);
	}
}

void AEnemy::ChangeTextDetails()
{
	FString text;

	text += "Last Hit Damage: ";
	text.AppendInt(LastDamage);
	text += "\nDamage In This Second: ";
	text.AppendInt(DamagePerSecond);
	text += "\nDamage In Last Second: ";
	text.AppendInt(DamagePerSecondOutput);

	DamageDetails->SetText(text);
}

void AEnemy::DPSTimer()
{
	DamagePerSecondOutput = DamagePerSecond;
	DamagePerSecond = 0;

	ChangeTextDetails();

	GetWorld()->GetTimerManager().ClearTimer(DpsTimer);
}


