#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EquipmentTypes.generated.h"

UENUM(BlueprintType)
enum class EDustlandEquipmentSlot : uint8
{
	Head,
	Mask,
	Torso,
	Legs,
	Feet,
	Gloves,
	Backpack,
	Accessory1,
	Accessory2,
	None
};

/**
 * FDustlandEquipmentDefinition - one DataTable row (Content/Data/DT_Equipment)
 * describing the gameplay stats of an equippable item. The ItemID here must
 * match an FDustlandItemDefinition row in the Inventory item table; this is
 * a separate table so armor/clothing stats stay out of the generic item
 * definition used by every non-equippable item too.
 */
USTRUCT(BlueprintType)
struct EQUIPMENT_API FDustlandEquipmentDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	EDustlandEquipmentSlot Slot = EDustlandEquipmentSlot::Torso;

	/** 0-1 contribution toward USurvivalComponent::EquipmentInsulationFactor when equipped. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float InsulationValue = 0.f;

	/** Abstract protection value; reduces incoming Gameplay damage before other mitigation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (ClampMin = "0.0"))
	float ArmorValue = 0.f;

	/** Additional carry capacity in kg while equipped (meaningful for Backpack slot). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (ClampMin = "0.0"))
	float BonusCarryWeightKg = 0.f;

	/** Additional carry volume while equipped (meaningful for Backpack slot). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (ClampMin = "0.0"))
	float BonusCarryVolume = 0.f;

	/** Condition lost per in-game hour of active use (0 = never degrades passively). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (ClampMin = "0.0"))
	float ConditionLossPerHour = 0.1f;
};
