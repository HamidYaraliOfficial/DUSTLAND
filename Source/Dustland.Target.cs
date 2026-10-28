// Dustland.Target.cs
// Runtime (game) build target. Declares every custom module that must be
// compiled into the final executable. Keep this list in sync with
// Dustland.uproject "Modules" and with any new Source/<Module> folder added.

using UnrealBuildTool;
using System.Collections.Generic;

public class DustlandTarget : TargetRules
{
	public DustlandTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new string[]
		{
			"Dustland",
			"DustlandCore",
			"Survival",
			"Inventory",
			"Economy",
			"Save",
			"Weather",
			"Equipment",
			"Crafting"
		});
	}
}
