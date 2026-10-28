// Weather.Build.cs
using UnrealBuildTool;

public class Weather : ModuleRules
{
	public Weather(ReadOnlyTargetRules Target) : base(Target)
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
