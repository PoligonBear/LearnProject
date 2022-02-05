#include "XYZPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "XYZBaseCharacter.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Components/CapsuleComponent.h"
#include "Components/MovementComponents/XYZCharacterMovementComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

AXYZPlayerCharacter::AXYZPlayerCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = true;

	GetCharacterMovement()->bOrientRotationToMovement = 1;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
}

void AXYZPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	DefaultArmLength = SpringArmComponent->TargetArmLength;

	FOnTimelineFloat InterpFunction;
	InterpFunction.BindUFunction(this, FName("TimelineFloatReturn"));

	SpringArmSprintTimeline.AddInterpFloat(SpringArmCurve, InterpFunction, FName("Alpha"));
	SpringArmSprintTimeline.SetLooping(false);
	SpringArmSprintTimeline.SetTimelineLength(0.5f);

	FOnTimelineFloat AimingFOVInterpFunction;
	AimingFOVInterpFunction.BindUFunction(this, FName("AimingFOVTimeLineFloatReturn"));
	AimingFOVTimeline.AddInterpFloat(AimingFOVCurve, AimingFOVInterpFunction, FName("Alpha"));
	AimingFOVTimeline.SetLooping(false);
	AimingFOVTimeline.SetTimelineLength(0.5f);
}

void AXYZPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SpringArmSprintTimeline.TickTimeline(DeltaTime);
	
	if(IsValid(AimingFOVCurve))
	{
		AimingFOVTimeline.TickTimeline(DeltaTime);	
	}
	
	CameraDistanceSprint();
}

void AXYZPlayerCharacter::OnStarAimingInternal()
{
	Super::OnStarAimingInternal();
	APlayerController* PlayerController = GetController<APlayerController>();

	if(!IsValid(PlayerController))
	{
		return;
	}

	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if(IsValid(CameraManager))
	{
		if(!IsValid(AimingFOVCurve))
		{
			ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
			CameraManager->SetFOV(CurrentRangeWeapon->GetAimFOV());	
		}
		AimingFOVTimeline.Play();
	}
}

void AXYZPlayerCharacter::OnStopAimingInternal()
{
	Super::OnStopAimingInternal();
	APlayerController* PlayerController = GetController<APlayerController>();

	if(!IsValid(PlayerController))
	{
		return;
	}

	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if(IsValid(CameraManager))
	{
		if(!IsValid(AimingFOVCurve))
		{
			ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
			CameraManager->UnlockFOV();	
		}
		AimingFOVTimeline.Reverse();
	}
}

void AXYZPlayerCharacter::TimelineFloatReturn(float Value)
{
	SpringArmComponent->TargetArmLength = FMath::Lerp(DefaultArmLength, SprintArmLength, Value);
}

void AXYZPlayerCharacter::AimingFOVTimeLineFloatReturn(float Value)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	
	CameraManager->SetFOV(FMath::Lerp(CameraManager->DefaultFOV, CurrentRangeWeapon->GetAimFOV(), Value));
}

