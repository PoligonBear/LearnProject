
// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "XYZ_HomeWork/Characters/XYZBaseCharacter.h"
#include "Curves/CurveVector.h"
#include "XYZ_HomeWork/Actors/Interactive/Environment/Ladder.h"
#include "XYZ_HomeWork/Actors/Interactive/Environment/Zipline.h"
#include "DrawDebugHelpers.h"
#include "XYZ_HomeWork/XYZ_HomeWork_Types.h"


void UXYZCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if(bForceRotation)
	{
		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
		CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

		FRotator DeltaRot = GetDeltaRotation(DeltaTime);
		DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

		const float AngleTolerance = 1e-3f;

		if (!CurrentRotation.Equals(ForcedTargetRotation, AngleTolerance))
		{
			FRotator DesiredRotation = ForcedTargetRotation;
			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
			}

			// Set the new rotation.
			DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
			MoveUpdatedComponent( FVector::ZeroVector, DesiredRotation, /*bSweep*/ false );
		}
		else
		{
			ForcedTargetRotation = FRotator::ZeroRotator;
			bForceRotation = false;
		}
		return;
	}
	
	if(IsOnLadder())
	{
		return;
	}
	Super::PhysicsRotation(DeltaTime);
}

void UXYZCharacterMovementComponent::SetIsOutOfStamina(bool bIsOutOfStamina_In)
{
	bIsOutOfStamina = bIsOutOfStamina_In;
	if(bIsOutOfStamina)
	{
		StopSprint();		
	}
}

void UXYZCharacterMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);


	if (HasValidData() || HasAnimRootMotion() || DeltaTime < MIN_TICK_TIME || (CharacterOwner && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy && bWasSimulatingRootMotion))
	{
		if(IsSliding())
		{
			Velocity = MaxSlidingSpeed * GetBaseCharacterOwner()->GetActorForwardVector();
		}
	}
	UpdateComponentVelocity();
}

float UXYZCharacterMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();

	if (IsSliding())
	{
	Result = MaxSlidingSpeed;
	}
	else if (bIsSprinting)
	{
		Result = SprintSpeed;
	}
	else if(bIsOutOfStamina)
	{
		Result = OutOfStaminaSpeed;
	}
	else if(IsCrawling())
	{
		Result = MaxCrawlSpeed;
	}
	else if(IsOnLadder())
	{
		Result  = ClimbingOnLadderMaxSpeed;
	}
	else if (IsOnZipline())
	{
		Result = ZiplineSpeed;
	}
	else if (IsWallRunning())
	{
		Result = MaxWallRunSpeed;
	}
	else if(GetBaseCharacterOwner()->IsAiming())
	{
		Result = GetBaseCharacterOwner()->GetAimingMovementSpeed();
	}

	return Result;
}

void UXYZCharacterMovementComponent::StarSprint()
{
	bIsSprinting = true;
	bForceMaxAccel = 1;
}

void UXYZCharacterMovementComponent::StopSprint()
{
	bIsSprinting = false;
	bForceMaxAccel = 0;
}

float UXYZCharacterMovementComponent::GetLadderSpeedRatio() const
{
	checkf(CharacterOwner->IsA<AXYZBaseCharacter>(), TEXT("UXYZCharacterMovementComponent::GetLadderSpeedRation() UXYZCharacterMovementComponent can be used only with AXZYBaseCharacter"));
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	return FVector::DotProduct(LadderUpVector, Velocity) / ClimbingOnLadderMaxSpeed;
}

bool UXYZCharacterMovementComponent::IsCrawling() const
{
	checkf(CharacterOwner->IsA<AXYZBaseCharacter>(), TEXT("UXYZCharacterMovementComponent::IsProning() UXYZCharacterMovementComponent can be used only with AXZYBaseCharacter"));
	AXYZBaseCharacter* CachedBaseCharacter = StaticCast<AXYZBaseCharacter*>(CharacterOwner);
	return CachedBaseCharacter && CachedBaseCharacter->bIsCrawled;
}

void UXYZCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);

	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		const bool bIsProning = IsCrawling();
		if (bIsProning && (!bWantsToCrawl || !CanCrawlInCurrentState()))
		{
			UnCrawl();
		}
		else if (!bIsProning && bWantsToCrawl && CanCrawlInCurrentState())
		{
			Crawl();
		}
	}
}

void UXYZCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);
	
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		if (IsCrawling() && !CanCrawlInCurrentState())
		{
			UnCrawl();
		}
	}
}

bool UXYZCharacterMovementComponent::CanCrawlInCurrentState() const
{
	return (IsFalling() || IsMovingOnGround() || IsCrouching()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

void UXYZCharacterMovementComponent::Crawl()
{
	if (!HasValidData())
	{
		return;
	}

	if (!CanCrouchInCurrentState())
	{
		return;
	}

	checkf(CharacterOwner->IsA<AXYZBaseCharacter>(), TEXT("UPABaseCharacterMovementComponent::Prone(bool bClientSimulation) UPABaseCharacterMovementComponent can be used only with APAKSBaseCharacter"));
	AXYZBaseCharacter* CachedBaseCharacter = StaticCast<AXYZBaseCharacter*>(CharacterOwner);

	if (CachedBaseCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == CrawlCapsuleHalfHeight &&
		CachedBaseCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius() == CrawlCapsuleRadius)
	{
		CachedBaseCharacter->bIsCrawled = true;
		CachedBaseCharacter->bIsCrouched = true;

		CachedBaseCharacter->OnStartCrawl(0.f, 0.f);
		return;
	}

	const float CurrentCrawledHalfHeight = CachedBaseCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float ComponentScale = CachedBaseCharacter->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CachedBaseCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = OldUnscaledHalfHeight - CrawlCapsuleHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const float ScaledRadius = CrawlCapsuleRadius * ComponentScale;
	const float ScaledHalfHeight = CrawlCapsuleHalfHeight * ComponentScale;

	check(CachedBaseCharacter->GetCapsuleComponent());

	const float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleParams, ResponseParam);

	FVector CapsuleExtent(ScaledRadius, ScaledRadius, ScaledHalfHeight - SweepInflation);
	const float MinExtent = KINDA_SMALL_NUMBER * 10.f;
	CapsuleExtent.Z = FMath::Max(CapsuleExtent.Z, MinExtent);
	const FCollisionShape StandingCapsuleShape = FCollisionShape::MakeCapsule(CapsuleExtent);
	const ECollisionChannel CollisionChannel = CachedBaseCharacter->GetCharacterMovement()->UpdatedComponent->GetCollisionObjectType();
	const FVector PawnLocation = CachedBaseCharacter->GetCharacterMovement()->UpdatedComponent->GetComponentLocation() + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentCrawledHalfHeight);
	bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

	if (bEncroached)
	{
		return;
	}

	UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);

	CachedBaseCharacter->bIsCrawled = true;
	CachedBaseCharacter->bIsCrouched = true;

	bForceNextFloorCheck = true;

	const float MeshAdjust = ScaledHalfHeightAdjust;
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(ScaledRadius, ScaledHalfHeight);

	AdjustProxyCapsuleSize();
	CachedBaseCharacter->OnStartCrawl(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void UXYZCharacterMovementComponent::UnCrawl()
{
	if (!HasValidData())
	{
		return;
	}

	checkf(CharacterOwner->IsA<AXYZBaseCharacter>(), TEXT("UXYZBaseCharacterMovementComponent::UnProne(bool bClientSimulation)UXYZBaseCharacterMovementComponent can be used only with AXYZBaseCharacter"));
	AXYZBaseCharacter* CachedBaseCharacter = StaticCast<AXYZBaseCharacter*>(CharacterOwner);

	if (CachedBaseCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == CrouchedHalfHeight)
	{
		CachedBaseCharacter->bIsCrawled = false;
		CachedBaseCharacter->bIsCrouched = true;

		CachedBaseCharacter->OnEndCrawl(0.f, 0.f);
		return;
	}

	const float CurrentCrouchedHalfHeight = CachedBaseCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float ComponentScale = CachedBaseCharacter->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CachedBaseCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = CrouchedHalfHeight - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	check(CharacterOwner->GetCapsuleComponent());

	const UWorld* MyWorld = GetWorld();
	const float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleParams, ResponseParam);

	const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust);
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
	bool bEncroached = true;

	if (!bCrouchMaintainsBaseLocation)
	{
		bEncroached = MyWorld->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

		if (bEncroached)
		{
			if (ScaledHalfHeightAdjust > 0.f)
			{
				float PawnRadius, PawnHalfHeight;
				CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
				const float ShrinkHalfHeight = PawnHalfHeight - PawnRadius;
				const float TraceDist = PawnHalfHeight - ShrinkHalfHeight;
				const FVector Down = FVector(0.f, 0.f, -TraceDist);

				FHitResult Hit(1.f);
				const FCollisionShape ShortCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, ShrinkHalfHeight);
				const bool bBlockingHit = MyWorld->SweepSingleByChannel(Hit, PawnLocation, PawnLocation + Down, FQuat::Identity, CollisionChannel, ShortCapsuleShape, CapsuleParams);
				if (Hit.bStartPenetrating)
				{
					bEncroached = true;
				}
				else
				{
					const float DistanceToBase = (Hit.Time * TraceDist) + ShortCapsuleShape.Capsule.HalfHeight;
					const FVector NewLoc = FVector(PawnLocation.X, PawnLocation.Y, PawnLocation.Z - DistanceToBase + StandingCapsuleShape.Capsule.HalfHeight + SweepInflation + MIN_FLOOR_DIST / 2.f);
					bEncroached = MyWorld->OverlapBlockingTestByChannel(NewLoc, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
					if (!bEncroached)
					{
						UpdatedComponent->MoveComponent(NewLoc - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
					}
				}
			}
		}
	}
	else
	{
		FVector StandingLocation = PawnLocation + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentCrouchedHalfHeight);
		bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

		if (bEncroached)
		{
			if (IsMovingOnGround())
			{
				const float MinFloorDist = KINDA_SMALL_NUMBER * 10.f;
				if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
				{
					StandingLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
					bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
				}
			}
		}

		if (!bEncroached)
		{
			UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
			bForceNextFloorCheck = true;
		}
	}

	if (bEncroached)
	{
		return;
	}

	CachedBaseCharacter->bIsCrawled = false;
	CachedBaseCharacter->bIsCrouched = true;

	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), CrouchedHalfHeight, true);

	const float MeshAdjust = ScaledHalfHeightAdjust;
	AdjustProxyCapsuleSize();
	CachedBaseCharacter->OnEndCrawl(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void UXYZCharacterMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingParameters)
{
	CurrentMantlingParameters = MantlingParameters;

	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);
}

