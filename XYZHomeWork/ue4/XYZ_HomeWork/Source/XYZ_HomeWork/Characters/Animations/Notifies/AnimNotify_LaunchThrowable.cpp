// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_LaunchThrowable.h"
#include "Characters/XYZBaseCharacter.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

void UAnimNotify_LaunchThrowable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AXYZBaseCharacter* CharacterOwner = Cast<AXYZBaseCharacter>(MeshComp->GetOwner());
	
	if(!IsValid(CharacterOwner))
	{
		return;
	}

	CharacterOwner->GetCharacterEquipmentComponent_Mutable()->LaunchCurrentThrowableItem();
}
