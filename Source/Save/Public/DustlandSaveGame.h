#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DustlandItemTypes.h"
#include "DustlandTypes.h"
#include "DustlandSaveGame.generated.h"

/**
 * Bump this whenever the save schema changes. UDustlandSaveGame::MigrateIfNeeded
 * uses it to upgrade older saves in place instead of breaking them.
 */
#define DUSTLAND_SAVE_SCHEMA_VERSION 1

USTRUCT(BlueprintType)
struct FDustlandSurvivalSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) float Hunger = 100.f;
	UPROPERTY(BlueprintReadWrite) float Thirst = 100.f;
	UPROPERTY(BlueprintReadWrite) float Fatigue = 100.f;
	UPROPERTY(BlueprintReadWrite) float Energy = 100.f;
	UPROPERTY(BlueprintReadWrite) float BodyTemperature = 37.f;
};

USTRUCT(BlueprintType)
struct FDustlandWorldClockSaveData
{
	GENERATED_BODY()

	/** Total elapsed in-game hours since campaign start; 0-24 wraps for time-of-day. */
	UPROPERTY(BlueprintReadWrite) float TotalElapsedGameHours = 8.f;

	UPROPERTY(BlueprintReadWrite) int32 DayNumber = 1;
};

/**
 * UDustlandSaveGame
 *
 * Root persistence object for one save slot. This foundational version
 * covers Player, Survival, Inventory and World Clock state; Faction State,
 * Settlement State, Quest State, Vehicle State, Weather State and Discovery
 * State are declared as reserved arrays-of-struct extension points so later
 * systems slot in without breaking existing save files (see MigrateIfNeeded).
 */
UCLASS(BlueprintType)
class SAVE_API UDustlandSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UDustlandSaveGame();

	UPROPERTY(BlueprintReadWrite, Category = "Save|Meta")
	int32 SchemaVersion = DUSTLAND_SAVE_SCHEMA_VERSION;

	UPROPERTY(BlueprintReadWrite, Category = "Save|Meta")
	FString SaveSlotDisplayName;

	UPROPERTY(BlueprintReadWrite, Category = "Save|Meta")
	FDateTime SavedAtUTC;

	UPROPERTY(BlueprintReadWrite, Category = "Save|Meta")
	EDustlandDifficulty Difficulty = EDustlandDifficulty::Normal;

	UPROPERTY(BlueprintReadWrite, Category = "Save|Data")
	FDustlandSurvivalSaveData PlayerSurvival;

	UPROPERTY(BlueprintReadWrite, Category = "Save|Data")
	TArray<FDustlandItemInstance> PlayerInventory;

	UPROPERTY(BlueprintReadWrite, Category = "Save|Data")
	FDustlandWorldClockSaveData WorldClock;

	UPROPERTY(BlueprintReadWrite, Category = "Save|Data")
	FVector PlayerLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Save|Data")
	FRotator PlayerRotation = FRotator::ZeroRotator;

	/** Simple CRC32 of the serialized payload, used by IsIntegrityValid() to catch corrupted/truncated files. */
	UPROPERTY(BlueprintReadWrite, Category = "Save|Meta")
	int32 IntegrityChecksum = 0;

	/** Recomputes IntegrityChecksum from current field values. Call right before writing to disk. */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void RecomputeIntegrityChecksum();

	/** Verifies IntegrityChecksum matches current contents. Call right after loading from disk. */
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool IsIntegrityValid() const;

	/** Upgrades older schema versions in place. Safe to call unconditionally after loading. */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void MigrateIfNeeded();

private:
	int32 ComputeChecksum() const;
};
