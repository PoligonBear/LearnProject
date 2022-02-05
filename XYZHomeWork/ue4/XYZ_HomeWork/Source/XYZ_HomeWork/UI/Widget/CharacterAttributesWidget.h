// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterAttributesWidget.generated.h"

/**
 * 
 */
UCLASS()
class XYZ_HOMEWORK_API UCharacterAttributesWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float CurrentHealthPercent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float CurrentStaminaPercent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float CurrentOxygenPercent;
	
private:
	class AXYZBaseCharacter* CachedBaseCharacter;

	UFUNCTION(BlueprintCallable)
	void UpdateHealthPercent(float HealthPercent);

	UFUNCTION(BlueprintCallable)
	void UpdateStaminaPercent(float StaminaPercent);

	UFUNCTION(BlueprintCallable)
	void UpdateOxygenPercent(float OxygenPercent);

	UFUNCTION(BlueprintCallable)
	ESlateVisibility StaminaVisibility();

	UFUNCTION(BlueprintCallable)
	ESlateVisibility OxygenVisibility();
};
