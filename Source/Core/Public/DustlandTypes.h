#pragma once

#include "CoreMinimal.h"
#include "DustlandTypes.generated.h"

/**
 * Global difficulty presets. Difficulty is intentionally NOT a single damage
 * multiplier: each preset also drives resource scarcity, AI awareness,
 * weather severity, save rules and quest timers. See Docs/ARCHITECTURE.md.
 */
UENUM(BlueprintType)
enum class EDustlandDifficulty : uint8
{
	Story        UMETA(DisplayName = "Story"),
	Normal       UMETA(DisplayName = "Normal"),
	SurvivalMode UMETA(DisplayName = "Survival"),
	Hardcore     UMETA(DisplayName = "Hardcore"),
	Custom       UMETA(DisplayName = "Custom")
};

UENUM(BlueprintType)
enum class EDustlandItemRarity : uint8
{
	Common    UMETA(DisplayName = "Common"),
	Uncommon  UMETA(DisplayName = "Uncommon"),
	Rare      UMETA(DisplayName = "Rare"),
	Exceptional UMETA(DisplayName = "Exceptional"),
	Unique    UMETA(DisplayName = "Unique")
};

UENUM(BlueprintType)
enum class EDustlandItemType : uint8
{
	Food,
	Water,
	Medical,
	Tool,
	Material,
	Component,
	Electronics,
	Clothing,
	Armor,
	Weapon,
	WeaponAttachment,
	Ammo,
	Document,
	Artifact,
	QuestItem,
	Blueprint,
	Misc
};

UENUM(BlueprintType)
enum class EDustlandSurvivalLevel : uint8
{
	Fine,
	Stressed,
	Critical,
	Failing
};

/** Generic three-language locale identifier used by UI/localization systems. */
UENUM(BlueprintType)
enum class EDustlandLocale : uint8
{
	English  UMETA(DisplayName = "English"),
	Persian  UMETA(DisplayName = "Persian (Farsi)"),
	Chinese  UMETA(DisplayName = "Chinese (Simplified)")
};

/** Text flow direction derived from EDustlandLocale, used by the UI layer. */
UENUM(BlueprintType)
enum class EDustlandTextDirection : uint8
{
	LeftToRight,
	RightToLeft
};

USTRUCT(BlueprintType)
struct DUSTLANDCORE_API FDustlandItemTags
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TArray<FName> Tags;

	bool Has(FName Tag) const { return Tags.Contains(Tag); }
};

/**
 * Helper library shared by every module that needs to know whether a locale
 * renders right-to-left (Persian) or left-to-right (English, Chinese).
 */
UCLASS()
class DUSTLANDCORE_API UDustlandLocalizationHelpers : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Dustland|Localization")
	static EDustlandTextDirection GetTextDirection(EDustlandLocale Locale)
	{
		return (Locale == EDustlandLocale::Persian)
			? EDustlandTextDirection::RightToLeft
			: EDustlandTextDirection::LeftToRight;
	}
};
