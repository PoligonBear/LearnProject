// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "ThrowableItem.generated.h"

UCLASS(Blueprintable)
class XYZ_HOMEWORK_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()

public:
	void Throw();

	EAmunitionType GetAmmoType() const;

	int32 ThrowableItemCount = 0; 

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwbles")
	TSubclassOf<class AXYZProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Throwbles", meta = (UIMin = -90.0f, UIMax = 90.0f, ClampMin = -90.0f, ClampMax = 90.0f))
	float ThrowAngle = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwbles | Parameters | Ammo")
	EAmunitionType AmmoType;
};
