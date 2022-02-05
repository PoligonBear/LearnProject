// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "RangeWeaponItem.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnReloadComplete);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoChanged, int32);

UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
{
	Single,
	FullAuto
};
UENUM(BlueprintType)
enum class EReloadType : uint8
{
	FullClip,
	ByBullet
};

class UAnimMontage;
UCLASS(Blueprintable)
class XYZ_HOMEWORK_API ARangeWeaponItem : public AEquipableItem
{
	GENERATED_BODY()
	
public:
	ARangeWeaponItem();

	void StartFire();
	void StopFire();

	void StartAim();
	void StopAim();

	void StartReload();
	void EndReload(bool bIsSuccess);

	float GetAimFOV() const;
	float GetAimMovementMaxSpeed() const;

	float GetAimTurnModifier() const;
	float GetAimLookUpModifier() const;

	int32 GetAmmo() const;
	int32 GetMaxAmmo() const;
	void SetAmmo(int32 NewAmmo);
	EAmunitionType GetAmmoType() const;

	bool CanShoot() const;
	
	FTransform GetForeGripTransform() const;

	FOnAmmoChanged OnAmmoChanged;
	FOnReloadComplete OnReloadComplete;
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* WeaponMesh = nullptr; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWeaponBarellComponent* WeaponBarell = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
	UAnimMontage* WeaponFireMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
	UAnimMontage* WeaponReloadMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Character")
	UAnimMontage* CharacterFireMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Character")
	UAnimMontage* CharacterReloadMontage = nullptr;

	// Rate of fire in rounds per minute
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters", meta = (ClampMin = 1.0f, UIMin = 1.0f))
	float RateOfFire = 600.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters")
	EWeaponFireMode WeaponFireMode = EWeaponFireMode::Single;

	// FullClip reload type adds ammo only when the whole reload animation is successfully played
	// ByBullet reload type requires section "reloadEnd" in character reload animation 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters")
	EReloadType ReloadType = EReloadType::FullClip;
	
	// Bullet spread half angle in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 2.0f, UIMax = 2.0f))
	float SpreadAngle = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 1.0f, UIMax = 1.0f))
	float AimTurnModifier = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 1.0f, UIMax = 1.0f))
	float AimLookUpModifier = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 2.0f, UIMax = 2.0f))
	float AimSpreadAngle = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AimMovementMaxSpeed = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 120.0f, UIMax = 120.0f))
	float AimFOV = 60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Ammo", meta = (ClampMin = 1, UIMin = 1))
	int32 MaxAmmo = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Ammo", meta = (ClampMin = 1, UIMin = 1))
	bool bAutoReload = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Ammo")
	EAmunitionType AmmoType;
private:
	int32 Ammo = 0;

	bool bIsReloading = false;
	bool bIsAiming = false;
	bool bIsFiring = false;
	
	void MakeShot();
	void OnShotTimerElapsed();

	float GetCurrentBulletSpreadAngle() const;
	float GetShotTimerInterval() const;
	float PlayAnimMontage(UAnimMontage* AnimMontage);

	void StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime = 0.0f);
	
	FTimerHandle ShotTimer;
	FTimerHandle ReloadTimer;
};
