// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PlatformInvokator.generated.h"

UCLASS()
class XYZ_HOMEWORK_API APlatformInvokator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlatformInvokator();

	UPROPERTY(EditInstanceOnly, Category = "Platforms")
	class ABasePlatform* Platforms = nullptr;

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* BoxCollision = nullptr;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};