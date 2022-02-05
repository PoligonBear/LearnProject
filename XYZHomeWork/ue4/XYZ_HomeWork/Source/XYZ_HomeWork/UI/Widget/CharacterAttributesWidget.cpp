// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributesWidget.h"
#include "Characters/XYZBaseCharacter.h"

void UCharacterAttributesWidget::UpdateHealthPercent(float HealthPercent)
{
	CurrentHealthPercent = HealthPercent;
	
}

void UCharacterAttributesWidget::UpdateStaminaPercent(float StaminaPercent)
{
	CurrentStaminaPercent = StaminaPercent;
}

void UCharacterAttributesWidget::UpdateOxygenPercent(float OxygenPercent)
{
	CurrentOxygenPercent = OxygenPercent;
}

ESlateVisibility UCharacterAttributesWidget::StaminaVisibility()
{
	APawn* Pawn = GetOwningPlayerPawn();
	AXYZBaseCharacter* Character = Cast<AXYZBaseCharacter>(Pawn);

	if (IsValid(Character))
	{
		const UCharacterAttributeComponent* CharacterAttributes = Character->GetCharacterAttributeComponent();
		if (CharacterAttributes->IsStaminaLessOfMax())
		{
			return ESlateVisibility::Visible;
		}
	}
	return ESlateVisibility::Hidden;
}

ESlateVisibility UCharacterAttributesWidget::OxygenVisibility()
{
	APawn* Pawn = GetOwningPlayerPawn();
	AXYZBaseCharacter* Character = Cast<AXYZBaseCharacter>(Pawn);

	if (IsValid(Character))
	{
		const UCharacterAttributeComponent* CharacterAttributes = Character->GetCharacterAttributeComponent();
		if (CharacterAttributes->IsOxygenLessOfMax())
		{
			return ESlateVisibility::Visible;
		}
	}
	return ESlateVisibility::Hidden;
}
