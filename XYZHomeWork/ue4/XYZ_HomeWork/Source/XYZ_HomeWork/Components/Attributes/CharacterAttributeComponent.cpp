// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributeComponent.h"
#include "../../Characters/XYZBaseCharacter.h"
#include "../../Subsystems/DebugSubsystem.h"
#include "DrawDebugHelpers.h"
#include "../../XYZ_HomeWork_Types.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "../MovementComponents/XYZCharacterMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"

// Sets default values for this component's properties
UCharacterAttributeComponent::UCharacterAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateStaminaValue(DeltaTime);
	UpdateOxygenValue(DeltaTime);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif
}

float UCharacterAttributeComponent::AttributesPercentValue(float CurrentValue, float MaxValue)
{
	return CurrentValue / MaxValue;
}

bool UCharacterAttributeComponent::IsStaminaLessOfMax() const
{
	if (Stamina == MaxStamina)
	{
		return false;
	}
	return true;
}

bool UCharacterAttributeComponent::IsOxygenLessOfMax() const
{
	if (Oxygen == MaxOxygen)
	{
		return false;
	}
	return true;
}

void UCharacterAttributeComponent::UpdateCurrentHealthPercent(float NewHealthPercent)
{
	if (OnUpdateHealthPercent.IsBound())
	{
		OnUpdateHealthPercent.Broadcast(CurrentHealthPercent);
	}
}

void UCharacterAttributeComponent::UpdateCurrentStaminaPercent(float NewStaminaPercent)
{
	if (OnUpdateStaminaPercent.IsBound())
	{
		OnUpdateStaminaPercent.Broadcast(CurrentStaminaPercent);
	}
}

void UCharacterAttributeComponent::UpdateCurrentOxygenPercent(float NewOxygenPercent)
{
	if (OnUpdateOxygenPercent.IsBound())
	{
		OnUpdateOxygenPercent.Broadcast(CurrentOxygenPercent);
	}
}

void UCharacterAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(MaxHealth > 0.0f, TEXT("UCharacterAttributeComponent::BeginPlay max health cannot be equal to 0"));
	checkf(GetOwner()->IsA<AXYZBaseCharacter>(), TEXT("UCharacterAttributeComponent::BeginPlay UCharacterAttributeComponent can be used  only with AXYZBaseCharacter"));
	CachedBaseCharacterOwner = StaticCast<AXYZBaseCharacter*>(GetOwner());

	Health = MaxHealth;
	Stamina = MaxStamina;
	Oxygen = MaxOxygen;

	CurrentHealthPercent = AttributesPercentValue(Health, MaxHealth);
	CurrentStaminaPercent = AttributesPercentValue(Stamina, MaxStamina);
	CurrentOxygenPercent = AttributesPercentValue(Oxygen, MaxOxygen);
	UpdateCurrentHealthPercent(CurrentHealthPercent);

	CachedBaseCharacterOwner->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributeComponent::OnTakeDamage);
}

void UCharacterAttributeComponent::UpdateStaminaValue(float DeltaTime)
{
	if (CachedBaseCharacterOwner->GetXYZCharacterMovementComponent()->IsSprinting())
	{
		Stamina -= SprintStaminaConsumptionVelocity * DeltaTime;
		CurrentStaminaPercent = AttributesPercentValue(Stamina, MaxStamina);
		UpdateCurrentStaminaPercent(CurrentStaminaPercent);
		if (Stamina <= 0.0f && !CachedBaseCharacterOwner->GetXYZCharacterMovementComponent()->bIsOutOfStamina)
		{
			if (OutOfStamina.IsBound())
			{
				OutOfStamina.Broadcast(bIsOutOfStamina = true);
			}
		}
	}
	else
	{
		Stamina += SprintStaminaConsumptionVelocity * DeltaTime;
		Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
		CurrentStaminaPercent = AttributesPercentValue(Stamina, MaxStamina);
		UpdateCurrentStaminaPercent(CurrentStaminaPercent);
		if (Stamina == MaxStamina && CachedBaseCharacterOwner->GetXYZCharacterMovementComponent()->bIsOutOfStamina)
		{
			if (OutOfStamina.IsBound())
			{
				OutOfStamina.Broadcast(bIsOutOfStamina = false);
			}
		}
	}
}

