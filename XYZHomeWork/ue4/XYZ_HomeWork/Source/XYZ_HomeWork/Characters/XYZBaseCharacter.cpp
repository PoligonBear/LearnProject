// Fill out your copyright notice in the Description page of Project Settings.

#include "XYZBaseCharacter.h"
#include "XYZ_HomeWork_Types.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Curves/CurveVector.h"
#include "Actors/Interactive/Environment/Ladder.h"
#include "Actors/Interactive/Environment/Zipline.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Controllers/XYZPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/LedgeDetectorComponent.h"
#include "Components/Attributes/CharacterAttributeComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Components/MovementComponents/XYZCharacterMovementComponent.h"


AXYZBaseCharacter::AXYZBaseCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UXYZCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	XYZCharacterMovementComponent = StaticCast<UXYZCharacterMovementComponent*>(GetCharacterMovement());

	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));
	CharacterAttributeComponent = CreateDefaultSubobject< UCharacterAttributeComponent>(TEXT("CharacterAttribute"));
	CharacterEquipmentComponents = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("CharacterEquipment"));

	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;
}

void AXYZBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AXYZBaseCharacter::OnPlayerCapsuleHit);

	CharacterAttributeComponent->OnDeathEvent.AddUObject(this, &AXYZBaseCharacter::OnDeath);
	CharacterAttributeComponent->OutOfStamina.AddUObject(this, &AXYZBaseCharacter::SetIsOutOfStamina);
}

void AXYZBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	XYZPlayerController = Cast<AXYZPlayerController>(NewController);
}

void AXYZBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TryChangeSprintState();
	UpdateIKSettings(DeltaTime);
}

void AXYZBaseCharacter::Slide()
{
	if (GetXYZCharacterMovementComponent()->IsSwimming() || GetXYZCharacterMovementComponent()->IsFalling())
	{
		return;
	}

	if (GetXYZCharacterMovementComponent()->IsSprinting())
	{
		GetXYZCharacterMovementComponent()->StartSlide();

		float MontageStartTime = 0.0f;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(SlidingMontage, 1.0f, EMontagePlayReturnType::Duration, MontageStartTime);
	}
}

void AXYZBaseCharacter::OnStartSlide(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		FVector& MeshRelativeLocation = MeshComponent->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z += HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		if (const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass()))
		{
			BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HalfHeightAdjust;
		}
	}
}

void AXYZBaseCharacter::OnEndSlide(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		FVector& MeshRelativeLocation = MeshComponent->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z -= HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		if (const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass()))
		{
			BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z;
		}
	}
}

void AXYZBaseCharacter::ChangeCrouchState()
{
	if (GetXYZCharacterMovementComponent()->IsSliding())
	{
		return;
	}

	if (!GetCharacterMovement()->IsCrouching())
	{
		Crouch();
	}
}

void AXYZBaseCharacter::StartSprint()
{
	if (bIsCrawled)
	{
		return;
	}

	bIsSprintRequested = true;
	if (bIsCrouched)
	{
		UnCrouch(); 
	}
}

void AXYZBaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

void AXYZBaseCharacter::Jump()
{
	if (bIsCrouched || bIsCrawled)
	{
		GetXYZCharacterMovementComponent()->bWantsToCrouch = false;
		GetXYZCharacterMovementComponent()->bWantsToCrawl = false;
	}
	/** Wall Run **/
	else if (GetXYZCharacterMovementComponent()->IsWallRunning()) 
	{
		FVector CurrentDirection = GetXYZCharacterMovementComponent()->CurrentWallRunDirection;
		FVector JumpDirection = FVector::ZeroVector;

		EWallRunSide CurrentSide = GetXYZCharacterMovementComponent()->CurrentWallRunSide;

		if (CurrentSide == EWallRunSide::Right)
		{
			JumpDirection = FVector::CrossProduct(CurrentDirection, FVector::UpVector).GetSafeNormal();
		}
		else
		{
			JumpDirection = FVector::CrossProduct(FVector::UpVector, CurrentDirection).GetSafeNormal();
		}

		JumpDirection += FVector::UpVector;
		LaunchCharacter(GetCharacterMovement()->JumpZVelocity * JumpDirection.GetSafeNormal(), false, true);

		GetXYZCharacterMovementComponent()->StopWallRun();
	}
	else
	{
		Super::Jump();
	}
}

