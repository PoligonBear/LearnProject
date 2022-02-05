// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XYZ_HomeWork_Types.h"
#include "Actors/Equipment/Throwables/ThrowableItem.h"
#include "CharacterEquipmentComponent.generated.h"

typedef TArray<class AEquipableItem*, TInlineAllocator<(uint32)EEquipmentSlots::MAX>> TItemsArray;
typedef TArray<int32, TInlineAllocator<(uint32)EAmunitionType::MAX>> TAmunitionArray;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChanged, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentThrowableItemAmmoChanged, int32);

class AThrowableItem;
class ARangeWeaponItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XYZ_HOMEWORK_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	EEquipableItemType GetCurrentEquippedItemType() const;
	
	ARangeWeaponItem* GetCurrentRangeWeapon() const;
	AThrowableItem* GetCurrentThrowableItem() const;

	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;
	FOnCurrentThrowableItemAmmoChanged OnCurrentThrowableItemAmmoChangedEvent;

	bool IsEquipping() const;
	
	void ReloadCurrentWeapon();
	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo = 0, bool bCheckIsFull = false);
	
	void UnEquipCurrentItem();
	void AttachCurrentItemToEquippedSocket();
	void LaunchCurrentThrowableItem();
	void EquipItemInSlot(EEquipmentSlots Slot);
	void EquipNextItem();
	void EquipPreviousItem();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLoadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSet<EEquipmentSlots> IgnoreSlotsWhileSwitching;

private:
	TAmunitionArray AmunationArray;
	TItemsArray ItemsArray;

	UFUNCTION()
	void OnWeaponReloadComplete();
	
	void CreateLoadout();
	void EquipAnimationFinished();

	int32 GetAvaibleAmunitionForCurrentWeapon();
	int32 GetAvaibleAmunitionForCurrentThrowableItem();
	
	uint32 NextItemsArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PrevioustItemsArraySlotIndex(uint32 CurrentSlotIndex);

	bool bIsEquipping = false;

	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 Ammo);

	UFUNCTION()
	void OnCurrentThrowableItemAmmoChanged(int32 Grenades);

	FDelegateHandle OnCurrentWeaponAmmoChangedHandle;
	FDelegateHandle OnCurrentWeaponReloadedHandle;

	FTimerHandle EquipTimer;

	EEquipmentSlots PreviousEquippedSlot;
	EEquipmentSlots CurrentEquippedSlot;
	
	AEquipableItem* CurrentEquippedItem = nullptr;
	ARangeWeaponItem* CurrentEquippedWeapon = nullptr;
	AThrowableItem* CurrentThrowableItem = nullptr;
	
	TWeakObjectPtr<class AXYZBaseCharacter> CachedBaseCharacter;
};