void UXYZCharacterMovementComponent::EndMantle()
{
	SetMovementMode(MOVE_Walking);
}

bool UXYZCharacterMovementComponent::IsMantling() const	
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling;
}

float UXYZCharacterMovementComponent::GetActorToCurrentLadderProjection(const FVector& Location) const
{
	checkf(IsValid(CurrentLadder), TEXT("UXYZCharacterMovementComponent::GetCharacterToCurrentLadderProjection() cannot be invoked when current ladder is null"));
	
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderToCharacterDistance = Location - CurrentLadder->GetActorLocation();
	return  FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
}

void UXYZCharacterMovementComponent::AttachToLadder(const ALadder* Ladder)
{
	CurrentLadder = Ladder;

	FRotator TargetOrientationRotation = CurrentLadder->GetActorForwardVector().ToOrientationRotator();
	TargetOrientationRotation.Yaw += 180.0f;

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderForwardVector = CurrentLadder->GetActorForwardVector();
	float Projection = GetActorToCurrentLadderProjection(GetActorLocation());
	FVector NewCharacterLocation = CurrentLadder->GetActorLocation() + Projection * LadderUpVector + LadderToCharacterOffset * LadderForwardVector;

	if(CurrentLadder->GetIsOnTop())
	{
		NewCharacterLocation = CurrentLadder->GetAttachFromTopAnimMontageStartingLocation();	
	}
	
	GetOwner()->SetActorLocation(NewCharacterLocation);
	GetOwner()->SetActorRotation(TargetOrientationRotation);
	
	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Ladder);
}