void AXYZBaseCharacter::ClimLadderUp(float Value)
{
	if(GetXYZCharacterMovementComponent()->IsOnLadder() && !FMath::IsNearlyZero(Value))
	{
		FVector LadderUpVector = GetXYZCharacterMovementComponent()->GetCurrentLadder()->GetActorUpVector();
		AddMovementInput(LadderUpVector, Value);
	}
}

void AXYZBaseCharacter::InteractWithLadder()
{
	if(GetXYZCharacterMovementComponent()->IsSliding())
	{
		return;
	}
	
	if(GetXYZCharacterMovementComponent()->IsOnLadder())
	{
		GetXYZCharacterMovementComponent()->DetachFromLadder(EDetachFromLadderMethod::JumpOff);
	}
	else
	{
		const ALadder* AvailableLadder = GetAvailableLadder();
		if(IsValid(AvailableLadder))
		{
			if(AvailableLadder->GetIsOnTop())
			{
				PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
			}
			GetXYZCharacterMovementComponent()->AttachToLadder(AvailableLadder);
		}		
	}
}

void AXYZBaseCharacter::InteractWithZipline()
{
	if(GetXYZCharacterMovementComponent()->IsSliding())
	{
		return;
	}
	
	if (GetXYZCharacterMovementComponent()->IsOnZipline())
	{
		GetXYZCharacterMovementComponent()->DetachFromZipline();
	}
	else
	{
		const AZipline* AvailableZipline = GetAvailableZipline();
		if (IsValid(AvailableZipline))
		{
			GetXYZCharacterMovementComponent()->AttachToZipline(AvailableZipline);
		}
	}
}

void AXYZBaseCharacter::Mantle(bool bForce)
{
	if(!(CanMantle() || bForce))
	{
		return;
	}

	if (GetXYZCharacterMovementComponent()->IsMantling() || GetXYZCharacterMovementComponent()->IsFalling() || 
		GetXYZCharacterMovementComponent()->IsWallRunning() || GetXYZCharacterMovementComponent()->IsSliding())
	{
		return;
	}
	
	if (bIsCrouched)
	{
		UnCrouch();
	}
	
	FLedgeDescription LedgeDescription;
	if (LedgeDetectorComponent->DetectLedge(LedgeDescription))
	{
		FMantlingMovementParameters MantlingParameters;
		MantlingParameters.TargetComponent = LedgeDescription.TargetComponentHit;
		MantlingParameters.InitialLocation = GetActorLocation() - LedgeDescription.TargetComponentHit->GetComponentLocation();
		MantlingParameters.InitialRotation = GetActorRotation();
		MantlingParameters.TargetLocation = LedgeDescription.Location - LedgeDescription.TargetComponentHit->GetComponentLocation();
		MantlingParameters.TargetRotation = LedgeDescription.Rotaion;

		float MantlingHeight = (MantlingParameters.TargetLocation - MantlingParameters.InitialLocation).Z;
		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);

		float MinRange;
		float MaxRange;
		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);
		MantlingParameters.Duration = MaxRange - MinRange;
		MantlingParameters.MantlingCurve = MantlingSettings.MantlingCurve;	
		
		FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStarTime);
		MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);
		MantlingParameters.InitialAnimationLocation = MantlingParameters.TargetLocation - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + HighMantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;

		GetXYZCharacterMovementComponent()->StartMantle(MantlingParameters);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingParameters.StartTime);

		OnMantle(MantlingSettings, MantlingParameters.StartTime);
	}
}

