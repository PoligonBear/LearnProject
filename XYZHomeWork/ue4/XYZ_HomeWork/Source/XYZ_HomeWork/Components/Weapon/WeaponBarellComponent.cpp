// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBarellComponent.h"
#include "XYZ_HomeWork_Types.h"
#include "DrawDebugHelpers.h"
#include "Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/DecalComponent.h"
#include "Actors/Projectiles/XYZProjectile.h"

bool UWeaponBarellComponent::HitScan(FVector ShotStart, FVector ShotDirection, FVector& ShotEnd)
{
	FHitResult ShotResult;
	bool bHasHit = GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet);
	if (bHasHit)
	{
		ShotEnd = ShotResult.ImpactPoint;

		AActor* HitActor = ShotResult.GetActor();

		

		if (IsValid(HitActor))
		{
			FPointDamageEvent DamageEvent;
			DamageEvent.HitInfo = ShotResult;
			DamageEvent.ShotDirection = ShotDirection;
			DamageEvent.DamageTypeClass = DamageTypeClass;
				
			float DamageCoefficient = 1.0f;
			float ShotDistance = (ShotEnd - GetComponentLocation()).Size();

			if (IsValid(FalloffDiagramCurve))
			{
				DamageCoefficient = FalloffDiagramCurve->GetFloatValue(ShotDistance);
			}
			
			float Damage = DamageCoefficient * DamageAmount;

			HitActor->TakeDamage(Damage * DamageCoefficient, DamageEvent, GetController(), GetOwner());	
		}

		UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecalInfo.DecalMaterial, DefaultDecalInfo.DecalSize, ShotResult.ImpactPoint, ShotResult.ImpactNormal.ToOrientationRotator());

		if (IsValid(DecalComponent))
		{
			DecalComponent->SetFadeScreenSize(0.001f);
			DecalComponent->SetFadeOut(DefaultDecalInfo.DecalLifeTime, DefaultDecalInfo.DecalFadeOutTime);
		}
	}

	return bHasHit;
}

void UWeaponBarellComponent::LaunchProjectile(const FVector& LaunchStart, const FVector& LaunchDirection)
{
	AXYZProjectile* Projectile = GetWorld()->SpawnActor<AXYZProjectile>(ProjectileClass, LaunchStart, LaunchDirection.ToOrientationRotator());

	if(IsValid(Projectile))
	{
		Projectile->SetOwner(GetOwningPawn());
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	}
}

void UWeaponBarellComponent::Shot(FVector ShotStart, FVector ShotDirection, AController* Controller, float SpreadAngle)
{
	FVector MuzzleLocation = GetComponentLocation();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLocation, GetComponentRotation());

	for(int i = 0; i < BulletsPerShot; i++)
	{
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.0f, SpreadAngle), ShotDirection.ToOrientationRotator());
		FVector ShotEnd = ShotStart + FiringRange * ShotDirection;

	#if ENABLE_DRAW_DEBUG
		UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
		bool bIsDebugEnable = DebugSubsystem->IsCategoryEnabled(DebugCategoryRangeWeapon);
	#else
		bool bIsDebugEnable = false;
	#endif

		switch (HitRegistration)
		{
		case EHitRegistrationType::HitScan:
			{				
				bool bHasHit = HitScan(ShotStart, ShotDirection, ShotEnd);

				if (bIsDebugEnable && bHasHit)
				{
					DrawDebugSphere(GetWorld(), ShotEnd, 10.0f, 24, FColor::Red, false, 1.0f);
				}
			}
		case EHitRegistrationType::Projectile:
			{
				LaunchProjectile(ShotStart, ShotDirection);
				break;
			}
		}
		
		

		UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, MuzzleLocation, GetComponentRotation());
		TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);

		if (bIsDebugEnable)
		{
			DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
		}	
	}
}

APawn* UWeaponBarellComponent::GetOwningPawn() const
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if(IsValid(PawnOwner))
	{
		PawnOwner = Cast<APawn>(GetOwner()->GetOwner());
	}

	return PawnOwner;
}

AController* UWeaponBarellComponent::GetController() const
{
	APawn* PawnOwner = GetOwningPawn();
	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}

FVector UWeaponBarellComponent::GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.0f, 2 * PI);
	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);

	FVector Result = (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ
					+ ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;

	return Result;
}

