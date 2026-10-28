#include "Misc/AutomationTest.h"
#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "Engine/DataTable.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace DustlandEquipmentTestHelpers
{
	static UDataTable* MakeItemTable()
	{
		UDataTable* Table = NewObject<UDataTable>();
		Table->RowStruct = FDustlandItemDefinition::StaticStruct();

		FDustlandItemDefinition JacketDef;
		JacketDef.ItemID = TEXT("Item.DustJacket");
		JacketDef.ItemType = EDustlandItemType::Clothing;
		JacketDef.Weight = 1.2f;
		JacketDef.Volume = 3.f;
		JacketDef.MaxStackSize = 1;
		JacketDef.bHasCondition = true;
		Table->AddRow(TEXT("Item.DustJacket"), JacketDef);

		return Table;
	}

	static UDataTable* MakeEquipmentTable()
	{
		UDataTable* Table = NewObject<UDataTable>();
		Table->RowStruct = FDustlandEquipmentDefinition::StaticStruct();

		FDustlandEquipmentDefinition JacketEquip;
		JacketEquip.ItemID = TEXT("Item.DustJacket");
		JacketEquip.Slot = EDustlandEquipmentSlot::Torso;
		JacketEquip.InsulationValue = 0.4f;
		JacketEquip.ArmorValue = 5.f;
		JacketEquip.ConditionLossPerHour = 1.f;
		Table->AddRow(TEXT("Item.DustJacket"), JacketEquip);

		return Table;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDustlandEquipItemMovesFromInventoryTest,
	"Dustland.Equipment.EquipItemMovesFromInventory",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FDustlandEquipItemMovesFromInventoryTest::RunTest(const FString& Parameters)
{
	using namespace DustlandEquipmentTestHelpers;

	UInventoryComponent* Backpack = NewObject<UInventoryComponent>();
	Backpack->ItemDefinitionTable = MakeItemTable();
	Backpack->MaxWeightKg = 50.f;
	Backpack->MaxVolume = 50.f;

	UEquipmentComponent* Equipment = NewObject<UEquipmentComponent>();
	Equipment->EquipmentDefinitionTable = MakeEquipmentTable();

	Backpack->AddItem(TEXT("Item.DustJacket"), 1, 80.f);
	TestEqual(TEXT("Backpack has the jacket before equipping"), Backpack->GetItemCount(TEXT("Item.DustJacket")), 1);

	const FGuid JacketGuid = Backpack->Items[0].InstanceGuid;
	const bool bEquipped = Equipment->EquipItem(Backpack, JacketGuid);

	TestTrue(TEXT("Equip succeeds"), bEquipped);
	TestEqual(TEXT("Backpack no longer has the jacket"), Backpack->GetItemCount(TEXT("Item.DustJacket")), 0);
	TestTrue(TEXT("Torso slot is occupied"), Equipment->IsSlotOccupied(EDustlandEquipmentSlot::Torso));
	TestNearlyEqual(TEXT("Total insulation reflects the jacket"), Equipment->GetTotalInsulation(), 0.4f, 0.01f);
	TestNearlyEqual(TEXT("Total armor reflects the jacket"), Equipment->GetTotalArmorValue(), 5.f, 0.01f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDustlandUnequipReturnsToInventoryTest,
	"Dustland.Equipment.UnequipReturnsToInventory",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FDustlandUnequipReturnsToInventoryTest::RunTest(const FString& Parameters)
{
	using namespace DustlandEquipmentTestHelpers;

	UInventoryComponent* Backpack = NewObject<UInventoryComponent>();
	Backpack->ItemDefinitionTable = MakeItemTable();
	Backpack->MaxWeightKg = 50.f;
	Backpack->MaxVolume = 50.f;

	UEquipmentComponent* Equipment = NewObject<UEquipmentComponent>();
	Equipment->EquipmentDefinitionTable = MakeEquipmentTable();

	Backpack->AddItem(TEXT("Item.DustJacket"), 1, 65.f);
	const FGuid JacketGuid = Backpack->Items[0].InstanceGuid;
	Equipment->EquipItem(Backpack, JacketGuid);

	const bool bUnequipped = Equipment->UnequipItem(EDustlandEquipmentSlot::Torso, Backpack);

	TestTrue(TEXT("Unequip succeeds"), bUnequipped);
	TestFalse(TEXT("Torso slot is empty again"), Equipment->IsSlotOccupied(EDustlandEquipmentSlot::Torso));
	TestEqual(TEXT("Backpack has the jacket again"), Backpack->GetItemCount(TEXT("Item.DustJacket")), 1);
	TestNearlyEqual(TEXT("Condition was preserved through equip/unequip"), Backpack->Items[0].Condition, 65.f, 0.01f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDustlandEquipmentDegradesOverTimeTest,
	"Dustland.Equipment.DegradesOverTime",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FDustlandEquipmentDegradesOverTimeTest::RunTest(const FString& Parameters)
{
	using namespace DustlandEquipmentTestHelpers;

	UInventoryComponent* Backpack = NewObject<UInventoryComponent>();
	Backpack->ItemDefinitionTable = MakeItemTable();
	Backpack->MaxWeightKg = 50.f;
	Backpack->MaxVolume = 50.f;

	UEquipmentComponent* Equipment = NewObject<UEquipmentComponent>();
	Equipment->EquipmentDefinitionTable = MakeEquipmentTable();

	Backpack->AddItem(TEXT("Item.DustJacket"), 1, 100.f);
	const FGuid JacketGuid = Backpack->Items[0].InstanceGuid;
	Equipment->EquipItem(Backpack, JacketGuid);

	Equipment->DegradeEquippedItems(10.f); // 10 in-game hours at 1.0 loss/hour

	const FDustlandItemInstance* Instance = Equipment->EquippedItems.Find(EDustlandEquipmentSlot::Torso);
	TestNotNull(TEXT("Jacket is still equipped"), Instance);
	if (Instance)
	{
		TestNearlyEqual(TEXT("Condition dropped by 10 over 10 hours"), Instance->Condition, 90.f, 0.01f);
	}

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
