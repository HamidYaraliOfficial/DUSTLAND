#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "CraftingTypes.h"
#include "CraftingStationComponent.generated.h"

class UInventoryComponent;
class UCraftingKnowledgeComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftingJobStarted, FName, RecipeID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftingJobCompleted, FName, RecipeID);

/**
 * UCraftingStationComponent
 *
 * Put this on a Workbench/Workshop/Repair Bench/Electronics Bench/Fabrication
 * Station actor. StationTag identifies which recipes it can build (empty tag
 * = a "pocket crafting" station that accepts any recipe with no station
 * requirement). Crafting is time-based: StartCraft() validates and consumes
 * ingredients immediately (so the same materials can't be double-spent on
 * two simultaneous jobs) and queues a job that TickComponent resolves after
 * the recipe's CraftingTimeSeconds elapses.
 */
UCLASS(ClassGroup = (Dustland), meta = (BlueprintSpawnableComponent))
class CRAFTING_API UCraftingStationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCraftingStationComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting|Config")
	UDataTable* RecipeTable = nullptr;

	/** e.g. "Station.Workbench". Empty = accepts any recipe with no RequiredStationTag. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting|Config")
	FName StationTag;

	/** How many crafts can run concurrently on this station (a real Workshop might run 2-3 in parallel). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting|Config", meta = (ClampMin = "1"))
	int32 MaxConcurrentJobs = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Crafting|State")
	TArray<FDustlandCraftingJob> ActiveJobs;

	UFUNCTION(BlueprintPure, Category = "Crafting")
	bool FindRecipe(FName RecipeID, FDustlandRecipeDefinition& OutRecipe) const;

	/**
	 * Checks station compatibility, known-recipe requirement and ingredient
	 * availability without consuming anything. OutReason is filled with a
	 * short machine-readable code on failure ("UnknownRecipe", "WrongStation",
	 * "NotUnlocked", "MissingIngredients", "StationBusy") for UI to localize.
	 */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool CanCraft(FName RecipeID, UInventoryComponent* SourceInventory,
		UCraftingKnowledgeComponent* Knowledge, FString& OutReason) const;

	/** Consumes ingredients from SourceInventory immediately and queues a timed job whose output lands in OutputInventory. */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool StartCraft(FName RecipeID, UInventoryComponent* SourceInventory,
		UCraftingKnowledgeComponent* Knowledge, UInventoryComponent* OutputInventory, FString& OutReason);

	UPROPERTY(BlueprintAssignable, Category = "Crafting|Events")
	FOnCraftingJobStarted OnCraftingJobStarted;

	UPROPERTY(BlueprintAssignable, Category = "Crafting|Events")
	FOnCraftingJobCompleted OnCraftingJobCompleted;

private:
	bool HasIngredients(const FDustlandRecipeDefinition& Recipe, UInventoryComponent* SourceInventory) const;
	void ConsumeIngredients(const FDustlandRecipeDefinition& Recipe, UInventoryComponent* SourceInventory) const;
};
