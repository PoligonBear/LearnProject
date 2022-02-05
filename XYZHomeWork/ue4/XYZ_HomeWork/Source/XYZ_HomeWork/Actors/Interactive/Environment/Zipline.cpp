// Fill out your copyright notice in the Description page of Project Settings.


#include "Zipline.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "XYZ_HomeWork/XYZ_HomeWork_Types.h"

AZipline::AZipline()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ZiplineRoot"));

	FirstPoleStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirstPilar"));
	FirstPoleStaticMeshComponent->SetupAttachment(RootComponent);

	SecondPoleStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondPilar"));
	SecondPoleStaticMeshComponent->SetupAttachment(RootComponent);

	CableStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cable"));
	CableStaticMeshComponent->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);

	CableComponent = CableStaticMeshComponent;
}

void AZipline::OnConstruction(const FTransform& Transform)
{
	FVector FirstPoleLocation = FirstPoleStaticMeshComponent->GetRelativeLocation();
	FVector SecondPoleTop = SecondPoleStaticMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize();
	
	DirectionVector = (SecondPoleBottom	- FirstPoleLocation).GetSafeNormal();
	FRotator CableRotation = DirectionVector.ToOrientationRotator();
	FRotator InteractionVolumeRatotion = CableRotation;
	InteractionVolumeRatotion.Pitch -= 90.f;
	float CableLength = SecondPoleBottom.Size();
	
	FirstPoleStaticMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SecondPoleStaticMeshComponent->SetRelativeLocation(SecondPoleBottom);
	
	CableStaticMeshComponent->SetRelativeLocation(FVector((SecondPoleBottom - FirstPoleLocation).X * 0.5f, SecondPoleBottom.Y * 0.5f, SecondPoleTop.Z * 0.5f + SecondPoleBottom.Z * 0.5f));
	CableStaticMeshComponent->SetRelativeRotation(CableRotation);
	
	GetZiplineInteractionCapsule()->SetRelativeLocation(FVector(CableStaticMeshComponent->GetRelativeLocation().X, CableStaticMeshComponent->GetRelativeLocation().Y, CableStaticMeshComponent->GetRelativeLocation().Z));
	GetZiplineInteractionCapsule()->SetRelativeRotation(InteractionVolumeRatotion);	

	UStaticMesh* CableMesh = CableStaticMeshComponent->GetStaticMesh();
	if (IsValid(CableMesh))
	{
		float MeshSizeX = CableMesh->GetBoundingBox().GetSize().X;
		if (!FMath::IsNearlyZero(MeshSizeX))
		{
			
			CableStaticMeshComponent->SetRelativeScale3D(FVector(CableLength / MeshSizeX, 1.0f, 1.0f));
		}
	}

	GetZiplineInteractionCapsule()->SetRelativeScale3D(FVector(3.0f, 3.0f, CableLength / GetZiplineInteractionCapsule()->GetUnscaledCapsuleHalfHeight() * 0.5f));
}

FVector AZipline::GetHeightPoleTopPosition() const
{
	FVector FirstPole = FirstPoleStaticMeshComponent->GetRelativeLocation();
	FVector SecondPole = SecondPoleStaticMeshComponent->GetRelativeLocation();
	FVector FirstPoleTop = FirstPoleStaticMeshComponent->GetComponentLocation();
	FVector SecondPoleTop = SecondPoleStaticMeshComponent->GetComponentLocation();

	return FirstPole.Z > SecondPole.Z ? FirstPoleTop : SecondPoleTop;
}

UCapsuleComponent* AZipline::GetZiplineInteractionCapsule() const
{
	return StaticCast<UCapsuleComponent*>(InteractionVolume);
}