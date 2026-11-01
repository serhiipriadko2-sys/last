using UnrealBuildTool;
using System.Collections.Generic;
public class LastLightUETarget : TargetRules
{
 public LastLightUETarget(TargetInfo Target) : base(Target)
 {
   Type=TargetType.Game;
   DefaultBuildSettings=BuildSettingsVersion.V7;
   IncludeOrderVersion=EngineIncludeOrderVersion.Latest;
   ExtraModuleNames.Add("LastLightUE");
 }
}
