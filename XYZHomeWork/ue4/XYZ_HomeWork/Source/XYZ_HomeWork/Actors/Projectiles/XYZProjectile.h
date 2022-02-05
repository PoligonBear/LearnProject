// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "XYZProjectile.generated.h"

UCLASS()
class XYZ_HOMEWORK_API AXYZProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AXYZProjectile();

	UFUNCTION(BlueprintCallable)
	void LaunchProjectile(FVector Direction);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent = nullptr;
	
	virtual void OnProjectileLaunched();
};
