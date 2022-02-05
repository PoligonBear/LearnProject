#include "InteractiveActor.h"
#include "../../Characters/XYZBaseCharacter.h"
#include "Components/CapsuleComponent.h"


void AInteractiveActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(InteractionVolume))
	{
		InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeOverlapBegin);
		InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeOverlapEnd);
	}
}

void AInteractiveActor::OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}
	AXYZBaseCharacter* BaseCharacter = StaticCast<AXYZBaseCharacter*>(OtherActor);
	BaseCharacter->RegisterInteractiveActor(this);
}

void AInteractiveActor::OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}
	AXYZBaseCharacter* BaseCharacter = StaticCast<AXYZBaseCharacter*>(OtherActor);
	BaseCharacter->UnRegisterInteractiveActor(this);
}

bool AInteractiveActor::IsOverlappingCharacterCapsule(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	AXYZBaseCharacter* BaseCharacter = Cast<AXYZBaseCharacter>(OtherActor);
	if (!IsValid(BaseCharacter))
	{
		return false;
	}

	if (OtherComp != BaseCharacter->GetCapsuleComponent())
	{
		return false;
	}
	return true;
}

