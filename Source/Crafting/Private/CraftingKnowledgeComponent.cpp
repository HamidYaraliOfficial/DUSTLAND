#include "CraftingKnowledgeComponent.h"

bool UCraftingKnowledgeComponent::UnlockRecipe(FName RecipeID)
{
	if (RecipeID.IsNone() || KnownRecipeIDs.Contains(RecipeID))
	{
		return false;
	}

	KnownRecipeIDs.Add(RecipeID);
	OnRecipeUnlocked.Broadcast(RecipeID);
	return true;
}
