#include "DustlandGameModeBase.h"
#include "SurvivalComponent.h"
#include "OperatingHoursComponent.h"
#include "EquipmentComponent.h"
#include "WorldClockComponent.h"
#include "WeatherSystemComponent.h"
#include "Kismet/GameplayStatics.h"

ADustlandGameModeBase::ADustlandGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	WorldClock = CreateDefaultSubobject<UWorldClockComponent>(TEXT("WorldClock"));
	WeatherSystem = CreateDefaultSubobject<UWeatherSystemComponent>(TEXT("WeatherSystem"));
}

float ADustlandGameModeBase::GetScarcityMultiplierForCurrentDifficulty() const
{
	switch (CurrentDifficulty)
	{
	case EDustlandDifficulty::Story:        return 0.5f;
	case EDustlandDifficulty::Normal:       return 1.0f;
	case EDustlandDifficulty::SurvivalMode: return 1.5f;
	case EDustlandDifficulty::Hardcore:     return 2.0f;
	case EDustlandDifficulty::Custom:       // fallthrough - custom values are set externally per-field
	default:                                return 1.0f;
	}
}

void ADustlandGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	const float Multiplier = GetScarcityMultiplierForCurrentDifficulty();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (USurvivalComponent* Survival = Actor->FindComponentByClass<USurvivalComponent>())
		{
			Survival->DifficultyScarcityMultiplier = Multiplier;
		}
	}
}

void ADustlandGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (WeatherSystem && WorldClock)
	{
		// WeatherSystem's own duration countdown runs on the same in-game-hours
		// clock as everything else, driven here rather than ticking itself, so
		// there is exactly one place that converts real seconds to game hours.
		const float DeltaGameHours = DeltaSeconds * WorldClock->InGameHoursPerRealSecond;
		WeatherSystem->TickWeather(DeltaGameHours);
	}

	TimeSinceLastWorldSync += DeltaSeconds;
	if (TimeSinceLastWorldSync >= WorldSyncIntervalSeconds)
	{
		SyncWorldStateToComponents();
		TimeSinceLastWorldSync = 0.f;
	}
}

void ADustlandGameModeBase::SyncWorldStateToComponents()
{
	if (!WorldClock) return;

	const float BaseTemperature = WorldClock->GetBaseAmbientTemperature();
	const float WeatherModifier = WeatherSystem ? WeatherSystem->GetTemperatureModifier() : 0.f;
	const float EffectiveAmbientTemp = BaseTemperature + WeatherModifier;
	const float CurrentHour = WorldClock->CurrentHour;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (USurvivalComponent* Survival = Actor->FindComponentByClass<USurvivalComponent>())
		{
			Survival->SetAmbientTemperature(EffectiveAmbientTemp);

			if (UEquipmentComponent* Equipment = Actor->FindComponentByClass<UEquipmentComponent>())
			{
				Survival->EquipmentInsulationFactor = Equipment->GetTotalInsulation();
			}
		}

		if (UOperatingHoursComponent* Hours = Actor->FindComponentByClass<UOperatingHoursComponent>())
		{
			Hours->UpdateFromWorldClock(CurrentHour);
		}
	}
}