void AXYZBaseCharacter::StartFire()
{
	if(CharacterEquipmentComponents->IsEquipping())
	{
		return; 
	}
	
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponents->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StartFire();
	}
}

void AXYZBaseCharacter::StopFire()
{
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponents->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopFire();
	}
}

void AXYZBaseCharacter::StartAiming()
{
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if(!IsValid(CurrentRangeWeapon))
	{
		return;
	}
	
	bIsAiming = true;
	CurrentAimingMovementSpeed = CurrentRangeWeapon->GetAimMovementMaxSpeed();
	CurrentRangeWeapon->StartAim();
	OnStartAiming();
}

void AXYZBaseCharacter::StopAiming()
{
	if(!bIsAiming)
	{
		return;
	}
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if(IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopAim();
	}
	
	bIsAiming = false;
	CurrentAimingMovementSpeed = 0.0f;
	CurrentRangeWeapon->StopAim();
	OnStopAiming();
}

void AXYZBaseCharacter::OnStartAiming_Implementation()
{
	OnStarAimingInternal();
}

void AXYZBaseCharacter::OnStopAiming_Implementation()
{
	OnStopAimingInternal();
}

float AXYZBaseCharacter::GetAimingMovementSpeed() const
{
	return CurrentAimingMovementSpeed;
}

bool AXYZBaseCharacter::IsAiming() const
{
	return bIsAiming;
}

void AXYZBaseCharacter::Reload()
{
	if(IsValid(CharacterEquipmentComponents->GetCurrentRangeWeapon()))
	{
		CharacterEquipmentComponents->ReloadCurrentWeapon();
	}
}

void AXYZBaseCharacter::NextItem()
{
	CharacterEquipmentComponents->EquipNextItem();
}

void AXYZBaseCharacter::PreviousItem()
{
	CharacterEquipmentComponents->EquipPreviousItem();
}

void AXYZBaseCharacter::EquipPrimaryItem()
{
	CharacterEquipmentComponents->EquipItemInSlot(EEquipmentSlots::PrimaryItemSlot);	
}

bool AXYZBaseCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() && !GetXYZCharacterMovementComponent()->IsMantling();
}

bool AXYZBaseCharacter::IsHardLanding(FVector& CurrentLocation)
{
	if (CurrentLocation.Z < CurrentFallApex.Z && (CurrentFallApex.Z - CurrentLocation.Z) > HardLandingHeight)
	{
		return true;
	}
		
	return false;
}

void AXYZBaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	CurrentFallApex = GetActorLocation();
}

void AXYZBaseCharacter::Falling()
{
	Super::Falling();
	GetXYZCharacterMovementComponent()->bNotifyApex = true;
}

void AXYZBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	FVector CurrentCharacterLocation = GetActorLocation();

	if (CurrentCharacterLocation.Z > CurrentFallApex.Z && GetXYZCharacterMovementComponent()->MovementMode == MOVE_Swimming)
	{
		return;
	}

	if (IsHardLanding(CurrentCharacterLocation))
	{
		OnHardLanded();	
	}

	float FallHeight = (CurrentFallApex - GetActorLocation()).Z * 0.01f;

	if (IsValid(FallDamageCurve))
	{
		float DamageAmount = FallDamageCurve->GetFloatValue(FallHeight);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.Actor.Get());
	}
}

