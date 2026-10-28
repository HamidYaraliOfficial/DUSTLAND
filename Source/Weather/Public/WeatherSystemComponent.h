#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeatherTypes.h"
#include "WeatherSystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeatherChanged,
	EDustlandWeatherType, OldWeather, EDustlandWeatherType, NewWeather);

/**
 * UWeatherSystemComponent
 *
 * Picks a weighted-random weather type from a designer-authored table,
 * holds it for a randomized duration, then transitions again. Exposes the
 * current temperature modifier and visibility multiplier for other systems
 * (Survival ambient temperature, camera/rendering, AI perception) to read -
 * this component does not touch them directly, keeping it decoupled.
 */
UCLASS(ClassGroup = (Dustland), meta = (BlueprintSpawnableComponent))
class WEATHER_API UWeatherSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeatherSystemComponent();

	virtual void BeginPlay() override;

	/** Per-weather-type tuning. Populate every EDustlandWeatherType entry; missing entries default to Weight 0 (never selected). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Config")
	TMap<EDustlandWeatherType, FDustlandWeatherTypeConfig> WeatherConfigs;

	UPROPERTY(BlueprintReadOnly, Category = "Weather|State")
	EDustlandWeatherType CurrentWeather = EDustlandWeatherType::Clear;

	UPROPERTY(BlueprintReadOnly, Category = "Weather|State")
	float RemainingDurationHours = 4.f;

	/** Advances the weather clock. Call with the same DeltaHours the World Clock advanced by this frame. */
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void TickWeather(float DeltaHours);

	/** Forces a specific weather type immediately (scripted events, quest triggers, developer console). */
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void ForceWeather(EDustlandWeatherType NewWeather, float DurationHours);

	UFUNCTION(BlueprintPure, Category = "Weather")
	float GetTemperatureModifier() const;

	UFUNCTION(BlueprintPure, Category = "Weather")
	float GetVisibilityMultiplier() const;

	UFUNCTION(BlueprintPure, Category = "Weather")
	float GetSurvivalDrainMultiplier() const;

	UPROPERTY(BlueprintAssignable, Category = "Weather|Events")
	FOnWeatherChanged OnWeatherChanged;

private:
	EDustlandWeatherType PickWeightedRandomWeather() const;
	void TransitionTo(EDustlandWeatherType NewWeather);
	void EnsureDefaultConfigs();
};
