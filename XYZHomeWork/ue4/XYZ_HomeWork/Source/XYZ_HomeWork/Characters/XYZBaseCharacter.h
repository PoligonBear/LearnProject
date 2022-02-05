 // Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/Attributes/CharacterAttributeComponent.h"
#include "GameFramework/Character.h"
#include "XYZBaseCharacter.generated.h"

USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* MantlingMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* FPMantlingMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCurveVector* MantlingCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorrectionXY = 34.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorrectionZ = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinHeight = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHeightStarTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinHeightStartTime = 0.5f;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAimingStateChanged, bool)

class UCharacterAttributeComponent;
class UCharacterEquipmentComponent;
class UXYZCharacterMovementComponent;
class AXYZPlayerController;
class AInteractiveActor;
class UAnimMontage;

typedef TArray<AInteractiveActor*, TInlineAllocator<10>> TInteractiveActorsArray;

UCLASS(Abstract, NotBlueprintable)
class XYZ_HOMEWORK_API AXYZBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AXYZBaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual void Turn(float Value) {};
	virtual void LookUp(float Value) {};
	virtual void TurnAtRate(float Value) {};
	virtual void LookUpAtRate(float Value) {};

	virtual void Slide();
	virtual void OnStartSlide(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	virtual void OnEndSlide(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* SlidingMontage = nullptr;

	virtual void ChangeCrouchState();

	virtual void StartSprint();
	virtual void StopSprint();

	virtual  void Jump() override;

	virtual  void SwimForward( float Value) {};
	virtual void SwimRight(float Value) {};
	virtual void SwimUp(float Value) {};

	void ClimLadderUp(float Value);
	void InteractWithLadder();

	void InteractWithZipline();

	void Mantle(bool bForce = false);

	/** Shoting **/
	void StartFire();
	void StopFire();
	void StartAiming();
	void StopAiming();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStartAiming();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStopAiming();

	float GetAimingMovementSpeed() const;

	bool IsAiming() const;

	void Reload();

	void NextItem();
	void PreviousItem();
	void EquipPrimaryItem();

	FOnAimingStateChanged OnAimingStateChanged;

	virtual bool CanJumpInternal_Implementation() const override;

	/** Hard Landing **/
	bool IsHardLanding(FVector& CurrentLocation);
	virtual void NotifyJumpApex() override;
	virtual void Falling() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void OnHardLanded();
	virtual void OnMontageTimerElapsed();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Hard landing")
	UAnimMontage* HardLandingMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Hard landing")
	UAnimMontage* FPHardLandingMontage = nullptr;

	FORCEINLINE UXYZCharacterMovementComponent* GetXYZCharacterMovementComponent() { return XYZCharacterMovementComponent; }
	FORCEINLINE UXYZCharacterMovementComponent* GetXYZCharacterMovementComponent() const { return XYZCharacterMovementComponent; }

	UCharacterEquipmentComponent* GetCharacterEquipmentComponent_Mutable() const;
	const UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const;
	UCharacterAttributeComponent* GetCharacterAttributeComponent_Mutable() const;
	const UCharacterAttributeComponent* GetCharacterAttributeComponent() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	class ULedgeDetectorComponent* LedgeDetectorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	UCharacterAttributeComponent* CharacterAttributeComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Character | Movement | Mantling")
	FMantlingSettings HighMantlingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings LowMantlingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float LowMantleMaxHeight = 125.0f;
	
    float GetIKRightFootOffset() const { return IKRightFootOffset; }
    float GetIKLeftFootOffset() const { return IKLeftFootOffset; }
	float GetIKPelvisOffset() const { return IKPelvisOffset; }
	
	/* Crawl */
	virtual void ChangeCrawlState();
	
	UFUNCTION(BlueprintCallable, Category = Character, meta = (HidePin = "bClientSimulation"))
    virtual void Crawl(bool bClientSimulation = false);
	
	UFUNCTION(BlueprintCallable, Category = Character, meta = (HidePin = "bClientSimulation"))
    virtual void UnCrawl(bool bClientSimulation = false);

	UFUNCTION(BlueprintCallable, Category = Character)
    virtual bool CanCrawl() const;
	
	virtual void OnEndCrawl(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	
	virtual void OnStartCrawl(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	UPROPERTY(BlueprintReadOnly, Category = Character)
	bool bIsCrawled = false;

	UPROPERTY(BlueprintReadOnly)
	UAnimMontage* HighMantleMontage;

	void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	void UnRegisterInteractiveActor(AInteractiveActor* InteractiveActor);

	const class ALadder* GetAvailableLadder() const;
	const class AZipline* GetAvailableZipline() const;
	void SetIsOutOfStamina(bool bIsOutOfStamina_In);
	bool IsSwimingUnderWater(FVector SocketLocation, float VolumeTopPlane) const;
	
protected:
	/* Hard Landing */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Hard landing")
	float HardLandingHeight = 1000.0f;

	/* Start IK */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	FName RightFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	FName LeftFootSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | IK settings", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IKTraceDistance = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | IK settings", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IKInterpSpeed = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
	float BaseTurnRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
	float BaseLookUpRate = 45.0f;

	virtual bool CanSprint();

	bool CanMantle() const;

	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime);
		
	bool bIsSprintRequested = false;

	UXYZCharacterMovementComponent* XYZCharacterMovementComponent = nullptr;

	virtual void OnDeath();

	UPROPERTY(EditDefaultsOnly, BlueprintReadONly, Category = "Character | Animations")
	UAnimMontage* OnDeathAnimMontage = nullptr;

	// Damage depending from fall height  (in meters)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes")
	class UCurveFloat* FallDamageCurve = nullptr; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	class  UCharacterEquipmentComponent* CharacterEquipmentComponents = nullptr;

	virtual void OnStarAimingInternal();
	virtual void OnStopAimingInternal();
private: 
	void UpdateIKSettings(float DeltaSeconds);

	float CalculateIKOffsetForSocketName(const FName& SocketName) const;
	float CalculateIKPelvisOffset();
	
	float IKRightFootOffset = 0.0f;
	float IKLeftFootOffset = 0.0f;
	float IKPelvisOffset = 0.0f;

	bool bIsAiming = false;

	float CurrentAimingMovementSpeed;
		
	void TryChangeSprintState();

	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;

	TInteractiveActorsArray AvailableInteractiveActors;

	UFUNCTION()
	void OnPlayerCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void EnableRagDoll();

	FVector CurrentFallApex = FVector::ZeroVector;

	TWeakObjectPtr<class AXYZPlayerController> XYZPlayerController;

	FTimerHandle MontageTimer;
};
