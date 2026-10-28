#include "Misc/AutomationTest.h"
#include "OperatingHoursComponent.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDustlandOperatingHoursBasicWindowTest,
	"Dustland.Economy.OperatingHours.BasicWindow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FDustlandOperatingHoursBasicWindowTest::RunTest(const FString& Parameters)
{
	UOperatingHoursComponent* Component = NewObject<UOperatingHoursComponent>();
	Component->AddWindow(8.f, 18.f, FText::FromString(TEXT("Day Shift")));

	TestFalse(TEXT("Closed at 03:00"), Component->IsOpenAtHour(3.f));
	TestTrue(TEXT("Open at 09:00"), Component->IsOpenAtHour(9.f));
	TestFalse(TEXT("Closed at 19:00"), Component->IsOpenAtHour(19.f));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDustlandOperatingHoursOvernightWindowTest,
	"Dustland.Economy.OperatingHours.OvernightWindow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FDustlandOperatingHoursOvernightWindowTest::RunTest(const FString& Parameters)
{
	UOperatingHoursComponent* Component = NewObject<UOperatingHoursComponent>();
	// Overnight radio broadcast window: 22:00 to 06:00.
	Component->AddWindow(22.f, 6.f, FText::FromString(TEXT("Night Broadcast")));

	TestTrue(TEXT("Open at 23:00 (before midnight)"), Component->IsOpenAtHour(23.f));
	TestTrue(TEXT("Open at 02:00 (after midnight)"), Component->IsOpenAtHour(2.f));
	TestFalse(TEXT("Closed at 12:00 (midday)"), Component->IsOpenAtHour(12.f));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDustlandOperatingHoursTimeUntilNextChangeTest,
	"Dustland.Economy.OperatingHours.TimeUntilNextChange",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FDustlandOperatingHoursTimeUntilNextChangeTest::RunTest(const FString& Parameters)
{
	UOperatingHoursComponent* Component = NewObject<UOperatingHoursComponent>();
	Component->AddWindow(8.f, 18.f, FText::FromString(TEXT("Day Shift")));

	bool bWillOpen = false;
	int32 Hours = 0, Minutes = 0;

	// At 06:00, closed, should open in exactly 2 hours.
	const bool bHasChange = Component->GetTimeUntilNextChange(6.f, bWillOpen, Hours, Minutes);
	TestTrue(TEXT("A change is scheduled"), bHasChange);
	TestTrue(TEXT("Currently closed, waiting to open"), bWillOpen);
	TestEqual(TEXT("2 hours until open"), Hours, 2);
	TestEqual(TEXT("0 minutes remainder"), Minutes, 0);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
