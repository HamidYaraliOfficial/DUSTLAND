#include "DustlandSaveGame.h"
#include "Serialization/BufferArchive.h"

UDustlandSaveGame::UDustlandSaveGame()
{
	SchemaVersion = DUSTLAND_SAVE_SCHEMA_VERSION;
	SavedAtUTC = FDateTime::UtcNow();
}

int32 UDustlandSaveGame::ComputeChecksum() const
{
	// A lightweight structural checksum, not a cryptographic hash: enough to
	// detect truncated/corrupted save files and accidental hand-editing,
	// which is the actual failure mode this guards against.
	FBufferArchive Archive;

	int32 Version = SchemaVersion;
	Archive << Version;

	float H = PlayerSurvival.Hunger, T = PlayerSurvival.Thirst,
		F = PlayerSurvival.Fatigue, E = PlayerSurvival.Energy, B = PlayerSurvival.BodyTemperature;
	Archive << H << T << F << E << B;

	int32 ItemCount = PlayerInventory.Num();
	Archive << ItemCount;
	for (const FDustlandItemInstance& Item : PlayerInventory)
	{
		FName ItemIdCopy = Item.ItemID;
		int32 StackCopy = Item.StackCount;
		float ConditionCopy = Item.Condition;
		Archive << ItemIdCopy << StackCopy << ConditionCopy;
	}

	float Elapsed = WorldClock.TotalElapsedGameHours;
	int32 Day = WorldClock.DayNumber;
	Archive << Elapsed << Day;

	FVector Loc = PlayerLocation;
	Archive << Loc;

	uint32 Crc = FCrc::MemCrc32(Archive.GetData(), Archive.Num());
	return static_cast<int32>(Crc);
}

void UDustlandSaveGame::RecomputeIntegrityChecksum()
{
	IntegrityChecksum = ComputeChecksum();
}

bool UDustlandSaveGame::IsIntegrityValid() const
{
	return IntegrityChecksum == ComputeChecksum();
}

void UDustlandSaveGame::MigrateIfNeeded()
{
	// Schema version 1 is the current baseline established by this build -
	// nothing to migrate yet. Future versions add an `if (SchemaVersion < N)`
	// block per step so a save can hop forward through several versions in
	// one call, e.g.:
	//
	//   if (SchemaVersion < 2) { /* populate newly-added FactionState array with defaults */ SchemaVersion = 2; }
	//   if (SchemaVersion < 3) { /* ... */ SchemaVersion = 3; }

	if (SchemaVersion < DUSTLAND_SAVE_SCHEMA_VERSION)
	{
		SchemaVersion = DUSTLAND_SAVE_SCHEMA_VERSION;
	}
}