void UXYZCharacterMovementComponent::DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod)
{
	switch (DetachFromLadderMethod)
	{
		case EDetachFromLadderMethod::JumpOff:
		{
			FVector JumpDirection = CurrentLadder->GetActorForwardVector();
			SetMovementMode(MOVE_Falling);

			FVector JumpVelocity = JumpDirection * JumpOffFromLadderSpeed;
			ForcedTargetRotation = JumpDirection.ToOrientationRotator();
			bForceRotation = true;
				
			Launch(JumpVelocity);
			break;
		}
		case EDetachFromLadderMethod::ReachingFromTop:
		{
			GetBaseCharacterOwner()->Mantle(true);
			break;
		}
		case EDetachFromLadderMethod::ReachingFromBottom:
		{
			SetMovementMode(MOVE_Walking);
		}
		case EDetachFromLadderMethod::Fall:
		default:
		{
			SetMovementMode(MOVE_Falling);
			break;
		}
	}
}

bool UXYZCharacterMovementComponent::IsOnLadder() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Ladder;
}

void UXYZCharacterMovementComponent::PhysLadder(float DeltaTime, uint32 Iterations)
{
	CalcVelocity(DeltaTime, 1.0f, false, ClimbingOnLadderBrakingDecelartion);
	FVector Delta = Velocity * DeltaTime;

	if(HasAnimRootMotion())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
		return;
	}

	FVector NewPosition = GetActorLocation() + Delta;
	float NewPositionProjection = GetActorToCurrentLadderProjection(NewPosition);

	if(NewPositionProjection < MinLadderBottomOffset)
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingFromBottom);
		return;
	}
	else if(NewPositionProjection > (CurrentLadder->GetLadderHeight() - MaxLadderTopOffset))
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingFromTop);
		return;
	}
	
	FHitResult HitResult;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, HitResult);
}

const ALadder* UXYZCharacterMovementComponent::GetCurrentLadder()
{
	return CurrentLadder;
}

void UXYZCharacterMovementComponent::AttachToZipline(const AZipline* Zipline)
{
	CurrentZipline = Zipline;
	FRotator OrientationRotation = CurrentZipline->GetMoveDirection().ToOrientationRotator();
	FRotator TargetRotation = OrientationRotation;

	if (CurrentZipline->GetMoveDirection().Z < 0.0f)
	{
		TargetRotation = OrientationRotation;
	}
	else if (CurrentZipline->GetMoveDirection().Z > 0.0f)
	{
		TargetRotation = FRotator(OrientationRotation.Pitch, OrientationRotation.Yaw + 180.0f, OrientationRotation.Roll);
	}
	
	const FVector TopPositionToCharacter = GetActorLocation() - CurrentZipline->GetHeightPoleTopPosition();
	const float ProjectionToZipLineDirection = FVector::DotProduct(TopPositionToCharacter, CurrentZipline->GetMoveDirection());
	const FVector AttachPosition = CurrentZipline->GetHeightPoleTopPosition() + ProjectionToZipLineDirection * CurrentZipline->GetMoveDirection();

	GetOwner()->SetActorLocation(AttachPosition + TopOffset);
	GetOwner()->SetActorRotation(TargetRotation);
	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Zipline);
}

void UXYZCharacterMovementComponent::DetachFromZipline(EDetachFromZiplineMethod DetachFromZipline)
{
	switch (DetachFromZipline)
	{
		case EDetachFromZiplineMethod::JumpOff:
		{
			FVector JumpDirection = CurrentZipline->GetActorForwardVector();
			SetMovementMode(MOVE_Falling);

			FVector JumpVelocity = JumpDirection * JumpOffFromZiplineSpeed;
			ForcedTargetRotation = JumpDirection.ToOrientationRotator();
			bForceRotation = true;

			Launch(JumpVelocity);
			break;
		}
		case EDetachFromZiplineMethod::Fall:
		default:
		{
			SetMovementMode(MOVE_Falling);
			break;
		}
	}
}

