#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

bool UInventoryComponent::FindItemDefinition(FName ItemID, FDustlandItemDefinition& OutDefinition) const
{
	if (!ItemDefinitionTable) return false;

	static const FString ContextString(TEXT("InventoryComponent::FindItemDefinition"));
	if (const FDustlandItemDefinition* Row =
		ItemDefinitionTable->FindRow<FDustlandItemDefinition>(ItemID, ContextString))
	{
		OutDefinition = *Row;
		return true;
	}
	return false;
}

int32 UInventoryComponent::AddItem(FName ItemID, int32 Count, float Condition)
{
	if (Count <= 0) return 0;

	FDustlandItemDefinition Def;
	if (!FindItemDefinition(ItemID, Def))
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory: unknown ItemID %s"), *ItemID.ToString());
		return 0;
	}

	int32 Remaining = Count;

	// Fill existing, non-full stacks first (only meaningful for stackable, no-condition items).
	if (Def.MaxStackSize > 1 && !Def.bHasCondition)
	{
		for (FDustlandItemInstance& Instance : Items)
		{
			if (Remaining <= 0) break;
			if (Instance.ItemID != ItemID) continue;

			const int32 SpaceInStack = Def.MaxStackSize - Instance.StackCount;
			if (SpaceInStack <= 0) continue;

			// Respect weight/volume capacity before committing.
			const float ProjectedWeight = GetCurrentWeight() + Def.Weight * FMath::Min(SpaceInStack, Remaining);
			const float ProjectedVolume = GetCurrentVolume() + Def.Volume * FMath::Min(SpaceInStack, Remaining);
			if (ProjectedWeight > MaxWeightKg || ProjectedVolume > MaxVolume) break;

			const int32 ToAdd = FMath::Min(SpaceInStack, Remaining);
			Instance.StackCount += ToAdd;
			Remaining -= ToAdd;
		}
	}

	// Create new stacks for whatever is left, respecting capacity one stack at a time.
	while (Remaining > 0)
	{
		const int32 StackAmount = (Def.MaxStackSize > 1) ? FMath::Min(Remaining, Def.MaxStackSize) : 1;

		const float ProjectedWeight = GetCurrentWeight() + Def.Weight * StackAmount;
		const float ProjectedVolume = GetCurrentVolume() + Def.Volume * StackAmount;
		if (ProjectedWeight > MaxWeightKg || ProjectedVolume > MaxVolume)
		{
			break; // no more room; whatever wasn't added is reported to the caller
		}

		FDustlandItemInstance NewInstance;
		NewInstance.ItemID = ItemID;
		NewInstance.StackCount = StackAmount;
		NewInstance.Condition = Def.bHasCondition ? Condition : 100.f;
		Items.Add(NewInstance);

		Remaining -= StackAmount;
	}

	const int32 ActuallyAdded = Count - Remaining;
	if (ActuallyAdded > 0)
	{
		OnInventoryChanged.Broadcast();
		CheckOverburdenedStateChanged();
	}
	return ActuallyAdded;
}

int32 UInventoryComponent::RemoveItem(FName ItemID, int32 Count)
{
	if (Count <= 0) return 0;

	int32 Remaining = Count;
	for (int32 i = Items.Num() - 1; i >= 0 && Remaining > 0; --i)
	{
		if (Items[i].ItemID != ItemID) continue;

		const int32 ToRemove = FMath::Min(Remaining, Items[i].StackCount);
		Items[i].StackCount -= ToRemove;
		Remaining -= ToRemove;

		if (Items[i].StackCount <= 0)
		{
			Items.RemoveAt(i);
		}
	}

	const int32 ActuallyRemoved = Count - Remaining;
	if (ActuallyRemoved > 0)
	{
		OnInventoryChanged.Broadcast();
		CheckOverburdenedStateChanged();
	}
	return ActuallyRemoved;
}

bool UInventoryComponent::TransferItem(UInventoryComponent* TargetInventory, FGuid InstanceGuid, int32 Count)
{
	if (!TargetInventory || Count <= 0) return false;

	FDustlandItemInstance* Found = Items.FindByPredicate(
		[&](const FDustlandItemInstance& Inst) { return Inst.InstanceGuid == InstanceGuid; });
	if (!Found) return false;

	const int32 ToMove = FMath::Min(Count, Found->StackCount);
	const int32 Added = TargetInventory->AddItem(Found->ItemID, ToMove, Found->Condition);
	if (Added <= 0) return false;

	Found->StackCount -= Added;
	if (Found->StackCount <= 0)
	{
		Items.RemoveAll([&](const FDustlandItemInstance& Inst) { return Inst.InstanceGuid == InstanceGuid; });
	}

	OnInventoryChanged.Broadcast();
	CheckOverburdenedStateChanged();
	return true;
}

bool UInventoryComponent::ExtractInstance(FGuid InstanceGuid, FDustlandItemInstance& OutExtracted)
{
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i].InstanceGuid != InstanceGuid) continue;

		OutExtracted.ItemID = Items[i].ItemID;
		OutExtracted.Condition = Items[i].Condition;
		OutExtracted.StackCount = 1;

		Items[i].StackCount -= 1;
		if (Items[i].StackCount <= 0)
		{
			Items.RemoveAt(i);
		}

		OnInventoryChanged.Broadcast();
		CheckOverburdenedStateChanged();
		return true;
	}
	return false;
}

int32 UInventoryComponent::GetItemCount(FName ItemID) const
{
	int32 Total = 0;
	for (const FDustlandItemInstance& Instance : Items)
	{
		if (Instance.ItemID == ItemID) Total += Instance.StackCount;
	}
	return Total;
}

float UInventoryComponent::GetCurrentWeight() const
{
	float Total = 0.f;
	for (const FDustlandItemInstance& Instance : Items)
	{
		FDustlandItemDefinition Def;
		if (FindItemDefinition(Instance.ItemID, Def))
		{
			Total += Def.Weight * Instance.StackCount;
		}
	}
	return Total;
}

float UInventoryComponent::GetCurrentVolume() const
{
	float Total = 0.f;
	for (const FDustlandItemInstance& Instance : Items)
	{
		FDustlandItemDefinition Def;
		if (FindItemDefinition(Instance.ItemID, Def))
		{
			Total += Def.Volume * Instance.StackCount;
		}
	}
	return Total;
}

TArray<FDustlandItemInstance> UInventoryComponent::GetItemsWithTag(FName Tag) const
{
	TArray<FDustlandItemInstance> Result;
	for (const FDustlandItemInstance& Instance : Items)
	{
		FDustlandItemDefinition Def;
		if (FindItemDefinition(Instance.ItemID, Def) && Def.Tags.Has(Tag))
		{
			Result.Add(Instance);
		}
	}
	return Result;
}

void UInventoryComponent::CheckOverburdenedStateChanged()
{
	const bool bNow = IsOverburdened();
	if (bNow != bLastOverburdenedState)
	{
		bLastOverburdenedState = bNow;
		OnInventoryOverburdened.Broadcast(bNow);
	}
}
