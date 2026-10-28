#include "WeatherSystemComponent.h"

UWeatherSystemComponent::UWeatherSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // driven externally via TickWeather(), see World Manager
}

void UWeatherSystemComponent::EnsureDefaultConfigs()
{
	if (WeatherConfigs.Num() > 0) return;

	auto AddDefault = [this](EDustlandWeatherType Type, float Weight, float MinDur, float MaxDur,
		float TempMod, float Visibility, float DrainMult)
	{
		FDustlandWeatherTypeConfig Config;
		Config.SelectionWeight = Weight;
		Config.MinDurationHours = MinDur;
		Config.MaxDurationHours = MaxDur;
		Config.TemperatureModifierC = TempMod;
		Config.VisibilityMultiplier = Visibility;
		Config.SurvivalDrainMultiplier = DrainMult;
		WeatherConfigs.Add(Type, Config);
	};

	// Reasonable out-of-the-box defaults for an arid post-apocalyptic biome;
	// designers override per-region via BeginPlay-time SetConfig calls or by
	// editing WeatherConfigs directly on a region-specific Weather actor.
	AddDefault(EDustlandWeatherType::Clear,        10.f, 4.f, 10.f,  0.f, 1.00f, 1.0f);
	AddDefault(EDustlandWeatherType::DustStorm,     3.f, 1.f, 4.f,   2.f, 0.35f, 1.3f);
	AddDefault(EDustlandWeatherType::Sandstorm,     2.f, 1.f, 3.f,   4.f, 0.15f, 1.6f);
	AddDefault(EDustlandWeatherType::Rain,          3.f, 1.f, 5.f,  -2.f, 0.70f, 1.1f);
	AddDefault(EDustlandWeatherType::HeavyRain,     1.5f,1.f, 3.f,  -4.f, 0.45f, 1.2f);
	AddDefault(EDustlandWeatherType::Fog,           2.f, 2.f, 6.f,  -1.f, 0.30f, 1.0f);
	AddDefault(EDustlandWeatherType::Heatwave,      2.f, 3.f, 8.f,   8.f, 0.90f, 1.7f);
	AddDefault(EDustlandWeatherType::ColdSnap,      2.f, 3.f, 8.f,  -8.f, 0.90f, 1.4f);
	AddDefault(EDustlandWeatherType::Thunderstorm,  1.f, 1.f, 2.f,  -3.f, 0.40f, 1.2f);
}

void UWeatherSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	EnsureDefaultConfigs();

	CurrentWeather = EDustlandWeatherType::Clear;
	if (const FDustlandWeatherTypeConfig* Config = WeatherConfigs.Find(CurrentWeather))
	{
		RemainingDurationHours = FMath::FRandRange(Config->MinDurationHours, Config->MaxDurationHours);
	}
}

EDustlandWeatherType UWeatherSystemComponent::PickWeightedRandomWeather() const
{
	float TotalWeight = 0.f;
	for (const auto& Pair : WeatherConfigs)
	{
		TotalWeight += FMath::Max(0.f, Pair.Value.SelectionWeight);
	}

	if (TotalWeight <= 0.f)
	{
		return EDustlandWeatherType::Clear;
	}

	float Roll = FMath::FRandRange(0.f, TotalWeight);
	for (const auto& Pair : WeatherConfigs)
	{
		const float Weight = FMath::Max(0.f, Pair.Value.SelectionWeight);
		if (Roll <= Weight)
		{
			return Pair.Key;
		}
		Roll -= Weight;
	}

	return EDustlandWeatherType::Clear;
}

void UWeatherSystemComponent::TransitionTo(EDustlandWeatherType NewWeather)
{
	const EDustlandWeatherType OldWeather = CurrentWeather;
	CurrentWeather = NewWeather;

	if (const FDustlandWeatherTypeConfig* Config = WeatherConfigs.Find(NewWeather))
	{
		RemainingDurationHours = FMath::FRandRange(Config->MinDurationHours, Config->MaxDurationHours);
	}
	else
	{
		RemainingDurationHours = 4.f;
	}

	if (OldWeather != NewWeather)
	{
		OnWeatherChanged.Broadcast(OldWeather, NewWeather);
	}
}

void UWeatherSystemComponent::TickWeather(float DeltaHours)
{
	EnsureDefaultConfigs();

	RemainingDurationHours -= DeltaHours;
	if (RemainingDurationHours <= 0.f)
	{
		TransitionTo(PickWeightedRandomWeather());
	}
}

void UWeatherSystemComponent::ForceWeather(EDustlandWeatherType NewWeather, float DurationHours)
{
	EnsureDefaultConfigs();
	TransitionTo(NewWeather);
	RemainingDurationHours = FMath::Max(0.1f, DurationHours);
}

float UWeatherSystemComponent::GetTemperatureModifier() const
{
	if (const FDustlandWeatherTypeConfig* Config = WeatherConfigs.Find(CurrentWeather))
	{
		return Config->TemperatureModifierC;
	}
	return 0.f;
}

float UWeatherSystemComponent::GetVisibilityMultiplier() const
{
	if (const FDustlandWeatherTypeConfig* Config = WeatherConfigs.Find(CurrentWeather))
	{
		return Config->VisibilityMultiplier;
	}
	return 1.f;
}

float UWeatherSystemComponent::GetSurvivalDrainMultiplier() const
{
	if (const FDustlandWeatherTypeConfig* Config = WeatherConfigs.Find(CurrentWeather))
	{
		return Config->SurvivalDrainMultiplier;
	}
	return 1.f;
}
