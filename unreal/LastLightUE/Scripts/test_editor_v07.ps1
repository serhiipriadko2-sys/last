$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$engine='C:\Program Files\Epic Games\UE_5.8'
$editor=Join-Path $engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$project=Join-Path $root 'LastLightUE.uproject'
$report='E:\LastLightUnreal\TestReportsV07'
$log='E:\LastLightUnreal\UE_AUTOMATION_V07.log'
if(-not(Test-Path $editor)){throw 'UNREAL_EDITOR_NOT_FOUND'}
if(-not(Test-Path $project)){throw 'UPROJECT_NOT_FOUND'}
New-Item -ItemType Directory -Force -Path $report | Out-Null
& $editor $project -nullrhi -unattended -nop4 '-ExecCmds=Automation RunTest LastLight.Migration;Quit' "-ReportExportPath=$report" "-abslog=$log"
$exitCode=$LASTEXITCODE
if($exitCode -ne 0){throw "UE_AUTOMATION_EXEC_EXIT=$exitCode"}
$index=Join-Path $report 'index.json'
if(-not(Test-Path $index)){throw 'UE_AUTOMATION_REPORT_MISSING'}
$result=Get-Content $index -Raw | ConvertFrom-Json
Write-Host "NATIVE_TESTS succeeded=$($result.succeeded) failed=$($result.failed) total=$($result.tests.Count)"
if($result.succeeded -ne 10 -or $result.failed -ne 0 -or $result.tests.Count -ne 10){throw 'UE_NATIVE_TESTS_NOT_ALL_PASS'}
Write-Host 'UE_NATIVE_10_OF_10_PASS'
