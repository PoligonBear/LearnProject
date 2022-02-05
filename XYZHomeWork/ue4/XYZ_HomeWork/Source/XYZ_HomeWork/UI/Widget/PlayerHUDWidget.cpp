// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"
#include "ReticleWidget.h"
#include "AmmoWidget.h"
#include "CharacterAttributesWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Characters/XYZBaseCharacter.h"
#include "Components/Attributes/CharacterAttributeComponent.h"

UReticleWidget* UPlayerHUDWidget::GetReticleWidget()
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}

UAmmoWidget* UPlayerHUDWidget::GetAmmoWidget()
{
	return  WidgetTree->FindWidget<UAmmoWidget>(AmmoWidgetName);
}

UCharacterAttributesWidget* UPlayerHUDWidget::GetCharacterAttributesWidget()
{
	return WidgetTree->FindWidget<UCharacterAttributesWidget>(CharacterAttributesWidgetName);
}

/*
float UPlayerHUDWidget::GetHealthPercent() const
{
	float Result = 1.0f;
	APawn* Pawn = GetOwningPlayerPawn();
	AXYZBaseCharacter* Character = Cast<AXYZBaseCharacter>(Pawn);

	if(IsValid(Character))
	{
		const UCharacterAttributeComponent* CharacterAttributes = Character->GetCharacterAttributeComponent();

		Result = CharacterAttributes->GetHealthPercent();
	}
	
	return Result;
}
*/