// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Timestep : ModuleRules
{
	public Timestep(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore",
            "PhysX", "APEX" // PhysX
        });
	}
}
