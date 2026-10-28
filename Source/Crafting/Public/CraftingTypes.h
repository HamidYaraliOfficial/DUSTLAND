#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CraftingTypes.generated.h"

USTRUCT(BlueprintType)
struct FDustlandCraftingIngredient
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting", meta = (ClampMin = "1"))
	int32 Count = 1;
};

/**
 * FDustlandRecipeDefinition - one DataTable row (Content/Data/DT_Recipes).
 * Recipes are unlocked through Exploration/NPC/Research/Found Documents by
 * adding the RecipeID to a UCraftingKnowledgeComponent - this struct only
 * describes the recipe itself, not who currently knows it.
 */
USTRUCT(BlueprintType)
struct CRAFTING_API FDustlandRecipeDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	FName RecipeID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	TArray<FDustlandCraftingIngredient> Ingredients;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	FName OutputItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting", meta = (ClampMin = "1"))
	int32 OutputCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting", meta = (ClampMin = "0.0"))
	float CraftingTimeSeconds = 3.f;

	/**
	 * Tag identifying which station type can craft this recipe (e.g.
	 * "Station.Workbench", "Station.ElectronicsBench"). None/empty means it
	 * can be crafted anywhere (pocket crafting).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	FName RequiredStationTag;

	/** If true, every UCraftingKnowledgeComponent starts knowing this recipe without needing to unlock it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	bool bUnlockedByDefault = false;
};

/** One in-progress craft on a station: which recipe, how many seconds remain, and which inventory receives the output. */
USTRUCT(BlueprintType)
struct FDustlandCraftingJob
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName RecipeID;

	UPROPERTY(BlueprintReadOnly)
	float RemainingSeconds = 0.f;

	UPROPERTY()
	TWeakObjectPtr<class UInventoryComponent> OutputInventory;

	UPROPERTY(BlueprintReadOnly)
	FGuid JobGuid;

	FDustlandCraftingJob()
	{
		JobGuid = FGuid::NewGuid();
	}
};
