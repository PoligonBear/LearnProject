// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlatform.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"

// Sets default values
ABasePlatform::ABasePlatform()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Platform Root"));
	RootComponent = DefaultPlatformRoot;

	PlatformMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	PlatformMeshComponent->SetupAttachment(DefaultPlatformRoot);
}

// Called when the game starts or when spawned
void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = PlatformMeshComponent->GetRelativeLocation();

	FOnTimelineFloat InterpFunction;
	FOnTimelineEvent TimelineFinished;
	InterpFunction.BindUFunction(this, FName("TimelineFloatReturn"));
	TimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));
	PlatformMovementTimeline.AddInterpFloat(PlatformMovementCurve, InterpFunction, FName("Alpha"));
	PlatformMovementTimeline.SetTimelineFinishedFunc(TimelineFinished);
	PlatformMovementTimeline.SetLooping(false);
	PlatformMovementTimeline.SetTimelineLength(2.0f);	

	if (MovementType == EMovementType::Loop)
	{
		MovePlatform();
	}
}

void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlatformMovementTimeline.TickTimeline(DeltaTime);
}

void ABasePlatform::TimelineFloatReturn(float Value)
{
	PlatformMeshComponent->SetRelativeLocation(FMath::Lerp(StartLocation, EndLocation, Value));
	GetWorld()->GetTimerManager().ClearTimer(PlatformMovementTimerHandle);
}

void ABasePlatform::MovePlatform()
{
	bReverse = !bReverse;

	if (bReverse)
	{
		PlatformMovementTimeline.SetPlayRate(ForwardSpeedRate);
		PlatformMovementTimeline.Play();
	}
	else
	{
		PlatformMovementTimeline.SetPlayRate(BackwardSpeedRate);
		PlatformMovementTimeline.Reverse();
	}
}

void ABasePlatform::OnTimelineFinished()
{
	if (MovementType == EMovementType::Loop)
	{
		GetWorld()->GetTimerManager().SetTimer(PlatformMovementTimerHandle, this, &ABasePlatform::MovePlatform, CooldownTimer, false);
	}
}

