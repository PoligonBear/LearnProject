// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterEquipmentComponent.h"
#include "Characters/XYZBaseCharacter.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "XYZ_HomeWork_Types.h"

EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedItemType() const
{
	EEquipableItemType Result = EEquipableItemType::None;

	if (IsValid(CurrentEquippedWeapon))
	{
		Result = CurrentEquippedWeapon->GetItemType();
	}
	return Result;
}

ARangeWeaponItem* UCharacterEquipmentComponent::GetCurrentRangeWeapon() const
{
	return CurrentEquippedWeapon;
}

AThrowableItem* UCharacterEquipmentComponent::GetCurrentThrowableItem() const
{
	return CurrentThrowableItem;
}

bool UCharacterEquipmentComponent::IsEquipping() const
{
	return bIsEquipping;
}

void UCharacterEquipmentComponent::ReloadCurrentWeapon()
{
	check(IsValid(CurrentEquippedWeapon));
	int32 AvaibleAmunition = GetAvaibleAmunitionForCurrentWeapon();
	if(AvaibleAmunition <= 0)
	{
		return;
	}

	CurrentEquippedWeapon->StartReload();
}

void UCharacterEquipmentComponent::ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo, bool bCheckIsFull)
{
	int32 AvaibleAmunition = GetAvaibleAmunitionForCurrentWeapon();
	int32 CurrentAmmo = CurrentEquippedWeapon->GetAmmo();
	int32 AmmoToReload = CurrentEquippedWeapon->GetMaxAmmo() - CurrentAmmo;
	int32 ReloadedAmmo = FMath::Min(AvaibleAmunition, AmmoToReload);
	if(NumberOfAmmo > 0)
	{
		ReloadedAmmo = FMath::Min(ReloadedAmmo, NumberOfAmmo);
	}

	AmunationArray[(uint32)CurrentEquippedWeapon->GetAmmoType()] -= ReloadedAmmo;
	CurrentEquippedWeapon->SetAmmo(ReloadedAmmo + CurrentAmmo);

	if(bCheckIsFull)
	{
		AvaibleAmunition = AmunationArray[(uint32)CurrentEquippedWeapon->GetAmmoType()];
		bool bIsFullyReloaded = CurrentEquippedWeapon->GetAmmo() == CurrentEquippedWeapon->GetMaxAmmo();

		if(AvaibleAmunition == 0 || bIsFullyReloaded)
		{
			CurrentEquippedWeapon->EndReload(true);
		}
	}
}

void UCharacterEquipmentComponent::UnEquipCurrentItem()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetUnEquippedSocketName());
		CurrentEquippedItem->UnEquip();
	}

	if (IsValid(CurrentEquippedWeapon))
	{
		CurrentEquippedWeapon->StopFire();
		CurrentEquippedWeapon->EndReload(false);
		CurrentEquippedWeapon->OnAmmoChanged.Remove(OnCurrentWeaponAmmoChangedHandle);
		CurrentEquippedWeapon->OnReloadComplete.Remove(OnCurrentWeaponReloadedHandle);
	}

	PreviousEquippedSlot = CurrentEquippedSlot;
	CurrentEquippedSlot = EEquipmentSlots::None;
	
}

void UCharacterEquipmentComponent::AttachCurrentItemToEquippedSocket()
{
	if(IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetEquippedSocketName());
	}
}

void UCharacterEquipmentComponent::LaunchCurrentThrowableItem()
{
	if(IsValid(CurrentThrowableItem))
	{
		if(AmunationArray[(uint32)CurrentThrowableItem->GetAmmoType()] > 0)
		{
			AmunationArray[(uint32)CurrentThrowableItem->GetAmmoType()] -= 1;
		}
		
		OnCurrentThrowableItemAmmoChanged(GetAvaibleAmunitionForCurrentThrowableItem());
		CurrentThrowableItem->Throw();
		bIsEquipping = false;
		EquipItemInSlot(PreviousEquippedSlot);
	}
}

void UCharacterEquipmentComponent::EquipItemInSlot(EEquipmentSlots Slot)
{
	if(bIsEquipping)
	{
		return;
	}
	
	UnEquipCurrentItem();
	CurrentEquippedItem = ItemsArray[(uint32)Slot];
	CurrentEquippedWeapon = Cast<ARangeWeaponItem>(CurrentEquippedItem);
	CurrentThrowableItem = Cast<AThrowableItem>(CurrentEquippedItem);
	
	if (IsValid(CurrentEquippedItem))
	{			
		UAnimMontage* EquipMontage = CurrentEquippedItem->GetCharacterEquipAnimMontage();

		if(AmunationArray[(uint32)CurrentThrowableItem->GetAmmoType()] == 0)
		{
			return;
		}
		
		if(IsValid(EquipMontage))
		{
			bIsEquipping = true;
			UAnimInstance* CharacterAnimInstnce = CachedBaseCharacter->GetMesh()->GetAnimInstance();
			float EquipDuration = CharacterAnimInstnce->Montage_Play(EquipMontage, 1.0f, EMontagePlayReturnType::Duration);
			GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &UCharacterEquipmentComponent::EquipAnimationFinished, EquipDuration, false);
		}
		else
		{
			AttachCurrentItemToEquippedSocket();	
		}
		
		CurrentEquippedSlot = Slot;
		CurrentEquippedItem->Equip();
	}
	
	if (IsValid(CurrentEquippedWeapon))
	{
		OnCurrentWeaponAmmoChangedHandle = CurrentEquippedWeapon->OnAmmoChanged.AddUFunction(this, FName("OnCurrentWeaponAmmoChanged"));
		OnCurrentWeaponReloadedHandle = CurrentEquippedWeapon->OnReloadComplete.AddUFunction(this, FName("OnWeaponReloadComplete"));
		OnCurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetAmmo());
	}
}

