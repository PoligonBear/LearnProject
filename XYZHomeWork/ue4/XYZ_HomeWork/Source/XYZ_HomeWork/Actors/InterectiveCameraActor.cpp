// Fill out your copyright notice in the Description page of Project Settings.


#include "InterectiveCameraActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

AInterectiveCameraActor::AInterectiveCameraActor()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Camera interaction volume"));
	BoxComponent->SetBoxExtent(FVector(200.0f, 200.0f, 200.0f));
	BoxComponent->SetCollisionObjectType(ECC_WorldDynamic);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComponent->SetupAttachment(RootComponent);
}

void AInterectiveCameraActor::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AInterectiveCameraActor::OnBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AInterectiveCameraActor::OnEndOverlap);
}

void AInterectiveCameraActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerController* PlayreController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FViewTargetTransitionParams TransitionToCameraParams;
	TransitionToCameraParams.BlendTime = TransitionToCameraTime;
	PlayreController->SetViewTarget(this, TransitionToCameraParams);
}

void AInterectiveCameraActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerController* PlayreController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APawn* Pawn = PlayreController->GetPawn();
	FViewTargetTransitionParams TransitionToPawnParams;
	TransitionToPawnParams.BlendTime = TransitionToPawnTime;
	TransitionToPawnParams.bLockOutgoing = true;
	PlayreController->SetViewTarget(Pawn, TransitionToPawnParams);
}
