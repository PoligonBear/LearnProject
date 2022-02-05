// Fill out your copyright notice in the Description page of Project Settings.

#include "PlatformInvokator.h"
#include "BasePlatform.h"

// Sets default values
APlatformInvokator::APlatformInvokator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetHiddenInGame(false);
	BoxCollision->SetupAttachment(RootComponent);
}

void APlatformInvokator::BeginPlay()
{
	Super::BeginPlay();
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &APlatformInvokator::OnOverlapBegin);
}

void APlatformInvokator::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(Platforms))
	{
		Platforms->MovePlatform();
	}
}
