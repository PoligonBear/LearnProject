// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EndSlide.h"
#include "../../XYZBaseCharacter.h"
#include "../../../Components/MovementComponents/XYZCharacterMovementComponent.h"

void UAnimNotify_EndSlide::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	AXYZBaseCharacter* CharacterOwner = Cast<AXYZBaseCharacter>(MeshComp->GetOwner());
	if (IsValid(CharacterOwner))
	{
		CharacterOwner->GetXYZCharacterMovementComponent()->StopSlide();
	}
}
