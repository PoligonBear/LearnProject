// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/XYZProjectile.h"
#include "Components/SceneComponent.h"
#include "WeaponBarellComponent.generated.h"

UENUM(BlueprintType)
enum class EHitRegistrationType : uint8
{
	HitScan,
	Projectile
};

USTRUCT(BlueprintType)
struct FDecalInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	UMaterialInterface* DecalMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	FVector DecalSize = FVector(5.0f, 5.0f, 5.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	float DecalLifeTime = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	float DecalFadeOutTime = 5.0f;
};

class UNiagaraSystem;
class UCurveFloat;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XYZ_HOMEWORK_API UWeaponBarellComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	
	void Shot(FVector ShotStart, FVector ShotDirection, AController* Controller, float SpreadAngle);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell atributes")
	float FiringRange = 5000.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell atributes", meta = (ClampMin = 1, UIMin = 1))
	int32 BulletsPerShot = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell atributes | Hit registration")
	EHitRegistrationType HitRegistration = EHitRegistrationType::HitScan;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell atributes | Hit registration" , meta = (EditCondition = "HitRegistration == EHitRegistrationType::Projectile"))
	TSubclassOf<class AXYZProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell atributes | Damage")
	float DamageAmount = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell atributes | Damage")
	TSubclassOf<class UDamageType> DamageTypeClass;

	// Damage depending from distance  (in meters)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell atributes | Damage")
	UCurveFloat* FalloffDiagramCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell atributes | VFX")
	UNiagaraSystem* MuzzleFlashFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell atributes | VFX")
	UNiagaraSystem* TraceFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell atributes | Decals")
	FDecalInfo DefaultDecalInfo;

private:
	APawn* GetOwningPawn() const;
	AController* GetController() const;
	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const;

	bool HitScan(FVector ShotStart, FVector ShotDirection, FVector& ShotEnd);
	void LaunchProjectile(const FVector& LaunchStart, const FVector& LaunchDirection);
};