void UCharacterAttributeComponent::UpdateOxygenValue(float DeltaTime)
{
	if (!IsALive())
	{
		return;
	}

	FVector HeadPosition = CachedBaseCharacterOwner->GetMesh()->GetSocketLocation(FName("HeadSocket"));

	APhysicsVolume* Volume = CachedBaseCharacterOwner->GetCharacterMovement()->GetPhysicsVolume();
	float VolumeTopPlane = Volume->GetActorLocation().Z + Volume->GetBounds().BoxExtent.Z * Volume->GetActorScale3D().Z;

	if (CachedBaseCharacterOwner->IsSwimingUnderWater(HeadPosition, VolumeTopPlane))
	{
		Oxygen -= SwimOxygenConsumptionVelocity * DeltaTime;
		Oxygen = FMath::Clamp(Oxygen, 0.0f, MaxOxygen);
		CurrentOxygenPercent = AttributesPercentValue(Oxygen, MaxOxygen);
		UpdateCurrentOxygenPercent(CurrentOxygenPercent);
		if (Oxygen <= 0.0f)
		{
			Health -= (OxygenOverDamage * OxygenDamageInterval) * DeltaTime;
			Health = FMath::Clamp(Health, 0.0f, MaxHealth);

			if (Health <= 0.0f)
			{
				if (OnDeathEvent.IsBound())
				{
					OnDeathEvent.Broadcast();
				}
			}
		}
	}
	else
	{
		Oxygen += OxygenRestoreVelocity * DeltaTime;
		Oxygen = FMath::Clamp(Oxygen, 0.0f, MaxOxygen);
		CurrentOxygenPercent = AttributesPercentValue(Oxygen, MaxOxygen);
		UpdateCurrentOxygenPercent(CurrentOxygenPercent);
	}
}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UCharacterAttributeComponent::DebugDrawAttributes()
{
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	if (!DebugSubsystem->IsCategoryEnabled(DebugCategoryCharacterAttributes))
	{
		return;
	}

	FVector HealthLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 25.0f) * FVector::UpVector;
	FVector StaminaLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 15.0f) * FVector::UpVector;
	FVector OxygenLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 5.0f) * FVector::UpVector;
	DrawDebugString(GetWorld(), HealthLocation, FString::Printf(TEXT("Health: %.2f"), Health), nullptr, FColor::Green, 0.0f, true);
	DrawDebugString(GetWorld(), StaminaLocation, FString::Printf(TEXT("Stamina: %.2f"), Stamina), nullptr, FColor::Green, 0.0f, true);
	DrawDebugString(GetWorld(), OxygenLocation, FString::Printf(TEXT("Oxygen: %.2f"), Oxygen), nullptr, FColor::Green, 0.0f, true);
}
#endif

void UCharacterAttributeComponent::OnTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsALive())
	{
		return;
	}

	UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponent::OnTakeDamage %s recived  %.2f amount of damage from  %s"), *CachedBaseCharacterOwner->GetName(), Damage, *DamageCauser->GetName());

	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	CurrentHealthPercent = AttributesPercentValue(Health, MaxHealth);
	UpdateCurrentHealthPercent(CurrentHealthPercent);
	if (Health <= 0.0f)
	{
		UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponent::OnTakeDamage character %s is killed by an actor %s"), *CachedBaseCharacterOwner->GetName(), *DamageCauser->GetName());
		if (OnDeathEvent.IsBound())
		{
			OnDeathEvent.Broadcast();
		}
	}
}




