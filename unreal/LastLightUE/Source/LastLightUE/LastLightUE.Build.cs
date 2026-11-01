using UnrealBuildTool;
public class LastLightUE : ModuleRules
{
 public LastLightUE(ReadOnlyTargetRules Target) : base(Target)
 {
   PCHUsage=PCHUsageMode.UseExplicitOrSharedPCHs;
   PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Json", "JsonUtilities", "Slate", "SlateCore", "UMG" });
   PrivateDependencyModuleNames.AddRange(new string[] { "Projects" });
 }
}
