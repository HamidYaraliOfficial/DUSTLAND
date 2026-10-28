#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DustlandTypes.h"
#include "DustlandGameModeBase.generated.h"

class UWorldClockComponent;
class UWeatherSystemComponent;

/**
 * ADustlandGameModeBase
 *
 * Foundation GameMode. Two responsibilities:
 *
 *  1. Reads the chosen EDustlandDifficulty (set by the New Game
 *     Configuration screen) and pushes the corresponding scarcity
 *     multiplier onto every USurvivalComponent in the world.
 *
 *  2. Owns the single authoritative UWorldClockComponent and
 *     UWeatherSystemComponent for the session (this is the seed of the
 *     eventual Persistent World Simulation Engine) and, every tick,
 *     propagates their state into every other component that cares about
 *     it: ambient temperature into USurvivalComponent, the current hour
 *     into UOperatingHoursComponent, and equipped-clothing insulation back
 *     into USurvivalComponent. Each of those modules stays decoupled from
 *     Weather/Equipment at compile time; only this GameMode links them.
 *
 * Later phases replace the "GetAllActorsOfClass every tick" approach below
 * with a proper registry/subsystem once actor counts get large enough for
 * it to matter (see Docs/ARCHITECTURE.md, Simulation LOD).
 */
UCLASS()
class DUSTLAND_API ADustlandGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADustlandGameModeBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dustland|Difficulty")
	EDustlandDifficulty CurrentDifficulty = EDustlandDifficulty::Normal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dustland|World")
	TObjectPtr<UWorldClockComponent> WorldClock;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dustland|World")
	TObjectPtr<UWeatherSystemComponent> WeatherSystem;

	/** Returns the survival depletion multiplier for CurrentDifficulty (Story=slow drain, Hardcore=fast drain). */
	UFUNCTION(BlueprintPure, Category = "Dustland|Difficulty")
	float GetScarcityMultiplierForCurrentDifficulty() const;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	/** Accumulates real seconds so cross-system sync runs a few times a second instead of every single frame. */
	float TimeSinceLastWorldSync = 0.f;
	static constexpr float WorldSyncIntervalSeconds = 0.25f;

	void SyncWorldStateToComponents();
};

