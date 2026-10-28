#include "SurvivalComponent.h"
#include "GameFramework/Actor.h"

USurvivalComponent::USurvivalComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.25f; // 4 Hz is plenty for meters that drain over in-game hours
	SetIsReplicatedByDefault(true);
}

void USurvivalComponent::BeginPlay()
{
	Super::BeginPlay();

	LastHungerLevel = GetStatLevel(Hunger, HungerConfig);
	LastThirstLevel = GetStatLevel(Thirst, ThirstConfig);
	LastFatigueLevel = GetStatLevel(Fatigue, FatigueConfig);
	LastEnergyLevel = GetStatLevel(Energy, EnergyConfig);
}

EDustlandSurvivalLevel USurvivalComponent::GetStatLevel(float StatValue, const FSurvivalStatConfig& Config) const
{
	if (StatValue <= Config.FailingThreshold)  return EDustlandSurvivalLevel::Failing;
	if (StatValue <= Config.CriticalThreshold) return EDustlandSurvivalLevel::Critical;
	if (StatValue <= Config.StressedThreshold) return EDustlandSurvivalLevel::Stressed;
	return EDustlandSurvivalLevel::Fine;
}

void USurvivalComponent::TickStat(float& Stat, const FSurvivalStatConfig& Config, float DeltaHours, FName StatName,
	EDustlandSurvivalLevel& InOutLastLevel, bool& InOutCollapseFired)
{
	const float Depletion = Config.DepletionPerHour * DifficultyScarcityMultiplier * DeltaHours;
	Stat = FMath::Clamp(Stat - Depletion, 0.f, 100.f);

	const EDustlandSurvivalLevel NewLevel = GetStatLevel(Stat, Config);
	if (NewLevel != InOutLastLevel)
	{
		InOutLastLevel = NewLevel;
		OnSurvivalLevelChanged.Broadcast(StatName, NewLevel);
	}

	if (NewLevel == EDustlandSurvivalLevel::Failing && !InOutCollapseFired)
	{
		InOutCollapseFired = true;
		OnSurvivalCollapse.Broadcast(StatName);
	}
	else if (NewLevel != EDustlandSurvivalLevel::Failing)
	{
		InOutCollapseFired = false;
	}
}

void USurvivalComponent::TickBodyTemperature(float DeltaHours)
{
	// Effective ambient temperature after clothing/equipment insulation pulls it
	// back toward the comfortable band.
	const float ComfortMid = (ComfortableTempMinC + ComfortableTempMaxC) * 0.5f;
	const float InsulatedAmbient = FMath::Lerp(AmbientTemperatureC, ComfortMid, EquipmentInsulationFactor);

	float TargetBodyTemp = 37.f;
	if (InsulatedAmbient < ComfortableTempMinC)
	{
		TargetBodyTemp = 37.f - (ComfortableTempMinC - InsulatedAmbient) * 0.15f;
	}
	else if (InsulatedAmbient > ComfortableTempMaxC)
	{
		TargetBodyTemp = 37.f + (InsulatedAmbient - ComfortableTempMaxC) * 0.15f;
	}

	// Drift body temperature toward target; DeltaHours keeps this frame-rate independent.
	const float DriftSpeedPerHour = 4.f;
	BodyTemperature = FMath::FInterpConstantTo(BodyTemperature, TargetBodyTemp, DeltaHours, DriftSpeedPerHour);
}

void USurvivalComponent::ApplyHealthDrainIfFailing(float DeltaSeconds)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	float TotalDrainPerSecond = 0.f;
	if (GetStatLevel(Hunger, HungerConfig) == EDustlandSurvivalLevel::Failing)
	{
		TotalDrainPerSecond += HungerConfig.FailingHealthDrainPerSecond;
	}
	if (GetStatLevel(Thirst, ThirstConfig) == EDustlandSurvivalLevel::Failing)
	{
		TotalDrainPerSecond += ThirstConfig.FailingHealthDrainPerSecond;
	}
	if (BodyTemperature <= 30.f || BodyTemperature >= 42.f)
	{
		TotalDrainPerSecond += 0.75f; // extreme hypothermia / hyperthermia
	}

	if (TotalDrainPerSecond > 0.f)
	{
		// Intentionally routed through a generic damage event rather than a
		// hardcoded health variable so any custom AttributeSet / GAS setup can
		// hook in without this module depending on a specific health component.
		Owner->TakeDamage(TotalDrainPerSecond * DeltaSeconds,
			FDamageEvent(), nullptr, Owner);
	}
}

void USurvivalComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// NOTE: DeltaTime here is real seconds between ticks (TickInterval = 0.25s).
	// A full World Time Dilation hook (in-game-hours-per-real-second) belongs to
	// the World/Weather module; for this foundational build we assume a fixed
	// 1 real-minute = 1 in-game-hour ratio, exposed here as a single constant
	// so the World module can later replace it with the authoritative clock.
	static constexpr float InGameHoursPerRealSecond = 1.f / 60.f;
	const float DeltaHours = DeltaTime * InGameHoursPerRealSecond;

	TickStat(Hunger, HungerConfig, DeltaHours, TEXT("Hunger"), LastHungerLevel, bHungerCollapseFired);
	TickStat(Thirst, ThirstConfig, DeltaHours, TEXT("Thirst"), LastThirstLevel, bThirstCollapseFired);
	TickStat(Fatigue, FatigueConfig, DeltaHours, TEXT("Fatigue"), LastFatigueLevel, bFatigueCollapseFired);
	TickStat(Energy, EnergyConfig, DeltaHours, TEXT("Energy"), LastEnergyLevel, bEnergyCollapseFired);
	TickBodyTemperature(DeltaHours);
	ApplyHealthDrainIfFailing(DeltaTime);

	OnSurvivalStatsChanged.Broadcast(Hunger, Thirst, Fatigue, BodyTemperature);
}

void USurvivalComponent::ConsumeFood(float Value)
{
	Hunger = FMath::Clamp(Hunger + Value, 0.f, 100.f);
}

void USurvivalComponent::ConsumeWater(float Value)
{
	Thirst = FMath::Clamp(Thirst + Value, 0.f, 100.f);
}

void USurvivalComponent::Rest(float DurationHours, float FatigueRecoveryPerHour, float EnergyRecoveryPerHour)
{
	Fatigue = FMath::Clamp(Fatigue + FatigueRecoveryPerHour * DurationHours, 0.f, 100.f);
	Energy = FMath::Clamp(Energy + EnergyRecoveryPerHour * DurationHours, 0.f, 100.f);

	// Resting still burns some hunger/thirst reserves, proportionally reduced
	// vs. active hours, so sleeping is not a "free" way to skip survival needs.
	Hunger = FMath::Clamp(Hunger - HungerConfig.DepletionPerHour * 0.4f * DurationHours, 0.f, 100.f);
	Thirst = FMath::Clamp(Thirst - ThirstConfig.DepletionPerHour * 0.4f * DurationHours, 0.f, 100.f);
}

void USurvivalComponent::SetAmbientTemperature(float NewAmbientC)
{
	AmbientTemperatureC = NewAmbientC;
}

float USurvivalComponent::GetOverallDistress() const
{
	const float HungerScore = 1.f - (Hunger / 100.f);
	const float ThirstScore = 1.f - (Thirst / 100.f);
	const float FatigueScore = 1.f - (Fatigue / 100.f);
	const float TempScore = FMath::Clamp(FMath::Abs(BodyTemperature - 37.f) / 8.f, 0.f, 1.f);

	return FMath::Clamp((HungerScore + ThirstScore + FatigueScore + TempScore) / 4.f, 0.f, 1.f);
}