float UXYZCharacterMovementComponent::GetZiplineSpeed()
{
	return ZiplineSpeed;
}

void UXYZCharacterMovementComponent::PhysZipline(float DeltaTime, uint32 Iterations)
{
	FVector	Delta;
	ZiplineDirection = CurrentZipline->GetMoveDirection();

	if (CurrentZipline->GetMoveDirection().Z > 0.0f)
	{
		Delta = -ZiplineDirection * ZiplineSpeed * DeltaTime;
	}
	else if (CurrentZipline->GetMoveDirection().Z < 0.0f)
	{
		Delta = ZiplineDirection * ZiplineSpeed * DeltaTime;
	}
	
	FHitResult HitResult;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, HitResult);

	FVector ActorLocation = GetActorLocation() - CurrentZipline->GetActorLocation();
	FVector TargetDirection = FVector::ZeroVector;
	
	if (CurrentZipline->GetMoveDirection().Z < 0.0f)
	{	
		TargetDirection = CurrentZipline->SecondPoleBottom  - ActorLocation;	
	}
	else if (CurrentZipline->GetMoveDirection().Z > 0.0f)
	{	
		TargetDirection = CurrentZipline->GetActorLocation() - GetActorLocation();	
	}

	float TargetLength = TargetDirection.Size();

	if (TargetLength < ZiplineDirectionOffset)
	{
		DetachFromZipline(EDetachFromZiplineMethod::Fall);
	}
}

void UXYZCharacterMovementComponent::PhysWallRun(float DeltaTime, uint32 Iterations)
{
	FHitResult HitResult;

	FVector ActorRightVector = GetBaseCharacterOwner()->GetActorRightVector();
	FVector LineTraceDiraction = CurrentWallRunSide == EWallRunSide::Right ? ActorRightVector : -ActorRightVector;
	FVector LIneTraceForwardDirection = GetBaseCharacterOwner()->GetActorForwardVector();
	
	float LineTraceLength = 200.0f;
	float ForwardLineTraceLength = 50.0f;

	FVector StartPosition = GetBaseCharacterOwner()->GetActorLocation();
	FVector EndPosition = StartPosition + LineTraceLength * LineTraceDiraction;
	FVector ForwardEndPosition = StartPosition + ForwardLineTraceLength * LIneTraceForwardDirection;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetBaseCharacterOwner());

	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPosition, ForwardEndPosition, ECC_WallRunnable, QueryParams))
	{
		StopWallRun();
	}

	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPosition, EndPosition, ECC_WallRunnable, QueryParams))
	{
		EWallRunSide Side = EWallRunSide::None;
		FVector Direction = FVector::ZeroVector;

		GetWallRunSideAndDirection(HitResult.ImpactNormal, Side, Direction);

		if (Side != CurrentWallRunSide)
		{
			StopWallRun();
		}
		else
		{
			CurrentWallRunDirection = Direction;
			FVector Delta = GetMaxSpeed() * CurrentWallRunDirection * DeltaTime;

			FRotator TargetOrientationRotation = Direction.ToOrientationRotator();
			GetOwner()->SetActorRotation(TargetOrientationRotation);
			SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, HitResult);
		}
	}
	else
	{
		StopWallRun();
	}
}

bool UXYZCharacterMovementComponent::IsOnZipline() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Zipline;
}

void UXYZCharacterMovementComponent::GetWallRunSideAndDirection(const FVector& HitNormal, EWallRunSide& OutSide, FVector& OutDirection) const
{
	if (FVector::DotProduct(HitNormal, GetBaseCharacterOwner()->GetActorRightVector()) > 0)
	{
		OutSide = EWallRunSide::Left;
		OutDirection = FVector::CrossProduct(HitNormal, FVector::UpVector).GetSafeNormal();
	}
	else
	{
		OutSide = EWallRunSide::Right;
		OutDirection = FVector::CrossProduct(FVector::UpVector, HitNormal).GetSafeNormal();
	}
}

