// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableItem.h"
#include "Actors/Projectiles/XYZProjectile.h"
#include "Characters/XYZBaseCharacter.h"

void AThrowableItem::Throw()
{
	checkf(GetOwner()->IsA<AXYZBaseCharacter>(), TEXT("AThrowableItem::Throw() only character can be owner of throwable weapon"));
	AXYZBaseCharacter* CharacterOwner = StaticCast<AXYZBaseCharacter*>(GetOwner());

	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
	if (!IsValid(Controller))
	{
		return;
	}
	
	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;
	
	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);
	FTransform PlayerViewTransform(PlayerViewRotation, PlayerViewPoint);
	
	FVector ViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	FVector ViewUpVector = PlayerViewRotation.RotateVector(FVector::UpVector);
	FVector LaunchDirection = ViewDirection + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * ViewUpVector;
	
	FVector ThrowableSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable);
	FVector SocketInViewSpace = PlayerViewTransform.InverseTransformPosition(ThrowableSocketLocation);
	
	FVector SpawnLocation = PlayerViewPoint + ViewDirection * SocketInViewSpace.X;
	AXYZProjectile* Projectile = GetWorld()->SpawnActor<AXYZProjectile>(ProjectileClass, SpawnLocation, FRotator::ZeroRotator);

	if(IsValid(Projectile))
	{
		Projectile->SetOwner(GetOwner());
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	}
}

EAmunitionType AThrowableItem::GetAmmoType() const
{
	return AmmoType;
}
