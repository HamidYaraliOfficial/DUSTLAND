#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftingKnowledgeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecipeUnlocked, FName, RecipeID);

/**
 * UCraftingKnowledgeComponent
 *
 * Put this on the player (or, later, on a Settlement/NPC that can craft).
 * Recipes flagged bUnlockedByDefault on the FDustlandRecipeDefinition are
 * implicitly known without needing an entry here; everything else must be
 * unlocked via UnlockRecipe (called by Exploration pickups, NPC teaching,
 * Research completion, or reading a found document/blueprint).
 */
UCLASS(ClassGroup = (Dustland), meta = (BlueprintSpawnableComponent))
class CRAFTING_API UCraftingKnowledgeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Crafting")
	TSet<FName> KnownRecipeIDs;

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool UnlockRecipe(FName RecipeID);

	UFUNCTION(BlueprintPure, Category = "Crafting")
	bool HasRecipe(FName RecipeID) const { return KnownRecipeIDs.Contains(RecipeID); }

	UPROPERTY(BlueprintAssignable, Category = "Crafting|Events")
	FOnRecipeUnlocked OnRecipeUnlocked;
};
