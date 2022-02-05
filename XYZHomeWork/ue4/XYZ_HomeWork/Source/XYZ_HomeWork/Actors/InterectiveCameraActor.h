// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "InterectiveCameraActor.generated.h"

/**
 * 
 */
UCLASS()
class XYZ_HOMEWORK_API AInterectiveCameraActor : public ACameraActor
{
	GENERATED_BODY()
	
public:
	AInterectiveCameraActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition settings")
	float TransitionToCameraTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition settings")
	float TransitionToPawnTime = 2.0f;
	 

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
