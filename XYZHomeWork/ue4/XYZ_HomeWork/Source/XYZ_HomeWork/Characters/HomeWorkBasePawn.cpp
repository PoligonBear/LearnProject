// Fill out your copyright notice in the Description page of Project Settings.


#include "HomeWorkBasePawn.h"
#include "Components/SphereComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "../Components/MovementComponents/GC_BasePawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "../XYZ_HomeWork.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
AHomeWorkBasePawn::AHomeWorkBasePawn()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->SetSphereRadius(CollisionSphereRadius);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent = CollisionComponent;

	//MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("Movement component"));
	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UGC_BasePawnMovementComponent>(TEXT("Movement component"));
	MovementComponent->SetUpdatedComponent(CollisionComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = 1;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

#if	WITH_EDITORONLY_DATA
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(RootComponent);
#endif


}


// Called to bind functionality to input
void AHomeWorkBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MoveForward", this, &AHomeWorkBasePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHomeWorkBasePawn::MoveRight);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AHomeWorkBasePawn::Jump);
}

void AHomeWorkBasePawn::MoveForward(float Value)
{
	InputForward = Value;
	if (Value != 0.0f)
	{
		AddMovementInput(CurrentViewActor->GetActorForwardVector(), Value);
	}
}

void AHomeWorkBasePawn::MoveRight(float Value)
{
	InputRight = Value;
	if (Value != 0.0f)
	{
		AddMovementInput(CurrentViewActor->GetActorRightVector(), Value);
	}
}

void AHomeWorkBasePawn::Jump()
{
	checkf(MovementComponent->IsA<UGC_BasePawnMovementComponent>(), TEXT("AHomeWorkBasePawn::Jump() Jump can work only with UGC_BasePawnMovementComponent"));
	UGC_BasePawnMovementComponent* BaseMovement = StaticCast<UGC_BasePawnMovementComponent*>(MovementComponent);
	BaseMovement->JumpStar();
}

void AHomeWorkBasePawn::BeginPlay()
{
	Super::BeginPlay();
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	CurrentViewActor = CameraManager->GetViewTarget();
	CameraManager->OnBlendComplete().AddUFunction(this, FName("OnBlendComplete"));
}

void AHomeWorkBasePawn::OnBlendComplete()
{
	CurrentViewActor = GetController()->GetViewTarget();
	UE_LOG(LogCameras, Log, TEXT("AHomeWorkBasePawn::OnBlendComplete() Current view actor: %s"), *CurrentViewActor->GetName());

}

