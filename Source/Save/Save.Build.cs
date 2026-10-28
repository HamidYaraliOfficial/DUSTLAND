// Save.Build.cs
using UnrealBuildTool;

public class Save : ModuleRules
{
	public Save(ReadOnlyTargetRules Target) : base(Target)
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