void UXYZCharacterMovementComponent::StartWallRun(EWallRunSide Side, const FVector& Direction)
{
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_WallRun);

	CurrentWallRunSide = Side;
	PreviousWallRunSide = Side;
	CurrentWallRunDirection = Direction;

	SetPlaneConstraintEnabled(true);
	SetPlaneConstraintNormal(FVector::UpVector);
}

void UXYZCharacterMovementComponent::StopWallRun()
{
	SetMovementMode(MOVE_Walking);
	
	PreviousWallRunSide = CurrentWallRunSide;
	CurrentWallRunSide = EWallRunSide::None;
	CurrentWallRunDirection = FVector::ZeroVector;

	SetPlaneConstraintNormal(FVector::ZeroVector);
}

bool UXYZCharacterMovementComponent::IsWallRunning() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_WallRun;
}

bool UXYZCharacterMovementComponent::CanWallRun(const FVector& HitNormal, EWallRunSide Side) const
{
	if (IsWallRunning())
	{
		return false;
	}

	if (!IsFalling())
	{
		return false;
	}

	if (!IsSurfaceWallRunnable(HitNormal))
	{
		return false;
	}

	if (Side == PreviousWallRunSide)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("CurrentWallRunSide == PreviousWallRunSide"));
		return false;
	}

	return true;
}

bool UXYZCharacterMovementComponent::IsSurfaceWallRunnable(const FVector& SurfaceNormal) const
{
	if (SurfaceNormal.Z > GetWalkableFloorZ() || SurfaceNormal.Z < -0.005f)
	{
		return false;
	}
	return true;
}

void UXYZCharacterMovementComponent::PhysMantling(float DeltaTime, uint32 Iterations)
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParameters.StartTime;

	FVector MantlingCurveValue = CurrentMantlingParameters.MantlingCurve->GetVectorValue(ElapsedTime);

	float PositionAlpha = MantlingCurveValue.X;
	float XYCorrectionLapha = MantlingCurveValue.Y;
	float ZCorrectionAlpha = MantlingCurveValue.Z;

	FVector CorrectedInitialLocation = FMath::Lerp(CurrentMantlingParameters.InitialLocation, CurrentMantlingParameters.InitialAnimationLocation, XYCorrectionLapha);
	CorrectedInitialLocation.Z = FMath::Lerp(CurrentMantlingParameters.InitialLocation.Z, CurrentMantlingParameters.InitialAnimationLocation.Z, ZCorrectionAlpha);
		
	FVector NewLocation = FMath::Lerp(CorrectedInitialLocation, CurrentMantlingParameters.TargetLocation, PositionAlpha);
	FRotator NewRotation = FMath::Lerp(CurrentMantlingParameters.InitialRotation, CurrentMantlingParameters.TargetRotation, PositionAlpha);

	FVector Delta = NewLocation - GetActorLocation() + CurrentMantlingParameters.TargetComponent->GetComponentLocation();

	FHitResult HitResult;
	SafeMoveUpdatedComponent(Delta, NewRotation, false, HitResult);
}

