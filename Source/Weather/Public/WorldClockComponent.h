#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorldClockComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHourChanged, int32, NewHour);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewDayStarted, int32, NewDayNumber);

/**
 * UWorldClockComponent
 *
 * The single authoritative source of "what time is it" for the whole game.
 * Every other time-of-day-aware system (Survival's ambient temperature,
 * Economy's OperatingHoursComponent, the eventual Music Director / NPC
 * schedules) reads CurrentHour from here rather than keeping its own clock,
 * so they can never drift out of sync with each other.
 *
 * Intentionally a plain ActorComponent (not a UGameInstanceSubsystem) for
 * this foundational build so a single Dustland World Manager actor can own
 * it alongside UWeatherSystemComponent and coordinate both each tick; see
 * ADustlandGameModeBase.
 */
UCLASS(ClassGroup = (Dustland), meta = (BlueprintSpawnableComponent))
class WEATHER_API UWorldClockComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWorldClockComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	/** How many in-game hours pass per real-world second. 1/60 = one real minute per in-game hour. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Clock|Config")
	float InGameHoursPerRealSecond = 1.f / 60.f;

	/** Set false to freeze time (menus, cutscenes, Photo Mode, Sandbox pause). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Clock|Config")
	bool bClockRunning = true;

	/** 0.0 - 23.999..., current hour of the day. */
	UPROPERTY(BlueprintReadOnly, Category = "World Clock|State")
	float CurrentHour = 8.f;

	UPROPERTY(BlueprintReadOnly, Category = "World Clock|State")
	int32 DayNumber = 1;

	UPROPERTY(BlueprintReadOnly, Category = "World Clock|State")
	float TotalElapsedGameHours = 8.f;

	// --- Day/night ambient temperature curve (before weather modifiers) -----

	/** Ambient temperature at the coldest point of the night (around 04:00). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Clock|Climate")
	float NightLowTemperatureC = 8.f;

	/** Ambient temperature at the hottest point of the day (around 14:00). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Clock|Climate")
	float DayHighTemperatureC = 34.f;

	/** Smooth day/night ambient temperature (a sine-based curve; no weather modifier applied). */
	UFUNCTION(BlueprintPure, Category = "World Clock")
	float GetBaseAmbientTemperature() const { return GetBaseAmbientTemperatureAtHour(CurrentHour); }

	UFUNCTION(BlueprintPure, Category = "World Clock")
	float GetBaseAmbientTemperatureAtHour(float Hour) const;

	UFUNCTION(BlueprintPure, Category = "World Clock")
	bool IsNightTime() const { return CurrentHour >= 20.f || CurrentHour < 6.f; }

	UFUNCTION(BlueprintCallable, Category = "World Clock")
	void SetTimeScale(float NewInGameHoursPerRealSecond) { InGameHoursPerRealSecond = FMath::Max(0.f, NewInGameHoursPerRealSecond); }

	/** Jump directly to an hour (used by Rest/Sleep and Fast Travel time costs). */
	UFUNCTION(BlueprintCallable, Category = "World Clock")
	void AdvanceByHours(float Hours);

	UPROPERTY(BlueprintAssignable, Category = "World Clock|Events")
	FOnHourChanged OnHourChanged;

	UPROPERTY(BlueprintAssignable, Category = "World Clock|Events")
	FOnNewDayStarted OnNewDayStarted;

private:
	int32 LastBroadcastHour = -1;
	void CheckHourAndDayBoundaries(float PreviousTotalElapsed);
};
