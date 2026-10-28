#pragma once

#include "CoreMinimal.h"
#include "WeatherTypes.generated.h"

UENUM(BlueprintType)
enum class EDustlandWeatherType : uint8
{
	Clear        UMETA(DisplayName = "Clear"),
	DustStorm    UMETA(DisplayName = "Dust Storm"),
	Sandstorm    UMETA(DisplayName = "Sandstorm"),
	Rain         UMETA(DisplayName = "Rain"),
	HeavyRain    UMETA(DisplayName = "Heavy Rain"),
	Fog          UMETA(DisplayName = "Fog"),
	Heatwave     UMETA(DisplayName = "Heatwave"),
	ColdSnap     UMETA(DisplayName = "Cold Snap"),
	Thunderstorm UMETA(DisplayName = "Thunderstorm")
};

/**
 * Designer-tunable parameters for one weather type. A full TMap of these
 * (keyed by EDustlandWeatherType) lives on UWeatherSystemComponent so every
 * value below can be balanced without touching C++.
 */
USTRUCT(BlueprintType)
struct FDustlandWeatherTypeConfig
{
	GENERATED_BODY()

	/** Relative chance of being picked when the current weather ends (0 = never occurs). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0"))
	float SelectionWeight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.1"))
	float MinDurationHours = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.1"))
	float MaxDurationHours = 6.f;

	/** Added on top of the base day/night temperature curve while this weather is active. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	float TemperatureModifierC = 0.f;

	/** 0 = pitch black / can't see your hand, 1 = perfectly clear. Feeds AI perception and camera FX. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float VisibilityMultiplier = 1.f;

	/** Multiplies Survival stat depletion while this weather is active (e.g. Heatwave increases Thirst drain). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0"))
	float SurvivalDrainMultiplier = 1.f;
};
