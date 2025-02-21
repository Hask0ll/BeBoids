// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BeBoids : ModuleRules
{
	public BeBoids(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
