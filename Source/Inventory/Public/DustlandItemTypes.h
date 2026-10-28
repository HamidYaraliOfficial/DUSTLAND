#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DustlandTypes.h"
#include "DustlandItemTypes.generated.h"

/**
 * FDustlandItemDefinition = the static, designer-authored template for an
 * item, stored as one row of a DataTable (Content/Data/DT_Items). This is
 * the Data-Driven half of the item system: designers add new items by
 * adding rows, never by writing C++.
 */
USTRUCT(BlueprintType)
struct INVENTORY_API FDustlandItemDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EDustlandItemType ItemType = EDustlandItemType::Misc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EDustlandItemRarity Rarity = EDustlandItemRarity::Common;

	/** Kilograms, used against carry-capacity limits. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = "0.0"))
	float Weight = 0.1f;

	/** Abstract volume units, used against container/backpack slot limits. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = "0.0"))
	float Volume = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = "1"))
	int32 MaxStackSize = 1;

	/** Base trade value in the abstract in-world currency ("Scrip"). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = "0"))
	int32 BaseValue = 0;

	/** For Food/Water items: how much Hunger/Thirst is restored (0-100 scale). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Consumable")
	float RestoreAmount = 0.f;

	/** Whether the item degrades with use (weapons, tools, armor, vehicles parts). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bHasCondition = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FDustlandItemTags Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSoftObjectPtr<UTexture2D> Icon;
};

/**
 * FDustlandItemInstance = one concrete stack sitting in an inventory. Points
 * back at its FDustlandItemDefinition by ItemID (resolved through the
 * DataTable at runtime) and carries only the state that varies per instance:
 * stack count and condition (0-100, 100 = pristine).
 */
USTRUCT(BlueprintType)
struct INVENTORY_API FDustlandItemInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = "1"))
	int32 StackCount = 1;

	/** 0-100. Only meaningful when the definition's bHasCondition is true. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Condition = 100.f;

	/** Unique per-instance ID so two stacks of the same item never collide in UI selection / drag-drop. */
	UPROPERTY(BlueprintReadOnly, Category = "Item")
	FGuid InstanceGuid;

	FDustlandItemInstance()
	{
		InstanceGuid = FGuid::NewGuid();
	}

	bool IsValid() const { return !ItemID.IsNone() && StackCount > 0; }
};
