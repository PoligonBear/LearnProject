// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XYZBaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "XYZPlayerCharacter.generated.h"

UCLASS(Blueprintable)
class XYZ_HOMEWORK_API AXYZPlayerCharacter : public AXYZBaseCharacter
{
	GENERATED_BODY()

public:
	AXYZPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void Turn(float Value) override;
	virtual void LookUp(float Value) override;
	virtual void TurnAtRate(float Value) override;
	virtual void LookUpAtRate(float Value) override;

	virtual void OnStartSlide(float HalfHeightAdjust, float ScaleHalfHeightAdjust) override;
	virtual void OnEndSlide(float HalfHeightAdjust, float ScaleHalfHeightAdjust) override;
		
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaleHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaleHalfHeightAdjust) override;

	virtual void OnStartCrawl(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrawl(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	
	virtual void SwimForward(float Value) override;
	virtual void SwimRight(float Value) override;
	virtual void SwimUp(float Value) override;

	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;

	void CameraDistanceSprint();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
    void OnSprintStart();
	virtual void OnSprintEnd_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
    void OnSprintEnd();
	virtual void OnSprintStart_Implementation();

	UFUNCTION()
    void TimelineFloatReturn(float Value);

	UFUNCTION()
	void AimingFOVTimeLineFloatReturn(float Value);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void OnStarAimingInternal() override;
	virtual void OnStopAimingInternal() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	class UCameraComponent* CameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	class USpringArmComponent* SpringArmComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Character | Camera")
	float SprintArmLength = 400.0f;

	float DefaultArmLength = 0.0f;

	FTimeline SpringArmSprintTimeline;
	FTimeline AimingFOVTimeline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Camera")
	UCurveFloat* SpringArmCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Camera")
	UCurveFloat* AimingFOVCurve = nullptr;
};


