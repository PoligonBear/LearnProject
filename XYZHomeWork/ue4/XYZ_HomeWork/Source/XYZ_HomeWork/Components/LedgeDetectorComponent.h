// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LedgeDetectorComponent.generated.h"

USTRUCT(BlueprintType)
struct FLedgeDescription
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge description")
	FVector Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge description")
	FVector LedgeNormal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge description")
	FRotator Rotaion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge description")
	float TargetHeight;

	class UPrimitiveComponent* TargetComponentHit = nullptr;
};
 
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XYZ_HOMEWORK_API ULedgeDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	bool DetectLedge(OUT FLedgeDescription& LedgeDescription);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category ="Detection settings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float MinimunLedgeHeight = 40.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection settings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float MaximunLedgeHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection settings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float ForwardCheckDistance = 100.0f;

	float BaseCapsuleHalfHeight;

private:
	TWeakObjectPtr<class ACharacter> CachedCharaterOwner;
	class UCapsuleComponent* CapsuleComponent = nullptr;
};
