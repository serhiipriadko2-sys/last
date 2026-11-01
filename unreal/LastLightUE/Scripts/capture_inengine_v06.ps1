param(
 [ValidateSet('Explore','Arena')][string]$Target='Arena',
 [int]$MaxSeconds=45
)
$ErrorActionPreference='Stop'
$root='E:\LastLightUnreal'
$exe=Join-Path $root 'Releases_v06\Windows\LastLightUE\Binaries\Win64\LastLightUE.exe'
$log=Join-Path $root ('INENGINE_V06_'+$Target.ToUpper()+'.log')
$image=Join-Path $root ('Releases_v06\Windows\LastLightUE\Saved\Screenshots\Windows\LL_QA\'+$Target+'_v05.png')
$since=Get-Date
$args=@('LastLightUE','-RenderOffScreen','-windowed','-ResX=1280','-ResY=720','-ForceRes','-unattended','-nosplash','-nosound','-LLScreenshotQA',('-abslog='+$log))
if($Target -eq 'Explore'){$args+='-LLQAExplore'}
else{$args+='-LLSmokeArena'}
$proc=Start-Process -FilePath $exe -WorkingDirectory (Split-Path $exe) -ArgumentList $args -PassThru
$created=$false
try {
 for($i=0;$i -lt $MaxSeconds;$i++){
  Start-Sleep -Seconds 1
  if(Test-Path $image){
   $stat=Get-Item $image
   if($stat.LastWriteTime -ge $since -and $stat.Length -gt 10000){$created=$true;break}
  }
  if($proc.HasExited){break}
 }
 $logs=@()
 if(Test-Path $log){$logs=Get-Content $log}
 Write-Host ('INENGINE_TARGET='+$Target+' SCREENSHOT_CREATED='+$created)
 $logs|Where-Object{$_ -match 'LL_SCREENSHOT_QA_REQUEST|LL_STARTPLAY:|Fatal error:|LL_ENEMY_RIG='}|Select-Object -Last 9|ForEach-Object{Write-Host $_}
 if(-not $created){throw ('INENGINE_'+$Target+'_SCREENSHOT_MISSING')}
 Add-Type -AssemblyName System.Drawing
 $bmp=[System.Drawing.Image]::FromFile($image)
 try{
  Write-Host ('INENGINE_IMAGE='+$image+' WIDTH='+$bmp.Width+' HEIGHT='+$bmp.Height+' BYTES='+(Get-Item $image).Length)
 }finally{$bmp.Dispose()}
 Write-Host ('INENGINE_IMAGE_SHA256='+((Get-FileHash $image -Algorithm SHA256).Hash.ToLower()))
} finally{
 if(-not $proc.HasExited){Stop-Process -Id $proc.Id -Force -ErrorAction SilentlyContinue}
 Write-Host 'INENGINE_CAPTURE_PROCESS_CLEANUP_COMPLETE'
}
