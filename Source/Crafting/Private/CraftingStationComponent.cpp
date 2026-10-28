#include "CraftingStationComponent.h"
#include "CraftingKnowledgeComponent.h"
#include "InventoryComponent.h"

UCraftingStationComponent::UCraftingStationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UCraftingStationComponent::FindRecipe(FName RecipeID, FDustlandRecipeDefinition& OutRecipe) const
{
	if (!RecipeTable) return false;

	static const FString ContextString(TEXT("CraftingStationComponent::FindRecipe"));
	if (const FDustlandRecipeDefinition* Row = RecipeTable->FindRow<FDustlandRecipeDefinition>(RecipeID, ContextString))
	{
		OutRecipe = *Row;
		return true;
	}
	return false;
}

bool UCraftingStationComponent::HasIngredients(const FDustlandRecipeDefinition& Recipe, UInventoryComponent* SourceInventory) const
{
	if (!SourceInventory) return false;

	for (const FDustlandCraftingIngredient& Ingredient : Recipe.Ingredients)
	{
		if (SourceInventory->GetItemCount(Ingredient.ItemID) < Ingredient.Count)
		{
			return false;
		}
	}
	return true;
}

void UCraftingStationComponent::ConsumeIngredients(const FDustlandRecipeDefinition& Recipe, UInventoryComponent* SourceInventory) const
{
	for (const FDustlandCraftingIngredient& Ingredient : Recipe.Ingredients)
	{
		SourceInventory->RemoveItem(Ingredient.ItemID, Ingredient.Count);
	}
}

bool UCraftingStationComponent::CanCraft(FName RecipeID, UInventoryComponent* SourceInventory,
	UCraftingKnowledgeComponent* Knowledge, FString& OutReason) const
{
	FDustlandRecipeDefinition Recipe;
	if (!FindRecipe(RecipeID, Recipe))
	{
		OutReason = TEXT("UnknownRecipe");
		return false;
	}

	if (!Recipe.RequiredStationTag.IsNone() && Recipe.RequiredStationTag != StationTag)
	{
		OutReason = TEXT("WrongStation");
		return false;
	}

	if (!Recipe.bUnlockedByDefault)
	{
		if (!Knowledge || !Knowledge->HasRecipe(RecipeID))
		{
			OutReason = TEXT("NotUnlocked");
			return false;
		}
	}

	if (!HasIngredients(Recipe, SourceInventory))
	{
		OutReason = TEXT("MissingIngredients");
		return false;
	}

	if (ActiveJobs.Num() >= MaxConcurrentJobs)
	{
		OutReason = TEXT("StationBusy");
		return false;
	}

	OutReason.Empty();
	return true;
}

bool UCraftingStationComponent::StartCraft(FName RecipeID, UInventoryComponent* SourceInventory,
	UCraftingKnowledgeComponent* Knowledge, UInventoryComponent* OutputInventory, FString& OutReason)
{
	if (!CanCraft(RecipeID, SourceInventory, Knowledge, OutReason))
	{
		return false;
	}

	FDustlandRecipeDefinition Recipe;
	FindRecipe(RecipeID, Recipe); // guaranteed to succeed, CanCraft already validated it

	// Consume ingredients up-front so two jobs can never double-spend the
	// same materials while both are "in progress".
	ConsumeIngredients(Recipe, SourceInventory);

	FDustlandCraftingJob Job;
	Job.RecipeID = RecipeID;
	Job.RemainingSeconds = FMath::Max(0.01f, Recipe.CraftingTimeSeconds);
	Job.OutputInventory = OutputInventory;
	ActiveJobs.Add(Job);

	OnCraftingJobStarted.Broadcast(RecipeID);
	return true;
}

void UCraftingStationComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ActiveJobs.Num() == 0) return;

	for (int32 i = ActiveJobs.Num() - 1; i >= 0; --i)
	{
		FDustlandCraftingJob& Job = ActiveJobs[i];
		Job.RemainingSeconds -= DeltaTime;

		if (Job.RemainingSeconds <= 0.f)
		{
			FDustlandRecipeDefinition Recipe;
			if (FindRecipe(Job.RecipeID, Recipe) && Job.OutputInventory.IsValid())
			{
				Job.OutputInventory->AddItem(Recipe.OutputItemID, Recipe.OutputCount);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Crafting: job for %s completed but output inventory was gone; item lost."),
					*Job.RecipeID.ToString());
			}

			OnCraftingJobCompleted.Broadcast(Job.RecipeID);
			ActiveJobs.RemoveAt(i);
		}
	}
}
