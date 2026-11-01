param(
 [ValidateSet('Editor','Packaged')][string]$Mode='Editor',
 [int]$MaxSeconds=65,
 [string]$ReleaseTag='v04'
)
$ErrorActionPreference='Stop'
$root='E:\LastLightUnreal'
if($Mode -eq 'Editor'){
  $exe='C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe'
  $working=Split-Path $exe
  $args=@('E:\LastLightUnreal\LastLightUE\LastLightUE.uproject','/Game/Maps/Explore','-game','-nullrhi','-LLSmokeArena','-unattended','-nosplash')
}else{
  $exe="E:\LastLightUnreal\Releases_$ReleaseTag\Windows\LastLightUE.exe"
  $working=Split-Path $exe
  $args=@('-nullrhi','-LLSmokeArena','-unattended','-nosplash')
}
$logName=('SMOKE_'+$Mode.ToUpper()+'_ARENA.log')
if($Mode -eq 'Packaged'){$logName='SMOKE_PACKAGED_ARENA_'+$ReleaseTag.ToUpper()+'.log'}
$log=Join-Path $root $logName
if(Test-Path $log){Remove-Item $log -Force}
$args+=('-abslog='+$log)
$proc=Start-Process -FilePath $exe -WorkingDirectory $working -ArgumentList $args -PassThru
$arena=$false
$start=Get-Date
$timeout=$MaxSeconds
for($i=0;$i -lt $timeout;$i++){
  Start-Sleep -Seconds 1
  if(Test-Path $log){
    $text=Get-Content $log -Raw -ErrorAction SilentlyContinue
    if($text -match 'LL_STARTPLAY: level=Arena arena=YES'){
      $arena=$true
      if($text -match 'LL_ENEMY_RIG=YES'){break}
    }
  }
  if($proc.HasExited){break}
}
if(Test-Path $log){
  $text=Get-Content $log -Raw -ErrorAction SilentlyContinue
  $lines=Get-Content $log
}else{$text='';$lines=@()}
$map=($text -match 'LL_STARTPLAY: level=Explore arena=NO')
$hero=($text -match 'LL_HERO_RIG=YES')
$enemy=($text -match 'LL_ENEMY_RIG=YES')
$worldRigs=($text -match 'LL_WORLD_RIGS=30')
$badAttach=@($lines|Where-Object{$_ -match 'AttachTo:.*cannot attach'}).Count
$badRig=@($lines|Where-Object{$_ -match 'LL_ENEMY_RIG=NO|LL_HERO_RIG=NO'}).Count
$errors=@($lines|Where-Object{$_ -match 'Fatal error:|Assertion failed:|LogWindows: Error:'}).Count
Write-Host ("SMOKE_MODE="+$Mode+" EXPLORE="+$map+" ARENA="+$arena+" HERO_RIG="+$hero+" ENEMY_RIG="+$enemy+" WORLD_RIGS_30="+$worldRigs+" ATTACH_WARNINGS="+$badAttach+" BAD_RIG="+$badRig+" FATAL="+$errors)
$lines|Where-Object{$_ -match 'LL_DATABASE:|LL_STARTPLAY:|LL_ENEMY_RIG=|LL_HERO_RIG=|LL_ARENA_STAGE:|LL_EXPLORE_STAGE:|LL_WORLD_RIGS='}|Select-Object -Last 22|ForEach-Object{Write-Host $_}
if(-not $proc.HasExited){Stop-Process -Id $proc.Id -Force -ErrorAction SilentlyContinue}
# The packaged launcher spawns a child game EXE. Stop only children launched
# with this exact smoke-test log, never an unrelated playable game session.
$testArgument=[regex]::Escape('-abslog='+$log)
Get-CimInstance Win32_Process -Filter "name='LastLightUE.exe'" -ErrorAction SilentlyContinue |
 Where-Object{$_.CommandLine -match $testArgument} |
 ForEach-Object{Stop-Process -Id $_.ProcessId -Force -ErrorAction SilentlyContinue}
if(-not($map -and $arena -and $hero -and $enemy -and $worldRigs) -or $badAttach -gt 0 -or $badRig -gt 0 -or $errors -gt 0){
  throw ('FAILED_'+$Mode+'_ARENA_SMOKE')
}
Write-Host ("SMOKE_"+$Mode.ToUpper()+"_ARENA_PASS")
