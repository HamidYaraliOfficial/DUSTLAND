// DustlandCore.Build.cs
// Shared types, enums and interfaces used by every other Dustland module.
// NOTE: the module is named "DustlandCore" (not "Core") to avoid a name
// collision with Unreal Engine's own built-in "Core" module.

using UnrealBuildTool;

public class DustlandCore : ModuleRules
{
	public DustlandCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}
