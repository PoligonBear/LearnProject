// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/XYZProjectile.h"
#include "ExplosiveProjectile.generated.h"

class UExplosionComponent;

UCLASS()
class XYZ_HOMEWORK_API AExplosiveProjectile : public AXYZProjectile
{
	GENERATED_BODY()

public:
	AExplosiveProjectile();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UExplosionComponent* ExplosionComponent = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Explosion")
	float DetonaionTime = 2.0f;

	virtual  void OnProjectileLaunched() override;
private:
	void OnDetonationTimerElapsed();

	AController* GetController();
	
	FTimerHandle DetonationTimer;
};
