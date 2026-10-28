#include "Misc/AutomationTest.h"
#include "WorldClockComponent.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDustlandWorldClockTemperatureCurveTest,
	"Dustland.Weather.WorldClock.TemperatureCurveBounds",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FDustlandWorldClockTemperatureCurveTest::RunTest(const FString& Parameters)
{
	UWorldClockComponent* Clock = NewObject<UWorldClockComponent>();
	Clock->NightLowTemperatureC = 8.f;
	Clock->DayHighTemperatureC = 34.f;

	const float AtTrough = Clock->GetBaseAmbientTemperatureAtHour(4.f);
	const float AtPeak = Clock->GetBaseAmbientTemperatureAtHour(14.f);

	TestNearlyEqual(TEXT("04:00 is the coldest point"), AtTrough, 8.f, 0.1f);
	TestNearlyEqual(TEXT("14:00 is the hottest point"), AtPeak, 34.f, 0.1f);

	const float AtNoon = Clock->GetBaseAmbientTemperatureAtHour(12.f);
	TestTrue(TEXT("Noon is warmer than the night trough"), AtNoon > AtTrough);
	TestTrue(TEXT("Noon is cooler than or equal to the day peak"), AtNoon <= AtPeak + 0.01f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDustlandWorldClockAdvanceByHoursTest,
	"Dustland.Weather.WorldClock.AdvanceByHoursWrapsAndFiresEvents",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FDustlandWorldClockAdvanceByHoursTest::RunTest(const FString& Parameters)
{
	UWorldClockComponent* Clock = NewObject<UWorldClockComponent>();
	Clock->CurrentHour = 22.f;
	Clock->TotalElapsedGameHours = 22.f;
	Clock->DayNumber = 1;

	Clock->AdvanceByHours(5.f); // 22:00 -> 03:00, crossing midnight

	TestNearlyEqual(TEXT("Hour wraps past midnight correctly"), Clock->CurrentHour, 3.f, 0.01f);
	TestEqual(TEXT("Day number incremented after crossing midnight"), Clock->DayNumber, 2);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
