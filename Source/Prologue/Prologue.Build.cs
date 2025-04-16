// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Prologue : ModuleRules
{
	public Prologue(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