void UXYZCharacterMovementComponent::StartSlide()
{
	bIsSliding = true;

	float ComponentScale = GetBaseCharacterOwner()->GetCapsuleComponent()->GetShapeScale();
	float OldUnscaledHalfHeight = GetBaseCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	float HalfHeightAdjust = OldUnscaledHalfHeight - SlidingCapsuleHalfHeight;
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	GetBaseCharacterOwner()->GetCapsuleComponent()->SetCapsuleSize(SlidingCapsuleRadius, SlidingCapsuleHalfHeight);

	UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);

	GetBaseCharacterOwner()->OnStartSlide(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void UXYZCharacterMovementComponent::StopSlide()
{
	bIsSliding = false;

	if (GetBaseCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() != CrouchedHalfHeight)
	{
		bWantsToCrouch = true;
		bWantsToCrawl = true;

		GetBaseCharacterOwner()->OnEndSlide(0.f, 0.f);
		return;
	}

	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();

	const float DefaultCapsuleHalfHeight = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float CurrentSlidingHalfHeight = GetBaseCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float ComponentScale = GetBaseCharacterOwner()->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = GetBaseCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = DefaultCapsuleHalfHeight - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	const UWorld* MyWorld = GetWorld();
	const float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleParams, ResponseParam);

	const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust);
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

	FVector StandingLocation = PawnLocation - FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentSlidingHalfHeight);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	FCollisionShape CollectionShape = FCollisionShape::MakeCapsule(SlidingCapsuleRadius, DefaultCapsuleHalfHeight);
	FVector StartPosition = GetBaseCharacterOwner()->GetActorLocation();
	StartPosition.Z += HalfHeightAdjust;
	FVector EndPosition = StartPosition + FVector(0.0f, 0.0f, DefaultCapsuleHalfHeight);

	DrawDebugCapsule(GetWorld(), StartPosition, DefaultCapsuleHalfHeight, SlidingCapsuleRadius, FQuat::Identity, FColor::Red, false, 2.0f);

	if (GetWorld()->SweepSingleByChannel(HitResult, StartPosition, EndPosition, FQuat::Identity, ECC_Visibility, CollectionShape, QueryParams, ResponseParam))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Hit!"));
		bWantsToCrouch = true;
	}
	
	GetBaseCharacterOwner()->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCapsuleHalfHeight);

	UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);

	GetBaseCharacterOwner()->OnEndSlide(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool UXYZCharacterMovementComponent::IsSliding() const
{
	return bIsSliding;
}

void UXYZCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
		
		case(uint8)ECustomMovementMode::CMOVE_Mantling:
		{
			PhysMantling(DeltaTime, Iterations);
			break;
		}
		case (uint8)ECustomMovementMode::CMOVE_Ladder:
		{
			PhysLadder(DeltaTime, Iterations);
			break;
		}	
		case (uint8)ECustomMovementMode::CMOVE_Zipline:
		{
			PhysZipline(DeltaTime, Iterations);
			break;
		}
		case (uint8)ECustomMovementMode::CMOVE_WallRun:
		{
			PhysWallRun(DeltaTime, Iterations);
			break;
		}
		default:
		{
			break;	
		}		
	}

	Super::PhysCustom(DeltaTime, Iterations);
}



void UXYZCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (MovementMode == MOVE_Falling)
	{
		bNotifyApex = true;
	}

	if(MovementMode == MOVE_Swimming)
	{
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(SwimingCapsuleRadius, SwimingCapsuleHalfHeight);
	}
	else if(PreviousMovementMode == MOVE_Swimming)
	{
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), true);
	}

	if(PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Ladder)
	{
		CurrentLadder = nullptr;
	}

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Zipline)
	{
		CurrentZipline = nullptr;
	}

	if (MovementMode != MOVE_Falling && !IsWallRunning())
	{
		PreviousWallRunSide = EWallRunSide::None;
	}

	if (MovementMode == MOVE_Custom)
	{
		switch (CustomMovementMode)
		{
			case (uint8)ECustomMovementMode::CMOVE_Mantling:
			{
				GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UXYZCharacterMovementComponent::EndMantle, CurrentMantlingParameters.Duration, false);
				break;
			}
			case(uint8)ECustomMovementMode::CMOVE_WallRun:
			{
				GetWorld()->GetTimerManager().SetTimer(WallRunTimer, this, &UXYZCharacterMovementComponent::StopWallRun, MaxWallRunTime, false);
				break;
			}
			default:
			{
				break;	
			}
				
		}
	}
}

AXYZBaseCharacter* UXYZCharacterMovementComponent::GetBaseCharacterOwner() const
{
	return StaticCast<AXYZBaseCharacter*>(CharacterOwner);
}




