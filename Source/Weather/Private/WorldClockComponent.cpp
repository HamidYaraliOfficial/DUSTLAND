#include "WorldClockComponent.h"

UWorldClockComponent::UWorldClockComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.f; // every frame: hour-boundary detection needs fine granularity
}

float UWorldClockComponent::GetBaseAmbientTemperatureAtHour(float Hour) const
{
	// A single sine wave across the 24h day, trough at 04:00, peak at 14:00.
	// Using (Hour - 4) keeps the trough anchored at 04:00 without needing a
	// lookup curve asset for this foundational build; a UCurveFloat can
	// replace this later for non-symmetrical biome-specific curves.
	const float Radians = ((Hour - 4.f) / 24.f) * 2.f * PI;
	const float Normalized = (1.f - FMath::Cos(Radians)) * 0.5f; // 0 at trough, 1 at peak
	return FMath::Lerp(NightLowTemperatureC, DayHighTemperatureC, Normalized);
}

void UWorldClockComponent::CheckHourAndDayBoundaries(float PreviousTotalElapsed)
{
	const int32 IntHour = FMath::FloorToInt(CurrentHour);
	if (IntHour != LastBroadcastHour)
	{
		LastBroadcastHour = IntHour;
		OnHourChanged.Broadcast(IntHour);
	}

	const int32 PreviousDay = FMath::FloorToInt(PreviousTotalElapsed / 24.f) + 1;
	const int32 NewDay = FMath::FloorToInt(TotalElapsedGameHours / 24.f) + 1;
	if (NewDay != PreviousDay)
	{
		DayNumber = NewDay;
		OnNewDayStarted.Broadcast(DayNumber);
	}
}

void UWorldClockComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bClockRunning) return;

	const float PreviousTotalElapsed = TotalElapsedGameHours;
	const float DeltaHours = DeltaTime * InGameHoursPerRealSecond;

	TotalElapsedGameHours += DeltaHours;
	CurrentHour = FMath::Fmod(TotalElapsedGameHours, 24.f);

	CheckHourAndDayBoundaries(PreviousTotalElapsed);
}

void UWorldClockComponent::AdvanceByHours(float Hours)
{
	if (Hours <= 0.f) return;

	const float PreviousTotalElapsed = TotalElapsedGameHours;
	TotalElapsedGameHours += Hours;
	CurrentHour = FMath::Fmod(TotalElapsedGameHours, 24.f);

	CheckHourAndDayBoundaries(PreviousTotalElapsed);
}
