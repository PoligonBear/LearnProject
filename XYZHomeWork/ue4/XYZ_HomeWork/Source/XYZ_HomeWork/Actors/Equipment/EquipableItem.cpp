#include "EquipableItem.h"

EEquipableItemType AEquipableItem::GetItemType() const
{
	return ItemType;
}

FName AEquipableItem::GetUnEquippedSocketName() const
{
	return UnEquippedSocketName;
}

FName AEquipableItem::GetEquippedSocketName() const
{
	return EquippedSocketName;
}

UAnimMontage* AEquipableItem::GetCharacterEquipAnimMontage() const
{
	return CharacterEquipAnimMontage;
}

void AEquipableItem::Equip()
{
	if(OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(true);
	}
	
}

void AEquipableItem::UnEquip()
{
	if(OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(false);
	}
}
