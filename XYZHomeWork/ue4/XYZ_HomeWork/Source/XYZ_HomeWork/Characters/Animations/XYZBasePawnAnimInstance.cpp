// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZBasePawnAnimInstance.h"
#include "../HomeWorkBasePawn.h"
#include "XYZ_HomeWork/Components/MovementComponents/GC_BasePawnMovementComponent.h"


void UXYZBasePawnAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AHomeWorkBasePawn>(), TEXT("UXYZBasePawnAnimInstance::NativeBeginPlay only AHomeWorkBasePawn can work with UXYZBasePawnAnimInstance."));
	CachedBasePawn = StaticCast<AHomeWorkBasePawn*>(TryGetPawnOwner());
}

void UXYZBasePawnAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedBasePawn.IsValid())
	{
		return;
	}

	InputForward = CachedBasePawn->GetInputForward();
	InputRight = CachedBasePawn->GetInputRight();

	bIsInAir = CachedBasePawn->GetMovementComponent()->IsFalling();
}