void AXYZBaseCharacter::OnHardLanded()
{
	if (XYZPlayerController.IsValid())
	{
		XYZPlayerController->SetIgnoreMoveInput(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		float MontageStartTime = 0.0f;
		float MontageDuration = AnimInstance->Montage_Play(HardLandingMontage, 1.0f, EMontagePlayReturnType::Duration, MontageStartTime);
		GetWorld()->GetTimerManager().SetTimer(MontageTimer, this, &AXYZBaseCharacter::OnMontageTimerElapsed, MontageDuration, false);
	}
}

void AXYZBaseCharacter::OnMontageTimerElapsed()
{
	if (XYZPlayerController.IsValid())
	{
		XYZPlayerController->SetIgnoreLookInput(false);
		XYZPlayerController->SetIgnoreMoveInput(false);
	}
}

UCharacterEquipmentComponent* AXYZBaseCharacter::GetCharacterEquipmentComponent_Mutable() const
{
	return CharacterEquipmentComponents;
}

const UCharacterEquipmentComponent* AXYZBaseCharacter::GetCharacterEquipmentComponent() const
{
	return CharacterEquipmentComponents;
}

UCharacterAttributeComponent* AXYZBaseCharacter::GetCharacterAttributeComponent_Mutable() const
{
	return CharacterAttributeComponent;
}

const UCharacterAttributeComponent* AXYZBaseCharacter::GetCharacterAttributeComponent() const
{
	return  CharacterAttributeComponent;
}

bool AXYZBaseCharacter::CanSprint()
{
	if(bIsCrouched)
	{		
			FHitResult OutHint;
			FVector StartPosition = GetCharacterMovement()->UpdatedComponent->GetComponentLocation();
			FVector UpVector = GetCapsuleComponent()->GetUpVector();
			FVector EndPosition = ((UpVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 10.0f) + StartPosition);
			FCollisionQueryParams  CollisionParams;

			DrawDebugLine(GetWorld(), StartPosition, EndPosition, FColor::Red, false, 1, 0, 1);

			if (GetWorld()->LineTraceSingleByChannel(OutHint, StartPosition, EndPosition, ECC_Visibility, CollisionParams))
			{
				if (OutHint.bBlockingHit )
				{
					return false;
				}
			}
		
		return  false;
	}

	if( XYZCharacterMovementComponent->IsOutOfStamina())
	{
		return  false;
	}
	
	if(XYZCharacterMovementComponent->GetLastUpdateVelocity().IsZero())
	{
		return false;
	}

	if (GetXYZCharacterMovementComponent()->IsSwimming() || GetXYZCharacterMovementComponent()->IsFalling())
	{
		return false;
	}
	
	return true;
}

bool AXYZBaseCharacter::CanMantle() const
{
	return !GetXYZCharacterMovementComponent()->IsOnLadder();
}

void AXYZBaseCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime)
{
}

void AXYZBaseCharacter::OnDeath()
{
	GetCharacterMovement()->DisableMovement();

	float Duratuion = PlayAnimMontage(OnDeathAnimMontage);

	if (Duratuion == 0.0f)
	{
		EnableRagDoll();
	}
}

void AXYZBaseCharacter::OnStarAimingInternal()
{
	if(OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(true);
	}
}

void AXYZBaseCharacter::OnStopAimingInternal()
{
	if(OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(false);
	}
}

