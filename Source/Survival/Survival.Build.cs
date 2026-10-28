// Survival.Build.cs
using UnrealBuildTool;

public class Survival : ModuleRules
{
	public Survival(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"DustlandCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "NetCore" });
	}
}
