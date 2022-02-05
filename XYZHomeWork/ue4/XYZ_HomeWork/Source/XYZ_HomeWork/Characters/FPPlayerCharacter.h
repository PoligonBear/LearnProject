#pragma once

#include "CoreMinimal.h"
#include "XYZPlayerCharacter.h"
#include "FPPlayerCharacter.generated.h"

class USkeletalMeshComponent;
class UCameraComponent;

UCLASS()
class XYZ_HOMEWORK_API AFPPlayerCharacter : public AXYZPlayerCharacter
{
	GENERATED_BODY()

public:
	AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnStartCrouch(float HalfHeightAdjast, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjast, float ScaledHalfHeightAdjust) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	virtual FRotator GetViewRotation() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First person")
	USkeletalMeshComponent* FirstPersonMeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First person")
	UCameraComponent* FirstPersonCameraComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Pitch", meta = (UIMn = -89.0f, UIMax = 89.0f))
	float LadderCameraMinPitch = -60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Pitch", meta = (UIMn = -89.0f, UIMax = 89.0f))
	float LadderCameraMaxPitch = 80.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Yaw", meta = (UIMn = -180.0f, UIMax = 180.0f))
	float LadderCameraMinYaw = -40.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Yaw", meta = (UIMn = -180.0f, UIMax = 180.0f))
	float LadderCameraMaxYaw = 40.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Zipline | Pitch", meta = (UIMn = -89.0f, UIMax = 89.0f))
	float ZiplineCameraMinPitch = -45.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Zipline | Pitch", meta = (UIMn = -89.0f, UIMax = 89.0f))
	float ZiplineCameraMaxPitch = 40.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Zipline | Yaw", meta = (UIMn = -180.0f, UIMax = 180.0f))
	float ZiplineCameraMinYaw = -40.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Zipline | Yaw", meta = (UIMn = -180.0f, UIMax = 180.0f))
	float ZiplineCameraMaxYaw = 40.0f;

private:
	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime) override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void OnHardLanded() override;

	void OnLadderStarted();
	void OnLadderStoped();
	
	void OnZiplineStarted();
	void OnZiplineStoped();

	bool IsFPMontagePlaying() const;

	FTimerHandle FPMontageTimer;

	TWeakObjectPtr<class AXYZPlayerController> XYZPlayerController;
};
