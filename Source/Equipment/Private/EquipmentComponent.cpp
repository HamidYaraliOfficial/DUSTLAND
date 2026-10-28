#include "EquipmentComponent.h"

UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UEquipmentComponent::FindEquipmentDefinition(FName ItemID, FDustlandEquipmentDefinition& OutDefinition) const
{
	if (!EquipmentDefinitionTable) return false;

	static const FString ContextString(TEXT("EquipmentComponent::FindEquipmentDefinition"));
	if (const FDustlandEquipmentDefinition* Row =
		EquipmentDefinitionTable->FindRow<FDustlandEquipmentDefinition>(ItemID, ContextString))
	{
		OutDefinition = *Row;
		return true;
	}
	return false;
}

bool UEquipmentComponent::EquipItem(UInventoryComponent* FromInventory, FGuid InstanceGuid)
{
	if (!FromInventory) return false;

	// Peek at the item's ID first (without removing it) so we can look up its slot.
	const FDustlandItemInstance* Existing = FromInventory->Items.FindByPredicate(
		[&](const FDustlandItemInstance& Inst) { return Inst.InstanceGuid == InstanceGuid; });
	if (!Existing) return false;

	FDustlandEquipmentDefinition Def;
	if (!FindEquipmentDefinition(Existing->ItemID, Def))
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipment: %s has no FDustlandEquipmentDefinition row - not equippable"),
			*Existing->ItemID.ToString());
		return false;
	}

	if (IsSlotOccupied(Def.Slot))
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipment: slot already occupied, unequip first"));
		return false;
	}

	FDustlandItemInstance Extracted;
	if (!FromInventory->ExtractInstance(InstanceGuid, Extracted))
	{
		return false;
	}

	EquippedItems.Add(Def.Slot, Extracted);
	OnEquipmentChanged.Broadcast(Def.Slot);
	return true;
}

bool UEquipmentComponent::UnequipItem(EDustlandEquipmentSlot Slot, UInventoryComponent* ToInventory)
{
	if (!ToInventory) return false;

	FDustlandItemInstance* Instance = EquippedItems.Find(Slot);
	if (!Instance) return false;

	const int32 Added = ToInventory->AddItem(Instance->ItemID, 1, Instance->Condition);
	if (Added <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipment: target inventory has no room to receive unequipped item"));
		return false;
	}

	EquippedItems.Remove(Slot);
	OnEquipmentChanged.Broadcast(Slot);
	return true;
}

float UEquipmentComponent::GetTotalInsulation() const
{
	float Total = 0.f;
	for (const auto& Pair : EquippedItems)
	{
		FDustlandEquipmentDefinition Def;
		if (FindEquipmentDefinition(Pair.Value.ItemID, Def))
		{
			Total += Def.InsulationValue;
		}
	}
	return FMath::Clamp(Total, 0.f, 1.f);
}

float UEquipmentComponent::GetTotalArmorValue() const
{
	float Total = 0.f;
	for (const auto& Pair : EquippedItems)
	{
		FDustlandEquipmentDefinition Def;
		if (FindEquipmentDefinition(Pair.Value.ItemID, Def))
		{
			Total += Def.ArmorValue;
		}
	}
	return Total;
}

float UEquipmentComponent::GetBonusCarryWeight() const
{
	float Total = 0.f;
	for (const auto& Pair : EquippedItems)
	{
		FDustlandEquipmentDefinition Def;
		if (FindEquipmentDefinition(Pair.Value.ItemID, Def))
		{
			Total += Def.BonusCarryWeightKg;
		}
	}
	return Total;
}

float UEquipmentComponent::GetBonusCarryVolume() const
{
	float Total = 0.f;
	for (const auto& Pair : EquippedItems)
	{
		FDustlandEquipmentDefinition Def;
		if (FindEquipmentDefinition(Pair.Value.ItemID, Def))
		{
			Total += Def.BonusCarryVolume;
		}
	}
	return Total;
}

void UEquipmentComponent::DegradeEquippedItems(float DeltaHours)
{
	TArray<EDustlandEquipmentSlot> SlotsToUpdate;
	EquippedItems.GetKeys(SlotsToUpdate);

	for (EDustlandEquipmentSlot Slot : SlotsToUpdate)
	{
		FDustlandItemInstance* Instance = EquippedItems.Find(Slot);
		if (!Instance) continue;

		FDustlandEquipmentDefinition Def;
		if (!FindEquipmentDefinition(Instance->ItemID, Def)) continue;

		Instance->Condition = FMath::Clamp(Instance->Condition - Def.ConditionLossPerHour * DeltaHours, 0.f, 100.f);
	}
}
