// Fill out your copyright notice in the Description page of Project Settings.


#include "LedgeDetectorComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "../XYZ_HomeWork_Types.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../Utils/XYZTraceUtils.h"
#include "../Characters/XYZBaseCharacter.h"
#include "../XYZGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../Subsystems/DebugSubsystem.h"

void ULedgeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(GetOwner()->IsA<ACharacter>(), TEXT("ULedgeDetectorComponent::BeginPlay() only a character can use ULedgeDetectorComponent"));
	
	CachedCharaterOwner = StaticCast<ACharacter*>(GetOwner());
	CapsuleComponent = CachedCharaterOwner->GetCapsuleComponent();
	BaseCapsuleHalfHeight = CapsuleComponent->GetUnscaledCapsuleHalfHeight();
}

bool ULedgeDetectorComponent::DetectLedge(OUT FLedgeDescription& LedgeDescription)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());

	UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubSystem->IsCategoryEnabled(DebugCategoryLedgeDetection);
	float DrawTime = 5.0f;

	float BottomZOffset = 20.0f;
	FVector CharacterBottom = CachedCharaterOwner->GetActorLocation() - (BaseCapsuleHalfHeight - BottomZOffset) * FVector::UpVector;
	
	// 1. Forward check
	float ForwardCheckCapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	float ForwardCheckCapsuleHalfHeight = (MaximunLedgeHeight - MinimunLedgeHeight) * 0.5f;

	FHitResult ForwardCheckHitResult;

	FVector ForwardStartLocation = CharacterBottom + (MinimunLedgeHeight + ForwardCheckCapsuleHalfHeight) * FVector::UpVector;
	FVector ForwardEndLocation = ForwardStartLocation + CachedCharaterOwner->GetActorForwardVector() * ForwardCheckDistance;

	if (!XYZTraceUtils::SweepCapsuleSingleByChannel(GetWorld(), ForwardCheckHitResult, ForwardStartLocation, ForwardEndLocation, ForwardCheckCapsuleRadius, ForwardCheckCapsuleHalfHeight, FQuat::Identity, ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, DrawTime))
	{
		return false;
	}

	// 2. Downward check
	float DownwardSphereCheckRadius = CapsuleComponent->GetScaledCapsuleRadius();
	float DownwardCheckDepthOffset = 5.0f;

	FHitResult DownwardCheckHitResult;

	FVector DownwardStartLocation = ForwardCheckHitResult.ImpactPoint - ForwardCheckHitResult.ImpactNormal * DownwardCheckDepthOffset;
	DownwardStartLocation.Z = CharacterBottom.Z + MaximunLedgeHeight + DownwardSphereCheckRadius;
	FVector DownwardEndLocation(DownwardStartLocation.X, DownwardStartLocation.Y, CharacterBottom.Z);

	if (!XYZTraceUtils::SweepSphereSingleByChannel(GetWorld(), DownwardCheckHitResult, DownwardStartLocation, DownwardEndLocation, DownwardSphereCheckRadius, ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, DrawTime))
	{
		return false;
	}

	// 3. Overlap check
	float OverlapCapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	float OverlapCapsuleHalfHeight = BaseCapsuleHalfHeight;
	float OverlapCapsuleFloorOffset = 2.0f;

	FVector OverlapLocation = DownwardCheckHitResult.ImpactPoint + (OverlapCapsuleHalfHeight + OverlapCapsuleFloorOffset) * FVector::UpVector;

	if (XYZTraceUtils::OverlapCapsuleBlockingByProfile(GetWorld(), OverlapLocation, OverlapCapsuleRadius, OverlapCapsuleHalfHeight, FQuat::Identity, CollisionProfilePawn, QueryParams, bIsDebugEnabled, DrawTime))
	{
		return false;
	}
	
	LedgeDescription.TargetComponentHit = ForwardCheckHitResult.GetComponent();
	LedgeDescription.TargetHeight = (DownwardCheckHitResult.ImpactPoint + BottomZOffset).Z;
	LedgeDescription.Location = OverlapLocation;
	LedgeDescription.Rotaion = (ForwardCheckHitResult.ImpactNormal * FVector(-1.0f, -1.0f, 0.0f)).ToOrientationRotator();
	LedgeDescription.LedgeNormal = ForwardCheckHitResult.ImpactNormal;

	return true;
}
