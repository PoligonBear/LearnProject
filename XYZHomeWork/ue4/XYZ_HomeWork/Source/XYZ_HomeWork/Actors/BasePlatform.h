// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "BasePlatform.generated.h"

UENUM(BlueprintType)
enum class EMovementType : uint8

{
	OnDeamnd = 0,
	Loop
};

UCLASS()
class XYZ_HOMEWORK_API ABasePlatform : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABasePlatform();

	UFUNCTION()
	void TimelineFloatReturn(float Value);

	UFUNCTION(BlueprintCallable)
	void MovePlatform();

	UFUNCTION()
	void OnTimelineFinished();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform settings")
	EMovementType MovementType = EMovementType::OnDeamnd;

	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform settings")
	UStaticMeshComponent* PlatformMeshComponent = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget), Category = "Platform settings")
	FVector EndLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Platform settings")
	FVector StartLocation = FVector::ZeroVector;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Platform settings")
	UCurveFloat* PlatformMovementCurve = nullptr;

	FTimeline PlatformMovementTimeline;
	FTimerHandle PlatformMovementTimerHandle;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Platform settings")
	float CooldownTimer = 2.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Platform settings")
	float ForwardSpeedRate = 2.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Platform settings")
	float BackwardSpeedRate = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform settings")
	bool bReverse = false;
};