// Crafting.Build.cs
using UnrealBuildTool;

public class Crafting : ModuleRules
{
	public Crafting(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"DustlandCore",
			"Inventory"
		});
	}
}
