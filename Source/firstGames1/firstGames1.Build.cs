// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class firstGames1 : ModuleRules
{
	public firstGames1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
