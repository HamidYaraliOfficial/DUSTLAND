#include "OperatingHoursComponent.h"
#include "Internationalization/Text.h"

UOperatingHoursComponent::UOperatingHoursComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

float UOperatingHoursComponent::WrapHour(float Hour)
{
	float Wrapped = FMath::Fmod(Hour, 24.f);
	if (Wrapped < 0.f) Wrapped += 24.f;
	return Wrapped;
}

bool UOperatingHoursComponent::HourWithinWindow(float Hour, const FOperatingWindow& Window)
{
	const float H = WrapHour(Hour);
	const float Open = WrapHour(Window.OpenHour);
	const float Close = WrapHour(Window.CloseHour);

	if (FMath::IsNearlyEqual(Open, Close))
	{
		return true; // Open == Close is treated as "open 24 hours" for this window
	}

	if (Open < Close)
	{
		return H >= Open && H < Close;
	}
	// Wraps past midnight, e.g. Open=20, Close=6
	return H >= Open || H < Close;
}

void UOperatingHoursComponent::SetWindows(const TArray<FOperatingWindow>& NewWindows)
{
	Windows = NewWindows;
}

void UOperatingHoursComponent::AddWindow(float OpenHour, float CloseHour, FText Label)
{
	FOperatingWindow NewWindow;
	NewWindow.OpenHour = WrapHour(OpenHour);
	NewWindow.CloseHour = WrapHour(CloseHour);
	NewWindow.Label = Label;
	Windows.Add(NewWindow);
}

void UOperatingHoursComponent::ClearWindows()
{
	Windows.Empty();
}

bool UOperatingHoursComponent::IsOpenAtHour(float CurrentHour) const
{
	if (Windows.Num() == 0)
	{
		return true; // no windows configured = always open
	}

	for (const FOperatingWindow& Window : Windows)
	{
		if (HourWithinWindow(CurrentHour, Window))
		{
			return true;
		}
	}
	return false;
}

FText UOperatingHoursComponent::GetStatusText(float CurrentHour) const
{
	return IsOpenAtHour(CurrentHour)
		? NSLOCTEXT("Dustland.Economy", "StatusOpen", "Open")
		: NSLOCTEXT("Dustland.Economy", "StatusClosed", "Closed");
}

bool UOperatingHoursComponent::GetTimeUntilNextChange(float CurrentHour, bool& bOutWillOpen, int32& OutHours, int32& OutMinutes) const
{
	OutHours = 0;
	OutMinutes = 0;
	bOutWillOpen = false;

	if (Windows.Num() == 0)
	{
		return false; // permanently open; there is no "next change" to report
	}

	const bool bCurrentlyOpen = IsOpenAtHour(CurrentHour);
	bOutWillOpen = !bCurrentlyOpen;

	// Scan forward in 1-minute increments (cheap: at most 1440 iterations) to
	// find the first minute at which the open/closed state differs from now.
	// This stays correct regardless of how many windows exist or whether they
	// overlap, without needing to reason about boundary math analytically.
	constexpr float MinuteInHours = 1.f / 60.f;
	for (int32 StepMinutes = 1; StepMinutes <= 24 * 60; ++StepMinutes)
	{
		const float ProbeHour = CurrentHour + StepMinutes * MinuteInHours;
		if (IsOpenAtHour(ProbeHour) != bCurrentlyOpen)
		{
			OutHours = (StepMinutes - 1) / 60;
			OutMinutes = (StepMinutes - 1) % 60;
			return true;
		}
	}

	// Should not normally happen (would mean an unbroken 24h open window that
	// still reported "closed" now, which is contradictory), but fail safely.
	return false;
}

FText UOperatingHoursComponent::GetStatusSummary(float CurrentHour) const
{
	bool bWillOpen = false;
	int32 Hours = 0, Minutes = 0;
	const FText StatusText = GetStatusText(CurrentHour);

	if (!GetTimeUntilNextChange(CurrentHour, bWillOpen, Hours, Minutes))
	{
		return StatusText; // always open, nothing to count down to
	}

	const FText TimePart = (Hours > 0)
		? FText::Format(NSLOCTEXT("Dustland.Economy", "TimeHM", "{0}h {1}m"), Hours, Minutes)
		: FText::Format(NSLOCTEXT("Dustland.Economy", "TimeM", "{0}m"), Minutes);

	return bWillOpen
		? FText::Format(NSLOCTEXT("Dustland.Economy", "OpensIn", "{0} - opens in {1}"), StatusText, TimePart)
		: FText::Format(NSLOCTEXT("Dustland.Economy", "ClosesIn", "{0} - closes in {1}"), StatusText, TimePart);
}

void UOperatingHoursComponent::UpdateFromWorldClock(float CurrentHour)
{
	const bool bNowOpen = IsOpenAtHour(CurrentHour);
	if (!bHasEvaluatedOnce || bNowOpen != bLastKnownOpenState)
	{
		bHasEvaluatedOnce = true;
		bLastKnownOpenState = bNowOpen;
		OnOperatingStateChanged.Broadcast(bNowOpen);
	}
}
