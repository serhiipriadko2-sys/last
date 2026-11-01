param([switch]$EditorOnly)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$projectFile = Join-Path $projectRoot 'LastLightUE.uproject'
$engineRoot = 'C:\Program Files\Epic Games\UE_5.8'
$releaseDir = Join-Path $projectRoot 'Releases\v08'
$logDir = Join-Path $projectRoot 'Saved\Verification\v08'
New-Item -ItemType Directory -Force -Path $releaseDir,$logDir | Out-Null
[Environment]::SetEnvironmentVariable('UE-LocalDataCachePath',(Join-Path $projectRoot 'DerivedDataCache'),'Process')
$targets = @('LastLightUEEditor')
if (!$EditorOnly) { $targets += 'LastLightUE' }
foreach ($target in $targets) {
    $compileLog = Join-Path $logDir ($target + '.log')
    & (Join-Path $engineRoot 'Engine\Build\BatchFiles\Build.bat') $target Win64 Development "-Project=$projectFile" -WaitMutex -NoUBA -NoUbaLocal *> $compileLog
    Write-Output "$target exit=$LASTEXITCODE log=$compileLog"
    if ($LASTEXITCODE -ne 0) { Get-Content $compileLog -Tail 45; throw "Compile failed: $target" }
}
if ($EditorOnly) { exit 0 }
$cookLog = Join-Path $logDir 'Package.log'
& (Join-Path $engineRoot 'Engine\Build\BatchFiles\RunUAT.bat') BuildCookRun "-project=$projectFile" -platform=Win64 -clientconfig=Development -skipbuild -cook -cookall -stage -pak -SkipIoStore -package -archive "-archivedirectory=$releaseDir" -nop4 -utf8output *> $cookLog
Write-Output "Packaging exit=$LASTEXITCODE log=$cookLog"
if ($LASTEXITCODE -ne 0) { Get-Content $cookLog -Tail 45; throw 'Packaging failed' }
$executable = Join-Path $releaseDir 'Windows\LastLightUE.exe'
if (!(Test-Path -LiteralPath $executable)) { throw 'Missing packaged executable' }
Get-Item -LiteralPath $executable | Select-Object FullName,Length
Write-Output 'LASTLIGHT_V08_BUILD_PASS'