void AXYZBaseCharacter::UpdateIKSettings(float DeltaSeconds)
{
	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, CalculateIKOffsetForSocketName(RightFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, CalculateIKOffsetForSocketName(LeftFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKPelvisOffset = FMath::FInterpTo(IKPelvisOffset, CalculateIKPelvisOffset(), DeltaSeconds, IKInterpSpeed);
}

float AXYZBaseCharacter::CalculateIKOffsetForSocketName(const FName& SocketName) const
{
	float Result = 0.0f;

	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
	const FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
	const FVector TraceEnd = TraceStart - (CapsuleHalfHeight + IKTraceDistance) * FVector::UpVector;

	FHitResult HitResult;
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	const FVector FootBoxSize = FVector(2.0f, 12.0f, 5.0f);
	if(UKismetSystemLibrary::BoxTraceSingle(GetWorld(), TraceStart, TraceEnd, FootBoxSize, GetMesh()->GetSocketRotation(SocketName), TraceType, true, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true))
	{
		Result = TraceStart.Z - CapsuleHalfHeight - HitResult.Location.Z;
	}

	return  Result;
}

float AXYZBaseCharacter::CalculateIKPelvisOffset()
{
	return IKRightFootOffset > IKLeftFootOffset ? -IKRightFootOffset : -IKLeftFootOffset;
}

void AXYZBaseCharacter::TryChangeSprintState()
{
	if (bIsSprintRequested && !XYZCharacterMovementComponent->IsSprinting() && CanSprint())
	{
		XYZCharacterMovementComponent->StarSprint();
	}

	if (!bIsSprintRequested && XYZCharacterMovementComponent->IsSprinting() || GetXYZCharacterMovementComponent()->IsSliding())
	{
		XYZCharacterMovementComponent->StopSprint();
	}
}

const FMantlingSettings& AXYZBaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight > LowMantleMaxHeight ? HighMantlingSettings : LowMantlingSettings;
}	

void AXYZBaseCharacter::ChangeCrawlState()
{
	if(GetXYZCharacterMovementComponent()->IsCrawling())
	{
		UnCrawl();
	}
	else if (GetXYZCharacterMovementComponent()->IsCrouching())
	{

		Crawl();
	}
}

void AXYZBaseCharacter::Crawl(bool bClientSimulation)
{
	if (GetXYZCharacterMovementComponent())
	{
		if (CanCrawl())
		{
			GetXYZCharacterMovementComponent()->bWantsToCrawl = true;
		}
	}
}

void AXYZBaseCharacter::UnCrawl(bool bClientSimulation)
{
	if (GetXYZCharacterMovementComponent())
	{
		GetXYZCharacterMovementComponent()->bWantsToCrawl = false;
	}
}

bool AXYZBaseCharacter::CanCrawl() const
{
	return !bIsCrawled && bIsCrouched && GetXYZCharacterMovementComponent() && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

void AXYZBaseCharacter::OnEndCrawl(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		FVector& MeshRelativeLocation = MeshComponent->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z -= HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		if (const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass()))
		{
			BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z;
		}
	}
}

void AXYZBaseCharacter::OnStartCrawl(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		FVector& MeshRelativeLocation = MeshComponent->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z += HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		if (const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass()))
		{
			BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HalfHeightAdjust;
		}
	}
}

void AXYZBaseCharacter::RegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.AddUnique(InteractiveActor);
}

void AXYZBaseCharacter::UnRegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.RemoveSingleSwap(InteractiveActor);
}

const ALadder* AXYZBaseCharacter::GetAvailableLadder() const
{
	const ALadder* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if(InteractiveActor->IsA<ALadder>())
		{
			Result = StaticCast<const ALadder*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

const AZipline* AXYZBaseCharacter::GetAvailableZipline() const
{
	const AZipline* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if (InteractiveActor->IsA<AZipline>())
		{
			Result = StaticCast<const AZipline*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

void AXYZBaseCharacter::SetIsOutOfStamina(bool bIsOutOfStamina_In)
{
	GetXYZCharacterMovementComponent()->bIsOutOfStamina = bIsOutOfStamina_In;
	if (bIsOutOfStamina_In)
	{
		StopSprint();
	}
}

bool AXYZBaseCharacter::IsSwimingUnderWater(FVector SocketLocation, float VolumeTopPlane) const
{
	if (VolumeTopPlane < SocketLocation.Z || !GetXYZCharacterMovementComponent()->IsSwimming())
	{
		return false;
	}
	
	return true;
}

void AXYZBaseCharacter::OnPlayerCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FVector HitNormal = Hit.ImpactNormal;

	EWallRunSide Side = EWallRunSide::None;
	FVector Direction = FVector::ZeroVector;

	GetXYZCharacterMovementComponent()->GetWallRunSideAndDirection(HitNormal, Side, Direction);
	
	if (!GetXYZCharacterMovementComponent()->CanWallRun(HitNormal, Side))
	{
		return;
	}

	GetXYZCharacterMovementComponent()->StartWallRun(Side, Direction);
}

void AXYZBaseCharacter::EnableRagDoll()
{
	GetMesh()->SetCollisionProfileName(CollisionProfileRagDoll);
	GetMesh()->SetSimulatePhysics(true);
}
