#include "FPPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../XYZ_HomeWork_Types.h"
#include "Controllers/XYZPlayerController.h"
#include "../Components/MovementComponents/XYZCharacterMovementComponent.h"

AFPPlayerCharacter::AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMEshComponent"));
	FirstPersonMeshComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -128.0f));
	FirstPersonMeshComponent->CastShadow = false;
	FirstPersonMeshComponent->bCastDynamicShadow = false;
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, SocketFPCamera);
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bCastHiddenShadow = true;

	CameraComponent->bAutoActivate = false;

	SpringArmComponent->bAutoActivate = false;
	SpringArmComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	bUseControllerRotationYaw = true;
}

void AFPPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsFPMontagePlaying() && XYZPlayerController.IsValid())
	{
		FRotator TargetControlRotation = XYZPlayerController->GetControlRotation();
		TargetControlRotation.Pitch = 0.0f;
		float BlendSpeed = 300.0f;
		TargetControlRotation = FMath::RInterpTo(XYZPlayerController->GetControlRotation(), TargetControlRotation, DeltaTime, BlendSpeed);
		XYZPlayerController->SetControlRotation(TargetControlRotation);
	}
}

void AFPPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	XYZPlayerController = Cast<AXYZPlayerController>(NewController);
}

void AFPPlayerCharacter::OnStartCrouch(float HalfHeightAdjast, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjast, ScaledHalfHeightAdjust);

	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());

	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z + HalfHeightAdjast;
}

void AFPPlayerCharacter::OnEndCrouch(float HalfHeightAdjast, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjast, ScaledHalfHeightAdjust);

	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());

	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z;
}

void AFPPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (GetXYZCharacterMovementComponent()->IsOnLadder())
	{
		OnLadderStarted();
	}
	else if (PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Ladder)
	{
		OnLadderStoped();
	}

	if (GetXYZCharacterMovementComponent()->IsOnZipline())
	{
		OnZiplineStarted();
	}
	else if (PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Zipline)
	{
		OnZiplineStoped();
	}
}

void AFPPlayerCharacter::OnLadderStarted()
{
	if (XYZPlayerController.IsValid())
	{
		XYZPlayerController->SetIgnoreCameraPitch(true);
		bUseControllerRotationYaw = false;
		APlayerCameraManager* CameraManager = XYZPlayerController->PlayerCameraManager;
		CameraManager->ViewPitchMin = LadderCameraMinPitch;
		CameraManager->ViewPitchMax = LadderCameraMaxPitch;
		CameraManager->ViewYawMin = GetActorForwardVector().ToOrientationRotator().Yaw + LadderCameraMinYaw;
		CameraManager->ViewYawMax = GetActorForwardVector().ToOrientationRotator().Yaw + LadderCameraMaxYaw;
	}
}

void AFPPlayerCharacter::OnLadderStoped()
{
	if (XYZPlayerController.IsValid())
	{
		XYZPlayerController->SetIgnoreCameraPitch(false);
		bUseControllerRotationYaw = true;
		APlayerCameraManager* CameraManager = XYZPlayerController->PlayerCameraManager;
		APlayerCameraManager* DefaultCameraManager = CameraManager->GetClass()->GetDefaultObject<APlayerCameraManager>();
		CameraManager->ViewPitchMin = DefaultCameraManager->ViewPitchMin;
		CameraManager->ViewPitchMax = DefaultCameraManager->ViewPitchMax;
		CameraManager->ViewYawMin = DefaultCameraManager->ViewYawMin;
		CameraManager->ViewYawMax = DefaultCameraManager->ViewYawMax;
	}
}

void AFPPlayerCharacter::OnZiplineStarted()
{
	if (XYZPlayerController.IsValid())
	{
		XYZPlayerController->SetIgnoreCameraPitch(true);
		bUseControllerRotationYaw = false;
		APlayerCameraManager* CameraManager = XYZPlayerController->PlayerCameraManager;
		CameraManager->ViewPitchMin = ZiplineCameraMinPitch;
		CameraManager->ViewPitchMax = ZiplineCameraMaxPitch;
		CameraManager->ViewYawMin = GetActorForwardVector().ToOrientationRotator().Yaw + ZiplineCameraMinYaw;
		CameraManager->ViewYawMax = GetActorForwardVector().ToOrientationRotator().Yaw + ZiplineCameraMaxYaw;
	}
}

void AFPPlayerCharacter::OnZiplineStoped()
{
	if (XYZPlayerController.IsValid())
	{
		XYZPlayerController->SetIgnoreCameraPitch(false);
		bUseControllerRotationYaw = true;
		APlayerCameraManager* CameraManager = XYZPlayerController->PlayerCameraManager;
		APlayerCameraManager* DefaultCameraManager = CameraManager->GetClass()->GetDefaultObject<APlayerCameraManager>();
		CameraManager->ViewPitchMin = DefaultCameraManager->ViewPitchMin;
		CameraManager->ViewPitchMax = DefaultCameraManager->ViewPitchMax;
		CameraManager->ViewYawMin = DefaultCameraManager->ViewYawMin;
		CameraManager->ViewYawMax = DefaultCameraManager->ViewYawMax;
	}
}

FRotator AFPPlayerCharacter::GetViewRotation() const
{
	FRotator Result = Super::GetViewRotation();

	if (IsFPMontagePlaying())
	{
		FRotator SocketRotation = FirstPersonMeshComponent->GetSocketRotation(SocketFPCamera);
		Result.Pitch += SocketRotation.Pitch;
		Result.Yaw = SocketRotation.Yaw;
		Result.Roll = SocketRotation.Roll;
	}

	return Result;
}

bool AFPPlayerCharacter::IsFPMontagePlaying() const
{
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();

	return IsValid(FPAnimInstance) && FPAnimInstance->IsAnyMontagePlaying();
}

void AFPPlayerCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime)
{
	Super::OnMantle(MantlingSettings, MantlingAnimationStartTime);

	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();

	if (IsValid(FPAnimInstance) && MantlingSettings.FPMantlingMontage)
	{
		if (XYZPlayerController.IsValid())
		{
			XYZPlayerController->SetIgnoreLookInput(true);
			XYZPlayerController->SetIgnoreMoveInput(true);
		}

		float MontageDuration = FPAnimInstance->Montage_Play(MantlingSettings.FPMantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingAnimationStartTime);

		GetWorld()->GetTimerManager().SetTimer(FPMontageTimer, this, &AXYZBaseCharacter::OnMontageTimerElapsed, MontageDuration, false);
	}
}

void AFPPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	FVector CurrentCharacterLocation = GetActorLocation();

	if(IsHardLanding(CurrentCharacterLocation))
	{
		OnHardLanded();
	}
}

void AFPPlayerCharacter::OnHardLanded()
{
	Super::OnHardLanded();

	if (XYZPlayerController.IsValid())
	{
		XYZPlayerController->SetIgnoreLookInput(true);

		UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();

		float MontageStartTime = 0.0f;
		float MontageDuration = FPAnimInstance->Montage_Play(FPHardLandingMontage, 1.0f, EMontagePlayReturnType::Duration, MontageStartTime);

		GetWorld()->GetTimerManager().SetTimer(FPMontageTimer, this, &AXYZBaseCharacter::OnMontageTimerElapsed, MontageDuration, false);
	}
	
}