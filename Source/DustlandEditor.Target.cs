// DustlandEditor.Target.cs
// Editor build target. Mirrors DustlandTarget but with Type = Editor so the
// full toolset (Blueprint compiler, Scenario Editor, Content Browser, etc.)
// is linked in for iteration inside the Unreal Editor.

using UnrealBuildTool;
using System.Collections.Generic;

public class DustlandEditorTarget : TargetRules
{
	public DustlandEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
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
