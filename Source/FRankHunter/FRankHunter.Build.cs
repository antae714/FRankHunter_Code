// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class FRankHunter : ModuleRules
{
	public FRankHunter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { "FRankHunter" });
	
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"UMG",
			"AdvancedSessions",
			"AdvancedSteamSessions",
			"DBNetwork",
			"NetCore",
			"SimpleSaveKit",
            "CommonUI",
			"CommonInput",
			"OpenSSL"
		});

        PrivateDependencyModuleNames.AddRange(new string[] {
			"NavigationSystem",
			"AnimGraphRuntime",
            "GameplayAbilities",
			"SteamCloudSave",
			"GameplayTags",
			"GameplayTasks",
			"OnlineSubsystem",
			"OnlineSubsystemSteam",
			"OnlineSubsystemUtils",
			"Steamworks",
			"Networking",
			"AssetRegistry",
            "HairStrandsCore",
			"ApplicationCore",
			"Niagara",
			"FieldSystemEngine",
			"GeometryCollectionEngine",
			"Chaos",
            "Slate",
            "SlateCore",
            "SimpleInventory",
            "PropertyPath",
            "AudioModulation",
            "AudioMixer",
            "RHI",
			"PhysicsCore",
		});

		AddEngineThirdPartyPrivateStaticDependencies(Target, "Steamworks");

		PublicIncludePaths.AddRange(new string[] {
            ModuleDirectory
		});
    }
}
