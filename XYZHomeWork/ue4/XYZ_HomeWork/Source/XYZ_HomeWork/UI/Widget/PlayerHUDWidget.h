// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmmoWidget.h"
#include "ReticleWidget.h"
#include "CharacterAttributesWidget.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

UCLASS()
class XYZ_HOMEWORK_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	class UReticleWidget* GetReticleWidget();
	class UAmmoWidget* GetAmmoWidget();
	class UCharacterAttributesWidget* GetCharacterAttributesWidget();
protected:
	//UFUNCTION(BlueprintCallable)
	//float GetHealthPercent() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widge Names")
	FName ReticleWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widge Names")
	FName AmmoWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widge Names")
	FName CharacterAttributesWidgetName;
};
