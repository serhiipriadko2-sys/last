$ErrorActionPreference="Stop"
$root="E:\LastLightUnreal\LastLightUE"
$proj=Join-Path $root "LastLightUE.uproject"
$engine="C:\Program Files\Epic Games\UE_5.8"
$out="E:\LastLightUnreal\Releases_v06"
$log="E:\LastLightUnreal\UAT_V06_BUILDCOOKRUN.log"
[Environment]::SetEnvironmentVariable("UE-LocalDataCachePath","E:\LastLightUnreal\DDC","Process")
New-Item -ItemType Directory -Path $out -Force | Out-Null
Write-Host "UNREAL_WINDOWS_PACKAGING_STARTED"
# Build locally without the stalled UBA executor. Preserve both editor and game binaries.
foreach($target in @("LastLightUEEditor","LastLightUE")){
  $buildLog=Join-Path "E:\LastLightUnreal" ("Build_"+$target+"_NoUBA.log")
  & (Join-Path $engine "Engine\Build\BatchFiles\Build.bat") $target Win64 Development "-Project=$proj" -WaitMutex -NoUBA -NoUbaLocal *> $buildLog
  $compileExit=$LASTEXITCODE
  Write-Host ($target+"_COMPILE_EXIT="+$compileExit)
  if($compileExit -ne 0){Get-Content $buildLog|Select-Object -Last 30;throw "COMPILE_FAIL_$target"}
}
& (Join-Path $engine "Engine\Build\BatchFiles\RunUAT.bat") BuildCookRun "-project=$proj" -platform=Win64 -clientconfig=Development -skipbuild -cook -cookall -stage -pak -SkipIoStore -package -archive "-archivedirectory=$out" -nop4 -utf8output *> $log
$exit=$LASTEXITCODE
Write-Host ("UAT_EXIT="+$exit)
Get-Content $log | Select-Object -Last 42
if($exit -ne 0){throw "UAT_BUILDCOOKRUN_FAILED"}
$exes=Get-ChildItem $out -Recurse -File -Filter "LastLightUE.exe"
if(-not $exes){throw "COOK_PASS_BUT_NO_WINDOWS_EXECUTABLE"}
foreach($e in $exes){Write-Host ("EXE="+$e.FullName+" BYTES="+$e.Length)}
Write-Host "UNREAL_WINDOWS_PACKAGING_PASS"
