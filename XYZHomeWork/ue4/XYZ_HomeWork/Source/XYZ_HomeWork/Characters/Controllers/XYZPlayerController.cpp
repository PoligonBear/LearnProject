// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZPlayerController.h"
#include "../XYZBaseCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "UI/Widget/PlayerHUDWidget.h"
#include "UI/Widget/ReticleWidget.h"
#include "UI/Widget/AmmoWidget.h"

void AXYZPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	
	CachedBaseCharacter = Cast<AXYZBaseCharacter>(InPawn);
	CreateAndInitializeWidgets();
}

bool AXYZPlayerController::GetIgnoreCameraPitch() const
{
	return bIgnoreCameraPitch;
}

void AXYZPlayerController::SetIgnoreCameraPitch(bool bIgnoreCameraPitch_In)
{
	bIgnoreCameraPitch = bIgnoreCameraPitch_In;
}

void AXYZPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &AXYZPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AXYZPlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &AXYZPlayerController::Turn);
	InputComponent->BindAxis("TurnAtRate", this, &AXYZPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &AXYZPlayerController::LookUp);
	InputComponent->BindAxis("LookUpAtRate", this, &AXYZPlayerController::LookUpAtRate);
	InputComponent->BindAxis("SwimForward", this, &AXYZPlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &AXYZPlayerController::SwimRight);
	InputComponent->BindAxis("SwimUp", this, &AXYZPlayerController::SwimUp);
	InputComponent->BindAxis("ClimbLadderUp", this, &AXYZPlayerController::ClimbLadderUp);
	
	InputComponent->BindAction("InteracteWithLadder",EInputEvent::IE_Pressed, this, &AXYZPlayerController::InteractWithLadder);
	InputComponent->BindAction("InteracteWithZipline", EInputEvent::IE_Pressed, this, &AXYZPlayerController::InteractWithZipline);
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &AXYZPlayerController::Mantle);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AXYZPlayerController::Jump);
	InputComponent->BindAction("Slide", EInputEvent::IE_Pressed, this, &AXYZPlayerController::Slide);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AXYZPlayerController::ChangeCrouchState);
	InputComponent->BindAction("Crawl", EInputEvent::IE_Pressed, this, &AXYZPlayerController::ChangeCrawlState);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AXYZPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AXYZPlayerController::StopSprint);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AXYZPlayerController::PlayerStartFire);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &AXYZPlayerController::PlayerStopFire);
	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &AXYZPlayerController::StartAiming);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &AXYZPlayerController::StopAiming);
	InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &AXYZPlayerController::Reload);
	InputComponent->BindAction("NextItem", EInputEvent::IE_Pressed, this, &AXYZPlayerController::NextItem);
	InputComponent->BindAction("PreviousItem", EInputEvent::IE_Pressed, this, &AXYZPlayerController::PreviousItem);
	InputComponent->BindAction("EquipPrimaryItem", EInputEvent::IE_Pressed, this, &AXYZPlayerController::EquipPrimaryItem);
}

void AXYZPlayerController::MoveForward(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveForward(Value);
	}
}

void AXYZPlayerController::MoveRight(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveRight(Value);
	}
}

void AXYZPlayerController::Turn(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Turn(Value);
	}
}

void AXYZPlayerController::TurnAtRate(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->TurnAtRate(Value);
	}
}

void AXYZPlayerController::LookUp(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUp(Value);
	}
}

void AXYZPlayerController::LookUpAtRate(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUpAtRate(Value);
	}
}

void AXYZPlayerController::Slide()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Slide();
	}
}

void AXYZPlayerController::ChangeCrouchState()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrouchState();
	}
}

void AXYZPlayerController::ChangeCrawlState()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrawlState();
	}
}

void AXYZPlayerController::Mantle()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Mantle();
	}
}

void AXYZPlayerController::Jump()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}

void AXYZPlayerController::StartSprint()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSprint();
	}
}

void AXYZPlayerController::StopSprint()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopSprint();
	}
}

void AXYZPlayerController::SwimForward(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimForward(Value);
	}
}

void AXYZPlayerController::SwimRight(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimRight(Value);
	}
}

void AXYZPlayerController::SwimUp(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimUp(Value);
	}
}

void AXYZPlayerController::ClimbLadderUp(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ClimLadderUp(Value);
	}
}

void AXYZPlayerController::InteractWithLadder()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithLadder();
	}
}

void AXYZPlayerController::InteractWithZipline()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithZipline();
	}
}

void AXYZPlayerController::PlayerStartFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartFire();
	}
}

void AXYZPlayerController::PlayerStopFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopFire();
	}
}

void AXYZPlayerController::StartAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartAiming();
	}
}

void AXYZPlayerController::StopAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopAiming();
	}
}

void AXYZPlayerController::NextItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->NextItem();
	}
}

void AXYZPlayerController::PreviousItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PreviousItem();
	}
}

void AXYZPlayerController::EquipPrimaryItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EquipPrimaryItem();
	}
}

void AXYZPlayerController::Reload()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Reload();
	}
}

void AXYZPlayerController::CreateAndInitializeWidgets()
{
	if(!IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(),PlayerHUDWidgetClass);
		
		if(IsValid(PlayerHUDWidget))
		{
			PlayerHUDWidget->AddToViewport();	
		}
		
	}
	
	if(CachedBaseCharacter.IsValid() && IsValid(PlayerHUDWidget))
	{
		UReticleWidget* ReticleWidget = PlayerHUDWidget->GetReticleWidget();
		if(IsValid(ReticleWidget))
		{
			CachedBaseCharacter->OnAimingStateChanged.AddUFunction(ReticleWidget, FName("OnAimingStateChanged"));
		}

		UAmmoWidget* AmmoWidget = PlayerHUDWidget->GetAmmoWidget();
		if(IsValid(AmmoWidget))
		{
			UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipment->OnCurrentWeaponAmmoChangedEvent.AddUFunction(AmmoWidget, FName("UpdateAmmoCount"));
			CharacterEquipment->OnCurrentThrowableItemAmmoChangedEvent.AddUFunction(AmmoWidget, FName("UpdateGrenadesCount"));
		}

		UCharacterAttributesWidget* CharacterAttributesWidget = PlayerHUDWidget->GetCharacterAttributesWidget();
		if(IsValid(CharacterAttributesWidget))
		{
			UCharacterAttributeComponent* CharacterAttributeComponent = CachedBaseCharacter->GetCharacterAttributeComponent_Mutable();
			CharacterAttributeComponent->OnUpdateHealthPercent.AddUFunction(CharacterAttributesWidget, FName("UpdateHealthPercent"));
			CharacterAttributeComponent->OnUpdateStaminaPercent.AddUFunction(CharacterAttributesWidget, FName("UpdateStaminaPercent"));
			CharacterAttributeComponent->OnUpdateOxygenPercent.AddUFunction(CharacterAttributesWidget, FName("UpdateOxygenPercent"));
		}
	}
}

