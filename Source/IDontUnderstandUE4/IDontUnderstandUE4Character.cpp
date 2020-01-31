// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "IDontUnderstandUE4Character.h"
#include "IDontUnderstandUE4Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "IDontUnderstandUE4\Weapon.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AIDontUnderstandUE4Character

AIDontUnderstandUE4Character::AIDontUnderstandUE4Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.


	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void AIDontUnderstandUE4Character::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Display, TEXT("Hit"));
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (OtherActor->IsA<AWeapon>())
		{
			UE_LOG(LogTemp, Display, TEXT("Hit"));
			OtherActor->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetIncludingScale, "GripPoint");

			OtherActor->SetActorTransform(Mesh1P->GetSocketTransform("GripPoint"));
			CurrentWeapon = Cast<AWeapon>(OtherActor);
		}
	}
}

void AIDontUnderstandUE4Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AIDontUnderstandUE4Character::OnBeginOverlap);

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AIDontUnderstandUE4Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AIDontUnderstandUE4Character::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AIDontUnderstandUE4Character::StopFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AIDontUnderstandUE4Character::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AIDontUnderstandUE4Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AIDontUnderstandUE4Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AIDontUnderstandUE4Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AIDontUnderstandUE4Character::LookUpAtRate);
}

void AIDontUnderstandUE4Character::FireCurrentWeapon()
{
	// Check if player has a weapon in hand
	if (CurrentWeapon != nullptr)
	{
		// Make sure the weapon has ammo
		if (CurrentWeapon->CurrentAmmo <= 0)
		{
			UE_LOG(LogTemp, Display, TEXT("Ammo: %d"), CurrentWeapon->CurrentAmmo);
			return;
		}

		// Get the location and rotation of the player
		const FRotator SpawnRotation = GetControlRotation();
		const FVector SpawnLocation = GetTargetLocation();

		// Fire weapon in hand
		CurrentWeapon->Fire(SpawnLocation, SpawnRotation);

		if (CurrentWeapon->IsSingleFire)
		{
			GetWorld()->GetTimerManager().SetTimer(FireWeapon, CurrentWeapon->FireCooldown, false);
		}

		// try and play the sound if specified
		if (CurrentWeapon->FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, CurrentWeapon->FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}
	}
}

void AIDontUnderstandUE4Character::BurstFireCurrentWeapon()
{
	// How many times the burst fire weapon has fired
	static int FireIntervals;

	// Check if player has a weapon in hand 
	if (CurrentWeapon != nullptr)
	{
		// Check if weapon has exceded the amount of fire intervals this weapon has
		if (FireIntervals < CurrentWeapon->BulletsFiredPerClick)
		{
			// Stop firing if no bullets are left
			if (CurrentWeapon->CurrentAmmo <= 0)
			{
				UE_LOG(LogTemp, Display, TEXT("Ammo: %d"), CurrentWeapon->CurrentAmmo);
				FireIntervals = 0;
				GetWorld()->GetTimerManager().ClearTimer(FireWeapon);
				return;
			}

			// Get player location and rotation
			const FRotator SpawnRotation = GetControlRotation();
			const FVector SpawnLocation = GetTargetLocation();
			
			// Fire weapon and increase intervals
			CurrentWeapon->Fire(SpawnLocation, SpawnRotation);
			FireIntervals++;

			// try and play the sound if specified
			if (CurrentWeapon->FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, CurrentWeapon->FireSound, GetActorLocation());
			}

			// try and play a firing animation if specified
			if (FireAnimation != nullptr)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
				if (AnimInstance != nullptr)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}
		}
		else
		{
			//Intervals exceeded, reset
			FireIntervals = 0;
			GetWorld()->GetTimerManager().ClearTimer(FireWeapon);
			//Fire cooldown
			GetWorld()->GetTimerManager().SetTimer(FireWeapon, CurrentWeapon->FireCooldown, false);
			return;
		}
	}
	else
	{
		FireIntervals = 0;
		GetWorld()->GetTimerManager().ClearTimer(FireWeapon);
		return;
	}
}

void AIDontUnderstandUE4Character::StartFire()
{
	// Fire cooldown
	if (GetWorld()->GetTimerManager().IsTimerActive(FireWeapon))
	{
		return;
	}
	// Check if player is holding a weapon
	if (CurrentWeapon != nullptr)
	{
		// Check weapon firemode
		if (CurrentWeapon->IsSingleFire)
		{
			// Check how many bullets fired per click
			if (CurrentWeapon->BulletsFiredPerClick <= 1)
			{
				//Single fire
				FireCurrentWeapon();
			}
			else
			{
				//Burst fire
				BurstFireCurrentWeapon();
				GetWorld()->GetTimerManager().SetTimer(FireWeapon, this, &AIDontUnderstandUE4Character::BurstFireCurrentWeapon, CurrentWeapon->FireRateInterval, true);
			}
		}
		else
		{
			//Automatic fire
			FireCurrentWeapon();
			GetWorld()->GetTimerManager().SetTimer(FireWeapon, this, &AIDontUnderstandUE4Character::FireCurrentWeapon, CurrentWeapon->FireRateInterval, true);
		}
	}
}

void AIDontUnderstandUE4Character::StopFire()
{
	//Check if player has weapon
	if (CurrentWeapon != nullptr)
	{
		//Check if weapon is automatic fire
		if (!CurrentWeapon->IsSingleFire)
		{
			GetWorld()->GetTimerManager().ClearTimer(FireWeapon);
		}
	}
}

void AIDontUnderstandUE4Character::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AIDontUnderstandUE4Character::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		StartFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AIDontUnderstandUE4Character::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AIDontUnderstandUE4Character::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AIDontUnderstandUE4Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AIDontUnderstandUE4Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AIDontUnderstandUE4Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AIDontUnderstandUE4Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AIDontUnderstandUE4Character::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AIDontUnderstandUE4Character::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AIDontUnderstandUE4Character::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AIDontUnderstandUE4Character::TouchUpdate);
		return true;
	}

	return false;
}
