// Equipment.Build.cs
using UnrealBuildTool;

public class Equipment : ModuleRules
{
	public Equipment(ReadOnlyTargetRules Target) : base(Target)
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
