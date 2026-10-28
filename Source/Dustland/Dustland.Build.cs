// Dustland.Build.cs
// The primary game module. Thin by design: it wires the gameplay modules
// (Survival, Inventory, Economy, Save, ...) onto actors (GameMode, Character)
// but keeps the actual system logic inside those dedicated modules so each
// one stays independently testable and reusable.

using UnrealBuildTool;

public class Dustland : ModuleRules
{
	public Dustland(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"UMG",
			"DustlandCore",
			"Survival",
			"Inventory",
			"Economy",
			"Save",
			"Weather",
			"Equipment"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
	}
}
