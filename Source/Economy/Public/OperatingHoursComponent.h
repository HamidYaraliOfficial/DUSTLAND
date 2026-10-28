#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OperatingHoursComponent.generated.h"

/**
 * One open/close window on the 24-hour in-game clock. CloseHour may be
 * numerically smaller than OpenHour to represent a window that crosses
 * midnight (e.g. Open = 20.0, Close = 6.0 -> open 20:00 to 06:00).
 *
 * These values are meant to be authored by a designer in the editor OR
 * entered at runtime by the player/designer through a settings widget
 * (e.g. "Trading Post Hours" panel) via SetWindows()/AddWindow() below -
 * nothing here is hardcoded to a specific building or NPC.
 */
USTRUCT(BlueprintType)
struct FOperatingWindow
{
	GENERATED_BODY()

	/** 0.0 - 23.99, hour of day this window opens. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Operating Hours",
		meta = (ClampMin = "0.0", ClampMax = "23.99", UIMin = "0.0", UIMax = "23.99"))
	float OpenHour = 8.f;

	/** 0.0 - 23.99, hour of day this window closes. Smaller than OpenHour = wraps past midnight. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Operating Hours",
		meta = (ClampMin = "0.0", ClampMax = "23.99", UIMin = "0.0", UIMax = "23.99"))
	float CloseHour = 18.f;

	/** Optional label shown in UI, e.g. "Weekday Hours", "Market Day". */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Operating Hours")
	FText Label;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOperatingStateChanged, bool, bIsNowOpen);

/**
 * UOperatingHoursComponent
 *
 * Drop this on any actor that should behave differently depending on the
 * time of day: Trading Posts, Settlement gates, Radio Towers (broadcast
 * windows), Faction markets, guarded checkpoints, etc. The component itself
 * determines, from the windows it holds and the current world-clock hour it
 * is given, whether it is currently open and exactly how long remains until
 * the next open/close transition - callers never need to do that math.
 *
 * Windows can be authored in the editor (EditAnywhere) or supplied entirely
 * at runtime by the player/designer (SetWindows / AddWindow), so this same
 * component covers both "hardcoded lore hours" and "user-configured hours".
 */
UCLASS(ClassGroup = (Dustland), meta = (BlueprintSpawnableComponent))
class ECONOMY_API UOperatingHoursComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOperatingHoursComponent();

	/** All windows during which this actor is considered "open". Empty = always open. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Operating Hours")
	TArray<FOperatingWindow> Windows;

	/** Display name used in UI, e.g. "New Haven Trading Post". */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Operating Hours")
	FText DisplayName;

	// --- Runtime configuration (lets a settings/kiosk widget collect input from the player) ---

	UFUNCTION(BlueprintCallable, Category = "Operating Hours")
	void SetWindows(const TArray<FOperatingWindow>& NewWindows);

	UFUNCTION(BlueprintCallable, Category = "Operating Hours")
	void AddWindow(float OpenHour, float CloseHour, FText Label);

	UFUNCTION(BlueprintCallable, Category = "Operating Hours")
	void ClearWindows();

	// --- Queries -------------------------------------------------------------

	/** CurrentHour is 0.0-23.99 on the in-game 24h clock (supplied by the World/Time system). */
	UFUNCTION(BlueprintPure, Category = "Operating Hours")
	bool IsOpenAtHour(float CurrentHour) const;

	/** Localized-ready status text: "Open" or "Closed" (hook a string table for FA/EN/ZH). */
	UFUNCTION(BlueprintPure, Category = "Operating Hours")
	FText GetStatusText(float CurrentHour) const;

	/**
	 * Computes time remaining until the next state change.
	 * bOutWillOpen = true  -> currently closed, OutHours/OutMinutes = time until it opens.
	 * bOutWillOpen = false -> currently open,   OutHours/OutMinutes = time until it closes.
	 * Returns false only when Windows is empty (permanently open, nothing to count down to).
	 */
	UFUNCTION(BlueprintCallable, Category = "Operating Hours")
	bool GetTimeUntilNextChange(float CurrentHour, bool& bOutWillOpen, int32& OutHours, int32& OutMinutes) const;

	/** Convenience one-line summary for UI/tooltips, e.g. "Closed - opens in 3h 20m". */
	UFUNCTION(BlueprintPure, Category = "Operating Hours")
	FText GetStatusSummary(float CurrentHour) const;

	/** Call once per world-clock tick (from the Weather/Time system) so OnOperatingStateChanged fires correctly. */
	UFUNCTION(BlueprintCallable, Category = "Operating Hours")
	void UpdateFromWorldClock(float CurrentHour);

	UPROPERTY(BlueprintAssignable, Category = "Operating Hours|Events")
	FOnOperatingStateChanged OnOperatingStateChanged;

private:
	bool bLastKnownOpenState = true;
	bool bHasEvaluatedOnce = false;

	static float WrapHour(float Hour);
	static bool HourWithinWindow(float Hour, const FOperatingWindow& Window);
};
