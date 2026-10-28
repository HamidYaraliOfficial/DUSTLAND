# Dustland — Architecture Overview

## Module layout

```
Source/
  Dustland/     Primary game module: GameMode, World Manager wiring, entry point.
  Core/         (module name "DustlandCore") Shared enums/structs used by every other module.
  Survival/     Hunger / Thirst / Fatigue / Energy / Body Temperature.
  Inventory/    Data-driven item definitions + weight/volume-limited containers.
  Economy/      Operating-hours / market-open logic (Trading Posts, Settlements, Radio windows).
  Save/         Versioned SaveGame object with integrity checksum + migration hook.
  Weather/      World clock (day/night, hour/day tracking) + dynamic weather with weighted transitions.
  Equipment/    Equip/unequip slots, insulation, armor, bonus carry capacity, condition decay.
  Crafting/     Data-driven recipes, crafting-knowledge unlocks, timed crafting-station jobs.
```

Each gameplay module depends only on `DustlandCore` (and, where genuinely
needed, on one sibling module — e.g. `Save` and `Equipment` depend on
`Inventory` because they operate on item instances). No gameplay module
depends on `Dustland` itself, which keeps them reusable and independently
testable. `Weather` and `Equipment` do **not** depend on `Survival`, and
`Weather` does not depend on `Economy` — the primary `Dustland` module's
`ADustlandGameModeBase` is the only place that reads from one and writes
into another (see "Cross-system wiring" below). This is deliberate: it
means Weather, Equipment and Economy can each be unit-tested and reused
(e.g. a Trading Post's operating hours) without ever compiling in Survival.

## Cross-system wiring (the seed of the World Simulation Engine)

`ADustlandGameModeBase` owns the session's one `UWorldClockComponent` and
`UWeatherSystemComponent` and, on a quarter-second interval, walks every
actor in the world and:

- pushes `WorldClock` hour + `WeatherSystem` temperature modifier into every
  `USurvivalComponent::SetAmbientTemperature`
- pushes `UEquipmentComponent::GetTotalInsulation` into
  `USurvivalComponent::EquipmentInsulationFactor` for actors that have both
- pushes the current hour into every `UOperatingHoursComponent::UpdateFromWorldClock`

This "GameMode walks all actors" approach is intentionally simple and does
not scale to hundreds of NPCs — it is a correct, testable placeholder for
what later becomes a proper registry/subsystem with Simulation LOD (see
Roadmap below), not the final architecture.

## Why this split

- **DustlandCore** exists purely so two gameplay modules can share an enum
  (e.g. `EDustlandItemType`) without creating a dependency between them.
- **Survival**, **Inventory**, **Economy**, **Equipment** and **Crafting**
  are Actor Components rather than singletons: the exact same
  `UInventoryComponent` backs the player backpack, a vehicle's cargo hold, a
  settlement storage building and a lootable footlocker. The exact same
  `UOperatingHoursComponent` backs a Trading Post, a Settlement gate, or a
  Radio Tower's broadcast window. The exact same `UCraftingStationComponent`
  backs a Workbench, an Electronics Bench or a Repair Bench — only
  `StationTag` and the recipe table differ.
- **Save** is deliberately minimal today (player survival + inventory +
  world clock + integrity check) with a documented migration path
  (`MigrateIfNeeded`) so Faction/Settlement/Quest/Vehicle/Weather state can
  be added in later milestones without breaking existing save files.

## Roadmap (not yet implemented in this build)

World Simulation & Faction AI · Settlement construction & population ·
Vehicle physics & garages · Research trees · Quest & Dialogue graphs ·
Companion AI · NPC schedules/memory/relationships · Wildlife ecosystem & AI
archetypes · Scavenging & loot tables · Trade caravans · Radio network &
world news · Modding SDK & Scenario Editor · Photo Mode · Full
UMG/CommonUI theme set (Dark, Light, Desert Amber, Rust Red, Blue Tech,
Green Survival, High Contrast) · Simulation LOD to replace the
GetAllActorsOfClass sync loop described above once actor counts grow.

Basic Weather (9 types, weighted random transitions), a day/night ambient
temperature curve, Equipment slots/insulation/armor/condition, and
data-driven Crafting with timed station jobs **are** implemented (see table
above) — what remains for those systems specifically is: biome-specific
weather tables, road/travel effects, multi-stage crafting chains, and
research-gated recipe unlocks.

Anything not listed in the module table above and not called out as
implemented in this section does not yet exist in the codebase. This
document is kept up to date as each system lands.

## Coding conventions

- Gameplay logic in C++; Blueprint reserved for animation graphs, UI
  composition and designer-facing content scripting.
- Every stat/threshold that affects game balance is an `EditAnywhere`
  `UPROPERTY`, never a hardcoded literal, so designers tune without
  recompiling.
- Public API on components favors `BlueprintCallable`/`BlueprintPure`
  functions over exposing raw mutable state, so UMG widgets and Blueprint
  event graphs have a stable contract.