void UCharacterEquipmentComponent::EquipNextItem()
{
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 NextSlotIndex = NextItemsArraySlotIndex(CurrentSlotIndex);

	while (CurrentSlotIndex != NextSlotIndex && IgnoreSlotsWhileSwitching.Contains((EEquipmentSlots)NextSlotIndex) || !IsValid(ItemsArray[NextSlotIndex]))
	{
		NextSlotIndex = NextItemsArraySlotIndex(NextSlotIndex);
	}

	if (CurrentSlotIndex != NextSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)NextSlotIndex);
	}
}

void UCharacterEquipmentComponent::EquipPreviousItem()
{
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 PreviousSlotIndex = PrevioustItemsArraySlotIndex(CurrentSlotIndex);

	while (CurrentSlotIndex != PreviousSlotIndex  && IgnoreSlotsWhileSwitching.Contains((EEquipmentSlots)PreviousSlotIndex) || !IsValid(ItemsArray[PreviousSlotIndex]))
	{
		PreviousSlotIndex = PrevioustItemsArraySlotIndex(PreviousSlotIndex);
	}

	if (CurrentSlotIndex != PreviousSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)PreviousSlotIndex);
	}
}

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(GetOwner()->IsA<AXYZBaseCharacter>(), TEXT("UCharacterEquipmentComponent::BeginPlay() CharacterEquipmentComponent can be use with a XYZBaseCharacter"));
	CachedBaseCharacter = StaticCast<AXYZBaseCharacter*>(GetOwner());
	CreateLoadout();
}

void UCharacterEquipmentComponent::OnWeaponReloadComplete()
{
	ReloadAmmoInCurrentWeapon();
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	AmunationArray.AddZeroed((uint32)EAmunitionType::MAX);
	for(const TPair<EAmunitionType, int32>& AmmoPair : MaxAmunitionAmount)
	{
		AmunationArray[(uint32)AmmoPair.Key] = FMath::Max(AmmoPair.Value, 0);
	}

	ItemsArray.AddZeroed((uint32)EEquipmentSlots::MAX);
	for (const TPair<EEquipmentSlots, TSubclassOf<AEquipableItem>>& ItemPair : ItemsLoadout)
	{
		if (!IsValid(ItemPair.Value))
		{
			continue;
		}
		AEquipableItem* Item = GetWorld()->SpawnActor<AEquipableItem>(ItemPair.Value);
		Item->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Item->GetUnEquippedSocketName());
		Item->SetOwner(CachedBaseCharacter.Get());
		Item->UnEquip();
		ItemsArray[(uint32)ItemPair.Key] = Item;
	}
}

void UCharacterEquipmentComponent::EquipAnimationFinished()
{
	bIsEquipping = false;
	AttachCurrentItemToEquippedSocket();
}

int32 UCharacterEquipmentComponent::GetAvaibleAmunitionForCurrentWeapon()
{
	check(GetCurrentRangeWeapon());
	return AmunationArray[(uint32)GetCurrentRangeWeapon()->GetAmmoType()];
}

int32 UCharacterEquipmentComponent::GetAvaibleAmunitionForCurrentThrowableItem()
{
	check(GetCurrentThrowableItem());
	return AmunationArray[(uint32)CurrentThrowableItem->GetAmmoType()];
	/*
	if(!IsValid(CurrentThrowableItem))
	{
		return 0;
	}
	else
	{
		return AmunationArray[(uint32)GetCurrentThrowableItem()->GetAmmoType()];
	}
	*/
}

uint32 UCharacterEquipmentComponent::NextItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == ItemsArray.Num() - 1)
	{
		return 0;
	}
	else
	{
		return CurrentSlotIndex + 1;
	}
}

uint32 UCharacterEquipmentComponent::PrevioustItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == 0)
	{
		return ItemsArray.Num() - 1;
	}
	else
	{
		return CurrentSlotIndex - 1;
	}
}

void UCharacterEquipmentComponent::OnCurrentWeaponAmmoChanged(int32 Ammo)
{
	if(OnCurrentWeaponAmmoChangedEvent.IsBound())
	{
		OnCurrentWeaponAmmoChangedEvent.Broadcast(Ammo, GetAvaibleAmunitionForCurrentWeapon());
	}
}

void UCharacterEquipmentComponent::OnCurrentThrowableItemAmmoChanged(int32 Grenades)
{
	if(OnCurrentThrowableItemAmmoChangedEvent.IsBound())
	{
		OnCurrentThrowableItemAmmoChangedEvent.Broadcast(GetAvaibleAmunitionForCurrentThrowableItem());
	}
}
