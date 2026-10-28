#include "Misc/AutomationTest.h"
#include "CraftingStationComponent.h"
#include "CraftingKnowledgeComponent.h"
#include "InventoryComponent.h"
#include "Engine/DataTable.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace DustlandCraftingTestHelpers
{
	static UDataTable* MakeItemTable()
	{
		UDataTable* Table = NewObject<UDataTable>();
		Table->RowStruct = FDustlandItemDefinition::StaticStruct();

		FDustlandItemDefinition Scrap;
		Scrap.ItemID = TEXT("Item.Scrap");
		Scrap.MaxStackSize = 99;
		Table->AddRow(TEXT("Item.Scrap"), Scrap);

		FDustlandItemDefinition Fiber;
		Fiber.ItemID = TEXT("Item.Fiber");
		Fiber.MaxStackSize = 99;
		Table->AddRow(TEXT("Item.Fiber"), Fiber);

		FDustlandItemDefinition Bandage;
		Bandage.ItemID = TEXT("Item.Bandage");
		Bandage.MaxStackSize = 20;
		Table->AddRow(TEXT("Item.Bandage"), Bandage);

		return Table;
	}

	static UDataTable* MakeRecipeTable()
	{
		UDataTable* Table = NewObject<UDataTable>();
		Table->RowStruct = FDustlandRecipeDefinition::StaticStruct();

		FDustlandRecipeDefinition BandageRecipe;
		BandageRecipe.RecipeID = TEXT("Recipe.Bandage");
		BandageRecipe.Ingredients.Add({ TEXT("Item.Fiber"), 2 });
		BandageRecipe.OutputItemID = TEXT("Item.Bandage");
		BandageRecipe.OutputCount = 1;
		BandageRecipe.CraftingTimeSeconds = 1.f;
		BandageRecipe.bUnlockedByDefault = true; // no station, no unlock required
		Table->AddRow(TEXT("Recipe.Bandage"), BandageRecipe);

		FDustlandRecipeDefinition LockedRecipe;
		LockedRecipe.RecipeID = TEXT("Recipe.Locked");
		LockedRecipe.Ingredients.Add({ TEXT("Item.Scrap"), 1 });
		LockedRecipe.OutputItemID = TEXT("Item.Bandage");
		LockedRecipe.OutputCount = 1;
		LockedRecipe.CraftingTimeSeconds = 1.f;
		LockedRecipe.bUnlockedByDefault = false;
		Table->AddRow(TEXT("Recipe.Locked"), LockedRecipe);

		return Table;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDustlandCraftingMissingIngredientsTest,
	"Dustland.Crafting.CanCraftFailsWithoutIngredients",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FDustlandCraftingMissingIngredientsTest::RunTest(const FString& Parameters)
{
	using namespace DustlandCraftingTestHelpers;

	UCraftingStationComponent* Station = NewObject<UCraftingStationComponent>();
	Station->RecipeTable = MakeRecipeTable();

	UInventoryComponent* Inventory = NewObject<UInventoryComponent>();
	Inventory->ItemDefinitionTable = MakeItemTable();
	Inventory->MaxWeightKg = 100.f;
	Inventory->MaxVolume = 100.f;

	FString Reason;
	const bool bCanCraft = Station->CanCraft(TEXT("Recipe.Bandage"), Inventory, nullptr, Reason);

	TestFalse(TEXT("Cannot craft without fiber"), bCanCraft);
	TestEqual(TEXT("Reason is MissingIngredients"), Reason, FString(TEXT("MissingIngredients")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDustlandCraftingLockedRecipeTest,
	"Dustland.Crafting.CanCraftFailsWhenNotUnlocked",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FDustlandCraftingLockedRecipeTest::RunTest(const FString& Parameters)
{
	using namespace DustlandCraftingTestHelpers;

	UCraftingStationComponent* Station = NewObject<UCraftingStationComponent>();
	Station->RecipeTable = MakeRecipeTable();

	UInventoryComponent* Inventory = NewObject<UInventoryComponent>();
	Inventory->ItemDefinitionTable = MakeItemTable();
	Inventory->MaxWeightKg = 100.f;
	Inventory->MaxVolume = 100.f;
	Inventory->AddItem(TEXT("Item.Scrap"), 5);

	UCraftingKnowledgeComponent* Knowledge = NewObject<UCraftingKnowledgeComponent>();

	FString Reason;
	const bool bCanCraft = Station->CanCraft(TEXT("Recipe.Locked"), Inventory, Knowledge, Reason);

	TestFalse(TEXT("Cannot craft a locked recipe without unlocking it"), bCanCraft);
	TestEqual(TEXT("Reason is NotUnlocked"), Reason, FString(TEXT("NotUnlocked")));

	Knowledge->UnlockRecipe(TEXT("Recipe.Locked"));
	const bool bCanCraftAfterUnlock = Station->CanCraft(TEXT("Recipe.Locked"), Inventory, Knowledge, Reason);
	TestTrue(TEXT("Can craft after unlocking"), bCanCraftAfterUnlock);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDustlandCraftingFullCycleTest,
	"Dustland.Crafting.StartCraftConsumesAndProducesAfterTime",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FDustlandCraftingFullCycleTest::RunTest(const FString& Parameters)
{
	using namespace DustlandCraftingTestHelpers;

	UCraftingStationComponent* Station = NewObject<UCraftingStationComponent>();
	Station->RecipeTable = MakeRecipeTable();

	UInventoryComponent* Inventory = NewObject<UInventoryComponent>();
	Inventory->ItemDefinitionTable = MakeItemTable();
	Inventory->MaxWeightKg = 100.f;
	Inventory->MaxVolume = 100.f;
	Inventory->AddItem(TEXT("Item.Fiber"), 5);

	FString Reason;
	const bool bStarted = Station->StartCraft(TEXT("Recipe.Bandage"), Inventory, nullptr, Inventory, Reason);
	TestTrue(TEXT("Craft starts successfully"), bStarted);
	TestEqual(TEXT("2 fiber consumed immediately"), Inventory->GetItemCount(TEXT("Item.Fiber")), 3);
	TestEqual(TEXT("No bandage yet - still crafting"), Inventory->GetItemCount(TEXT("Item.Bandage")), 0);
	TestEqual(TEXT("One active job"), Station->ActiveJobs.Num(), 1);

	// Simulate enough time passing (recipe takes 1 second) for the job to finish.
	Station->TickComponent(1.5f, LEVELTICK_All, nullptr);

	TestEqual(TEXT("Job completed and was removed"), Station->ActiveJobs.Num(), 0);
	TestEqual(TEXT("Bandage was produced"), Inventory->GetItemCount(TEXT("Item.Bandage")), 1);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
