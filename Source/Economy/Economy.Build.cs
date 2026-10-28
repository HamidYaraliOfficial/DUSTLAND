// Economy.Build.cs
using UnrealBuildTool;

public class Economy : ModuleRules
{
	public Economy(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"DustlandCore"
		});
	}
}
