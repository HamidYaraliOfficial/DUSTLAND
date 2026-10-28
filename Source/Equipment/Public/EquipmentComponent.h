#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "EquipmentTypes.h"
#include "DustlandItemTypes.h"
#include "InventoryComponent.h"
#include "EquipmentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentChanged, EDustlandEquipmentSlot, ChangedSlot);

/**
 * UEquipmentComponent
 *
 * Holds up to one FDustlandItemInstance per EDustlandEquipmentSlot. Moving
 * an item into a slot removes it from the source UInventoryComponent (so it
 * stops counting toward backpack weight); moving it out returns it there.
 * Aggregated stats (insulation, armor, bonus carry capacity) are meant to be
 * read once per relevant tick by the owning pawn or by the World Manager
 * (see ADustlandGameModeBase) and pushed into USurvivalComponent /
 * UInventoryComponent - this component does not reach into those directly,
 * keeping Equipment decoupled from Survival.
 */
UCLASS(ClassGroup = (Dustland), meta = (BlueprintSpawnableComponent))
class EQUIPMENT_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Config")
	UDataTable* EquipmentDefinitionTable = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Equipment|State")
	TMap<EDustlandEquipmentSlot, FDustlandItemInstance> EquippedItems;

	/** Moves InstanceGuid from FromInventory into its defined slot. Fails if the slot is already occupied (unequip first) or the item isn't equipment. */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool EquipItem(UInventoryComponent* FromInventory, FGuid InstanceGuid);

	/** Moves whatever is in Slot back into ToInventory. Fails if ToInventory has no room. */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool UnequipItem(EDustlandEquipmentSlot Slot, UInventoryComponent* ToInventory);

	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool IsSlotOccupied(EDustlandEquipmentSlot Slot) const { return EquippedItems.Contains(Slot); }

	UFUNCTION(BlueprintPure, Category = "Equipment")
	float GetTotalInsulation() const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	float GetTotalArmorValue() const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	float GetBonusCarryWeight() const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	float GetBonusCarryVolume() const;

	/** Reduces Condition on every equipped item that degrades with use. Call once per in-game hour elapsed. */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void DegradeEquippedItems(float DeltaHours);

	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool FindEquipmentDefinition(FName ItemID, FDustlandEquipmentDefinition& OutDefinition) const;

	UPROPERTY(BlueprintAssignable, Category = "Equipment|Events")
	FOnEquipmentChanged OnEquipmentChanged;
};
