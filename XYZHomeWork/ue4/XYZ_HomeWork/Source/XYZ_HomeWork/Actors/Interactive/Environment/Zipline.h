// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../InteractiveActor.h"
#include "Zipline.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable)
class XYZ_HOMEWORK_API AZipline : public AInteractiveActor
{
	GENERATED_BODY()
	
public:
	AZipline();

	virtual void OnConstruction(const FTransform& Transform) override;
	FVector GetHeightPoleTopPosition() const;
	FVector GetMoveDirection() const { return DirectionVector; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters", meta = (MakeEditWidget))
	FVector SecondPoleBottom = FVector::FVector(100.0f, 0.0f, -50.0f);

	class UPrimitiveComponent* CableComponent = nullptr;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	UStaticMeshComponent* FirstPoleStaticMeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	UStaticMeshComponent* SecondPoleStaticMeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	UStaticMeshComponent* CableStaticMeshComponent = nullptr;	

	UCapsuleComponent* GetZiplineInteractionCapsule() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	FVector DirectionVector = FVector::ZeroVector;
};
