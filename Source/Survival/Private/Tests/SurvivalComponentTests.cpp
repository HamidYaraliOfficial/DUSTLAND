#include "Misc/AutomationTest.h"
#include "SurvivalComponent.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDustlandSurvivalConsumeFoodTest,
	"Dustland.Survival.ConsumeFoodClampsAt100",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FDustlandSurvivalConsumeFoodTest::RunTest(const FString& Parameters)
{
	USurvivalComponent* Component = NewObject<USurvivalComponent>();
	Component->Hunger = 90.f;

	Component->ConsumeFood(25.f);
	TestEqual(TEXT("Hunger clamps at 100 after overfeeding"), Component->Hunger, 100.f);

	Component->Hunger = 40.f;
	Component->ConsumeFood(10.f);
	TestEqual(TEXT("Hunger adds normally below the clamp"), Component->Hunger, 50.f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDustlandSurvivalStatLevelTest,
	"Dustland.Survival.StatLevelThresholds",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FDustlandSurvivalStatLevelTest::RunTest(const FString& Parameters)
{
	USurvivalComponent* Component = NewObject<USurvivalComponent>();
	FSurvivalStatConfig Config;
	Config.StressedThreshold = 50.f;
	Config.CriticalThreshold = 20.f;
	Config.FailingThreshold = 5.f;

	TestEqual(TEXT("100 is Fine"), (int32)Component->GetStatLevel(100.f, Config), (int32)EDustlandSurvivalLevel::Fine);
	TestEqual(TEXT("49 is Stressed"), (int32)Component->GetStatLevel(49.f, Config), (int32)EDustlandSurvivalLevel::Stressed);
	TestEqual(TEXT("19 is Critical"), (int32)Component->GetStatLevel(19.f, Config), (int32)EDustlandSurvivalLevel::Critical);
	TestEqual(TEXT("4 is Failing"), (int32)Component->GetStatLevel(4.f, Config), (int32)EDustlandSurvivalLevel::Failing);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
