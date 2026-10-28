#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DustlandTypes.h"
#include "SurvivalComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnSurvivalStatsChanged,
	float, Hunger, float, Thirst, float, Fatigue, float, BodyTemperature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSurvivalLevelChanged,
	FName, StatName, EDustlandSurvivalLevel, NewLevel);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSurvivalCollapse, FName, CauseStat);

/**
 * Per-stat configuration. Exposed on the component so designers can tune
 * depletion/regen rates per difficulty preset without touching C++.
 */
USTRUCT(BlueprintType)
struct FSurvivalStatConfig
{
	GENERATED_BODY()

	/** Units lost per in-game hour under neutral conditions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
	float DepletionPerHour = 2.f;

	/** Value (0-100) at which the stat is considered "Stressed". */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
	float StressedThreshold = 50.f;

	/** Value (0-100) at which the stat is considered "Critical". */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
	float CriticalThreshold = 20.f;

	/** Value (0-100) at which the stat is considered "Failing" (health drain begins). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
	float FailingThreshold = 5.f;

	/** Health lost per second while this stat is at/under FailingThreshold. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
	float FailingHealthDrainPerSecond = 0.5f;
};

/**
 * UDustlandSurvivalComponent
 *
 * Owns Hunger, Thirst, Fatigue, BodyTemperature and Energy for the actor it
 * is attached to (normally the player pawn, but companions/NPCs marked as
 * "SurvivalTracked" can use the same component so hardcore playthroughs can
 * make companions need food and water too).
 *
 * Design intent: these systems create real gameplay decisions (carry water,
 * plan rest stops, dress for climate) without becoming a medical simulator.
 * There is no disease modelling, no real medication dosing and no realistic
 * physiological detail - only abstracted, game-balanced meters.
 */
UCLASS(ClassGroup = (Dustland), meta = (BlueprintSpawnableComponent))
class SURVIVAL_API USurvivalComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USurvivalComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	// --- Configuration -----------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Config")
	FSurvivalStatConfig HungerConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Config")
	FSurvivalStatConfig ThirstConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Config")
	FSurvivalStatConfig FatigueConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Config")
	FSurvivalStatConfig EnergyConfig;

	/** Comfortable body-temperature band; outside of it the stat drifts toward hot/cold extremes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Config")
	float ComfortableTempMinC = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Config")
	float ComfortableTempMaxC = 28.f;

	/** How strongly worn Equipment (clothing) insulates against ambient temperature, 0=no protection, 1=full protection. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Config", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float EquipmentInsulationFactor = 0.f;

	/** Global multiplier applied to every depletion rate, driven by the active EDustlandDifficulty preset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Config")
	float DifficultyScarcityMultiplier = 1.f;

	// --- Current state (0-100, 100 = fully satisfied) -----------------------

	UPROPERTY(BlueprintReadOnly, Category = "Survival|State")
	float Hunger = 100.f;

	UPROPERTY(BlueprintReadOnly, Category = "Survival|State")
	float Thirst = 100.f;

	UPROPERTY(BlueprintReadOnly, Category = "Survival|State")
	float Fatigue = 100.f;

	UPROPERTY(BlueprintReadOnly, Category = "Survival|State")
	float Energy = 100.f;

	/** Degrees Celsius, abstracted core body temperature. Neutral = 37. */
	UPROPERTY(BlueprintReadOnly, Category = "Survival|State")
	float BodyTemperature = 37.f;

	/** Ambient temperature at the actor's current location, normally pushed in by the Weather system. */
	UPROPERTY(BlueprintReadOnly, Category = "Survival|State")
	float AmbientTemperatureC = 20.f;

	// --- Public API ----------------------------------------------------------

	/** Consume a food item; Value typically comes from the item's data (0-100 scale). */
	UFUNCTION(BlueprintCallable, Category = "Survival")
	void ConsumeFood(float Value);

	/** Consume a water/drink item. */
	UFUNCTION(BlueprintCallable, Category = "Survival")
	void ConsumeWater(float Value);

	/** Rest for DurationHours of in-game time (sleeping bag, bed, settlement housing, etc.). */
	UFUNCTION(BlueprintCallable, Category = "Survival")
	void Rest(float DurationHours, float FatigueRecoveryPerHour = 12.f, float EnergyRecoveryPerHour = 8.f);

	/** Called by the Weather/World system whenever the actor's local ambient temperature changes. */
	UFUNCTION(BlueprintCallable, Category = "Survival")
	void SetAmbientTemperature(float NewAmbientC);

	/** Aggregate 0-1 "how bad is it right now" score, useful for UI vignettes and Music Director hooks. */
	UFUNCTION(BlueprintPure, Category = "Survival")
	float GetOverallDistress() const;

	UFUNCTION(BlueprintPure, Category = "Survival")
	EDustlandSurvivalLevel GetStatLevel(float StatValue, const FSurvivalStatConfig& Config) const;

	// --- Events ----------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Survival|Events")
	FOnSurvivalStatsChanged OnSurvivalStatsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Survival|Events")
	FOnSurvivalLevelChanged OnSurvivalLevelChanged;

	/** Fired once per stat the moment it first drops to/under FailingThreshold. */
	UPROPERTY(BlueprintAssignable, Category = "Survival|Events")
	FOnSurvivalCollapse OnSurvivalCollapse;

protected:
	virtual void BeginPlay() override;

private:
	void TickStat(float& Stat, const FSurvivalStatConfig& Config, float DeltaHours, FName StatName,
		EDustlandSurvivalLevel& InOutLastLevel, bool& InOutCollapseFired);
	void TickBodyTemperature(float DeltaHours);
	void ApplyHealthDrainIfFailing(float DeltaSeconds);

	EDustlandSurvivalLevel LastHungerLevel = EDustlandSurvivalLevel::Fine;
	EDustlandSurvivalLevel LastThirstLevel = EDustlandSurvivalLevel::Fine;
	EDustlandSurvivalLevel LastFatigueLevel = EDustlandSurvivalLevel::Fine;
	EDustlandSurvivalLevel LastEnergyLevel = EDustlandSurvivalLevel::Fine;

	bool bHungerCollapseFired = false;
	bool bThirstCollapseFired = false;
	bool bFatigueCollapseFired = false;
	bool bEnergyCollapseFired = false;
};
