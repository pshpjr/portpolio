// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class client : ModuleRules
{
	public client(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Json", "UMG" });
		PublicDependencyModuleNames.Add("ImGui");

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "AnimGraphRuntime" });

		string ThirdPartyDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty"));
		PublicIncludePaths.Add(ThirdPartyDir);
		PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyDir, "libprotobuf.lib"));

		bUseRTTI = true;
		bEnableExceptions = true;
		bEnableUndefinedIdentifierWarnings = false;
		ShadowVariableWarningLevel = WarningLevel.Off;
		PublicDefinitions.Add("GOOGLE_PROTOBUF_NO_RTTI=0");
		PublicDefinitions.Add("GOOGLE_PROTOBUF_INTERNAL_DONATE_STEAL_INLINE=0");

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
