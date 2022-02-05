// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeathEventSignature)
DECLARE_MULTICAST_DELEGATE_OneParam(FOutOfStaminaSignature, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateHealthPercent, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateStaminaPercent, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateOxygenPercent, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XYZ_HOMEWORK_API UCharacterAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnDeathEventSignature OnDeathEvent;
	FOutOfStaminaSignature OutOfStamina;
	FOnUpdateHealthPercent OnUpdateHealthPercent;
	FOnUpdateStaminaPercent OnUpdateStaminaPercent;
	FOnUpdateOxygenPercent OnUpdateOxygenPercent;

	float AttributesPercentValue(float CurrentValue, float MaxValue);
	float CurrentHealthPercent = 1.0f;
	float CurrentStaminaPercent = 1.0f;
	float CurrentOxygenPercent = 1.0f;

	bool bIsOutOfStamina = false;
	bool IsALive() { return Health > 0.0f; }
	bool IsStaminaLessOfMax() const;
	bool IsOxygenLessOfMax() const;

	void UpdateCurrentHealthPercent(float NewHealthPercent);
	void UpdateCurrentStaminaPercent(float NewStaminaPercent);
	void UpdateCurrentOxygenPercent(float NewOxygenPercent);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void UpdateStaminaValue(float DeltaTime);
	void UpdateOxygenValue(float DeltaTime);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (UIMin = 0.0f))
	float MaxHealth = 100.0f;

	/** Stamina **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina", meta = (UIMin = 0.0f))
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina", meta = (UIMin = 0.0f))
	float StaminaRestoreVelocity = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina", meta = (UIMin = 0.0f))
	float SprintStaminaConsumptionVelocity = 30.0f;

	/** Oxygen **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oxygen", meta = (UIMin = 0.0f))
	float MaxOxygen = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oxygen", meta = (UIMin = 0.0f))
	float OxygenRestoreVelocity = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oxygen", meta = (UIMin = 0.0f))
	float SwimOxygenConsumptionVelocity = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oxygen", meta = (UIMin = 0.0f))
	float OxygenOverDamage = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oxygen", meta = (UIMin = 0.0f))
	float OxygenDamageInterval = 2.0f;

private:
	float Health = 0.0f;
	float Stamina = 0.0f;
	float Oxygen = 0.0f;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	void DebugDrawAttributes();
#endif

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	TWeakObjectPtr<class AXYZBaseCharacter> CachedBaseCharacterOwner;
};
