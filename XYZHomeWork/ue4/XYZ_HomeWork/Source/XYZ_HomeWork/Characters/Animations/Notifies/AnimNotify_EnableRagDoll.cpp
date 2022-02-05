// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EnableRagDoll.h"
#include "../../../XYZ_HomeWork_Types.h"

void UAnimNotify_EnableRagDoll::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	MeshComp->SetCollisionProfileName(CollisionProfileRagDoll);
	MeshComp->SetSimulatePhysics(true);
}
