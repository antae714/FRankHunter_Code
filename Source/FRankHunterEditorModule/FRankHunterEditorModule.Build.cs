// Copyright Epic Games, Inc. All Rights Reserved.
using UnrealBuildTool;
 
public class FRankHunterEditorModule : ModuleRules
{
	 public FRankHunterEditorModule(ReadOnlyTargetRules Target) : base(Target)
	 {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UnrealEd",         // 에디터 기능에 필요
                "PropertyEditor",   // Property Customization에 필요
                "LevelEditor",
				"FRankHunter"       // 구조체가 있는 메인 게임 모듈
            }
        );
    }
}