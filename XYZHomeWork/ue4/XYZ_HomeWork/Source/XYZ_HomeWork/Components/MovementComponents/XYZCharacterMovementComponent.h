// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../LedgeDetectorComponent.h"
#include "XYZ_HomeWork/Characters/XYZBaseCharacter.h"
#include "XYZCharacterMovementComponent.generated.h"

struct FMantlingMovementParameters
{
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;
	
	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;

	FVector InitialAnimationLocation = FVector::ZeroVector;

	float Duration = 1.0f;
	float StartTime = 0.0f;

	UCurveVector* MantlingCurve = nullptr;
	class UPrimitiveComponent* TargetComponent = nullptr;
};


UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_Ladder UMETA(DisplayName = "Ladder"),
	CMOVE_Zipline UMETA(DisplayName = "Zipline"),
	CMOVE_WallRun UMETA(DisplayName = "WallRun"),
	CMOVE_Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDetachFromLadderMethod : uint8
{
	Fall = 0,
	ReachingFromTop,
	ReachingFromBottom,
	JumpOff
};

UENUM(BlueprintType)
enum class EDetachFromZiplineMethod : uint8
{
	Fall = 0,
	JumpOff
};

UENUM(BlueprintType)
enum class EWallRunSide : uint8
{
	None,
	Left,
	Right
};

/**
 * 
 */
UCLASS()
class XYZ_HOMEWORK_API UXYZCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	virtual void PhysicsRotation(float DeltaTime) override;

	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;
	virtual float GetMaxSpeed() const override;
	float GetVelocity() {return  Velocity.Size(); };

	/** Sprinting **/
	bool bIsSprinting;
	FORCEINLINE bool IsSprinting() { return bIsSprinting; }
	void StarSprint();
	void StopSprint();

	/** Stamina **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement")
	bool bIsOutOfStamina = false;

	FORCEINLINE bool IsOutOfStamina() const { return bIsOutOfStamina; }
	void SetIsOutOfStamina(bool bIsOutOfStamina_In);
	
	/** Crawling **/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Character Movement")
	bool bWantsToCrawl = false;

	virtual bool IsCrawling() const;	
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;
	virtual bool CanCrawlInCurrentState() const;
	virtual void Crawl();
	virtual void UnCrawl();

	/** Mantling **/
	FMantlingMovementParameters CurrentMantlingParameters;
	void StartMantle(const FMantlingMovementParameters& MantlingParameters);
	void EndMantle();
	bool IsMantling() const;	

	/** Ladder **/
	float GetLadderSpeedRatio() const;
	void AttachToLadder(const class ALadder* Ladder);
	void DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod = EDetachFromLadderMethod::Fall);
	bool IsOnLadder() const;
	float GetActorToCurrentLadderProjection(const FVector& Location) const;
	const class ALadder* GetCurrentLadder();

	/** Zipline **/
	void AttachToZipline(const class AZipline* Zipline);
	void DetachFromZipline(EDetachFromZiplineMethod DetachFromZiplineMethod = EDetachFromZiplineMethod::Fall);
	float GetZiplineSpeed();
	bool IsOnZipline() const;
	const class AZipline* GetCurrentZipline() { return CurrentZipline; }
	FVector ZiplineDirection = FVector::ZeroVector;

	/** WallRun **/
	void GetWallRunSideAndDirection(const FVector& HitNormal, EWallRunSide& OutSide, FVector& OutDirection) const;
	void StartWallRun(EWallRunSide Side, const FVector& Direction);
	void StopWallRun();
	bool IsWallRunning() const;
	bool CanWallRun(const FVector& HitNormal, EWallRunSide Side) const;
	bool IsSurfaceWallRunnable(const FVector& SurfaceNormal) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallRun")
	EWallRunSide CurrentWallRunSide = EWallRunSide::None;

	EWallRunSide PreviousWallRunSide = EWallRunSide::None;

	FVector CurrentWallRunDirection = FVector::ZeroVector;

	FTimerHandle WallRunTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallRun", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float MaxWallRunTime = 1.0f;

	/** Slide **/
	bool bIsSliding = false;
	bool IsSliding() const;

	void StartSlide();
	void StopSlide();

protected:
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	void PhysMantling(float DeltaTime, uint32 Iterations);
	void PhysLadder(float DeltaTime, uint32 Iterations);
	void PhysZipline(float DeltaTime, uint32 Iterations);
	void PhysWallRun(float DeltaTime, uint32 Iterations);
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  "Character Movement: Swimming", meta = (ClampMin ="0", UIMin = "0"))
	float SwimingCapsuleRadius = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  "Character Movement: Swimming", meta = (ClampMin ="0", UIMin = "0"))
	float SwimingCapsuleHalfHeight = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement",  meta = (ClampMin = "0", UIMin = "0"))
	float CrawlCapsuleHalfHeight = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement",  meta = (ClampMin = "0", UIMin = "0"))
	float CrawlCapsuleRadius = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  "Character Movement: Ladder", meta = (ClampMin ="0", UIMin = "0"))
	float ClimbingOnLadderMaxSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  "Character Movement: Ladder", meta = (ClampMin ="0", UIMin = "0"))
	float LadderToCharacterOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  "Character Movement: Ladder", meta = (ClampMin ="0", UIMin = "0"))
	float ClimbingOnLadderBrakingDecelartion = 2048.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category  = "Character movement", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OutOfStaminaSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", meta = (ClampMin = 0.0f, IUMin = 0.0f))
	float MaxCrawlSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", meta = (ClampMin = 0.0f, IUMin = 0.0f))
	float MaxLadderTopOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", meta = (ClampMin = 0.0f, IUMin = 0.0f))
	float MinLadderBottomOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", meta = (ClampMin = 0.0f, IUMin = 0.0f))
	float JumpOffFromLadderSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", meta = (ClampMin = 0.0f, IUMin = 0.0f))
	float ZiplineDirectionOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", meta = (ClampMin = 0.0f, IUMin = 0.0f))
	float JumpOffFromZiplineSpeed = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character movement : Zipline")
	float ZiplineSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement : Zipline")
	float TopOffset = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxWallRunSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement : Slide", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SlidingCapsuleHalfHeight = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement : Slide", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SlidingCapsuleRadius = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement : Slide", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxSlidingSpeed = 1000.0f;
	
	class AXYZBaseCharacter* GetBaseCharacterOwner() const;
private:
	FRotator ForcedTargetRotation = FRotator::ZeroRotator;
	bool bForceRotation = false;
	
	const AZipline* CurrentZipline = nullptr;
	const ALadder* CurrentLadder = nullptr;
	FTimerHandle MantlingTimer;
};
