#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "DustlandItemTypes.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryOverburdened, bool, bIsOverburdened);

/**
 * UInventoryComponent
 *
 * A weight- and volume-limited container of FDustlandItemInstance stacks.
 * Works identically for the player backpack, a vehicle's cargo bed, a
 * settlement storage building or a lootable container in the world - only
 * the capacity numbers differ, which is exactly what lets Scavenging,
 * Settlement Storage and Vehicle Storage all reuse this one component.
 */
UCLASS(ClassGroup = (Dustland), meta = (BlueprintSpawnableComponent))
class INVENTORY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	/** DataTable of FDustlandItemDefinition rows. Assign Content/Data/DT_Items in the editor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Config")
	UDataTable* ItemDefinitionTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Config", meta = (ClampMin = "0.0"))
	float MaxWeightKg = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Config", meta = (ClampMin = "0.0"))
	float MaxVolume = 60.f;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|State")
	TArray<FDustlandItemInstance> Items;

	/** Attempts to add Count of ItemID. Returns how many actually fit (may be less than Count if capacity-limited). */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 AddItem(FName ItemID, int32 Count = 1, float Condition = 100.f);

	/** Removes up to Count of ItemID (searching oldest stacks first). Returns how many were actually removed. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveItem(FName ItemID, int32 Count = 1);

	/** Moves Count of a specific instance into another inventory component (looting, trading, storing). */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool TransferItem(UInventoryComponent* TargetInventory, FGuid InstanceGuid, int32 Count);

	/**
	 * Pulls exactly one unit of the stack identified by InstanceGuid out of
	 * this inventory (decrementing/removing the stack) and returns a copy of
	 * it (StackCount = 1, same ItemID/Condition) via OutExtracted. Used by
	 * UEquipmentComponent::EquipItem so the equipped copy keeps its exact
	 * Condition value. Returns false if InstanceGuid isn't found here.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ExtractInstance(FGuid InstanceGuid, FDustlandItemInstance& OutExtracted);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemCount(FName ItemID) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	float GetCurrentWeight() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	float GetCurrentVolume() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsOverburdened() const { return GetCurrentWeight() > MaxWeightKg; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FDustlandItemInstance> GetItemsWithTag(FName Tag) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool FindItemDefinition(FName ItemID, FDustlandItemDefinition& OutDefinition) const;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryChanged OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryOverburdened OnInventoryOverburdened;

private:
	bool bLastOverburdenedState = false;
	void CheckOverburdenedStateChanged();
};
