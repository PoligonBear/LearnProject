// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZCharacterAnimInstance.h"
#include "Characters/XYZBaseCharacter.h"
#include "Components/MovementComponents/XYZCharacterMovementComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"


void UXYZCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AXYZBaseCharacter>(), TEXT("UXYZCharacterAnimInstance::NativeBeginPlay() UXYZCharacterAnimInstance can be used only with AXYZBaseCharacter"));
	CachedBaseCharacter = StaticCast<AXYZBaseCharacter*>(TryGetPawnOwner());
}

void UXYZCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedBaseCharacter.IsValid())
	{
		return;
	}

	bIsAiming = CachedBaseCharacter->IsAiming();

	UXYZCharacterMovementComponent* CharacterMovement = CachedBaseCharacter->GetXYZCharacterMovementComponent();

	Speed = CharacterMovement->GetVelocity();
	Direction = CalculateDirection(CharacterMovement->Velocity, CachedBaseCharacter->GetActorRotation());

	bIsFalling = CharacterMovement->IsFalling();
	bIsCrouching = CharacterMovement->IsCrouching() || CharacterMovement->bWantsToCrawl;
	bIsSprinting = CharacterMovement->IsSprinting();
	bIsOutOfStamina = CharacterMovement->IsOutOfStamina();
	bIsCrawling = CharacterMovement->IsCrawling();
	bIsSwimming = CharacterMovement->IsSwimming();
	bIsOnLadder = CharacterMovement->IsOnLadder();
	bIsOnZipline = CharacterMovement->IsOnZipline();
	bIsWallRunning = CharacterMovement->IsWallRunning();
	bIsSliding = CharacterMovement->IsSliding();
	bIsStrafing = CharacterMovement->bOrientRotationToMovement;

	WallRunSide = CharacterMovement->CurrentWallRunSide;

	if(bIsOnLadder)
	{
		LadderSpeedRatio = CharacterMovement->GetLadderSpeedRatio();	
	}

	if (bIsOnZipline)
	{
		ZiplineSpeedRatio = CharacterMovement->GetZiplineSpeed();
	}
	

	IKFootRightOffset = FVector((CachedBaseCharacter->GetIKRightFootOffset() + CachedBaseCharacter->GetIKPelvisOffset()), 0.0f, 0.0f);
	IKFootLeftOffset = FVector(-(CachedBaseCharacter->GetIKLeftFootOffset() + CachedBaseCharacter->GetIKPelvisOffset()), 0.0f, 0.0f);
	IKPelvisBoneOffset = FVector(0.0f, 0.0f, CachedBaseCharacter->GetIKPelvisOffset());

	AimRotation = CachedBaseCharacter->GetBaseAimRotation();

	const UCharacterEquipmentComponent* CharacterEquipmentComponent = CachedBaseCharacter->GetCharacterEquipmentComponent();
	CurretnEquippedItemType = CharacterEquipmentComponent->GetCurrentEquippedItemType();

	ARangeWeaponItem* CurretnRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();

	if (IsValid(CurretnRangeWeapon))
	{
		ForeGripSocketTransform = CurretnRangeWeapon->GetForeGripTransform();
	}
}