void AXYZPlayerCharacter::MoveForward(float Value)
{
	if ((GetXYZCharacterMovementComponent()->IsMovingOnGround() || GetXYZCharacterMovementComponent()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void AXYZPlayerCharacter::MoveRight(float Value)
{
	if ((GetXYZCharacterMovementComponent()->IsMovingOnGround() || GetXYZCharacterMovementComponent()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void AXYZPlayerCharacter::Turn(float Value)
{
	if(IsValid(GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()))
	{
		AddControllerYawInput(Value * GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()->GetAimTurnModifier());	
	}
	else
	{
		AddControllerYawInput(Value);	
	}
}

void AXYZPlayerCharacter::LookUp(float Value)
{
	if(IsValid(GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()))
	{
		AddControllerPitchInput(Value * GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()->GetAimLookUpModifier());
	}
	else
	{
		AddControllerPitchInput(Value);	
	}
}

void AXYZPlayerCharacter::TurnAtRate(float Value)
{
	if(IsValid(GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()))
	{
		AddControllerYawInput(Value *  GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()->GetAimTurnModifier() * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
	else
	{
		AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AXYZPlayerCharacter::LookUpAtRate(float Value)
{
	if(IsValid(GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()))
	{
		AddControllerPitchInput(Value * GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()->GetAimLookUpModifier() * BaseLookUpRate * GetWorld()->GetDeltaSeconds());	
	}
	else
	{
		AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());	
	}
}

void AXYZPlayerCharacter::OnStartSlide(float HalfHeightAdjust, float ScaleHalfHeightAdjust)
{
	Super::OnStartSlide(HalfHeightAdjust, ScaleHalfHeightAdjust);
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void AXYZPlayerCharacter::OnEndSlide(float HalfHeightAdjust, float ScaleHalfHeightAdjust)
{
	Super::OnEndSlide(HalfHeightAdjust, ScaleHalfHeightAdjust);
	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void AXYZPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaleHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaleHalfHeightAdjust);
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void AXYZPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaleHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaleHalfHeightAdjust);
	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void AXYZPlayerCharacter::OnStartCrawl(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrawl(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void AXYZPlayerCharacter::OnEndCrawl(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrawl(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void AXYZPlayerCharacter::SwimForward(float Value)
{
	if (GetXYZCharacterMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator PitchYawRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = PitchYawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void AXYZPlayerCharacter::SwimRight(float Value)
{
	if (GetXYZCharacterMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void AXYZPlayerCharacter::SwimUp(float Value)
{
	if (GetXYZCharacterMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		
		AddMovementInput(FVector::UpVector, Value);
	}
}

void AXYZPlayerCharacter::CameraDistanceSprint()
{
	if (bIsSprintRequested && XYZCharacterMovementComponent->Velocity.Size() > 0.1f )
	{
		OnSprintStart();
	}

	if (!bIsSprintRequested )
	{
		OnSprintEnd();
	}
}

void AXYZPlayerCharacter::OnSprintStart_Implementation()
{
	if (!CanSprint())
	{
		return;
	}
	
	if(!XYZCharacterMovementComponent->IsOutOfStamina())
	{
		SpringArmSprintTimeline.Play();
	}	
	
}

void AXYZPlayerCharacter::OnSprintEnd_Implementation()
{
	SpringArmSprintTimeline.Reverse();
}

void AXYZPlayerCharacter::OnJumped_Implementation()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

bool AXYZPlayerCharacter::CanJumpInternal_Implementation() const
{
	if(GetXYZCharacterMovementComponent()->IsSliding())
	{
		return false;
	}
	
	if (bIsCrawled)
	{
		return false;
	}

	if (XYZCharacterMovementComponent->IsOutOfStamina())
	{
		return false;
	}
	
	if (bIsCrouched)
	{
		ACharacter* DefaultCharacter = GetClass()->GetDefaultObject<ACharacter>();
		check(GetCapsuleComponent());
		const float CurrentCrouchedHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const float	ComonentScale = GetCapsuleComponent()->GetShapeScale();
		const float OldUnscaledHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		const float HalfHeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()-OldUnscaledHalfHeight;
		const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComonentScale;
		const FVector PawnLocation = GetCharacterMovement()->UpdatedComponent->GetComponentLocation();
		const float SweepInlation = KINDA_SMALL_NUMBER * 10.0f;

		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, this);
		FCollisionResponseParams ResponseParams;
		GetCharacterMovement()->InitCollisionParams(CapsuleParams, ResponseParams);

		float Radius, HalfHeight;
		GetCapsuleComponent()->GetScaledCapsuleSize(Radius, HalfHeight);
		FVector CapsuleExtent(Radius, Radius, HalfHeight);
		const float MinExtent = KINDA_SMALL_NUMBER * 10.0f;
		CapsuleExtent.Z = FMath::Max(CapsuleExtent.Z - (-SweepInlation - ScaledHalfHeightAdjust), MinExtent);
		const FCollisionShape StandingCapsuleShape = FCollisionShape::MakeCapsule(CapsuleExtent);
		const ECollisionChannel CollisionChannel = GetCharacterMovement()->UpdatedComponent->GetCollisionObjectType();
		FVector StandingLocation = PawnLocation + FVector(0.0f, 0.0f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentCrouchedHalfHeight);
		bool bEncrouched = GetWorld()->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParams);
		if(bEncrouched )
		{
			return	false;
		}
	}
	return bIsCrouched || Super::CanJumpInternal_Implementation();
}
