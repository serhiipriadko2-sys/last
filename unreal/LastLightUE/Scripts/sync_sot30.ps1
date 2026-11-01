param([string]$Commit='e38ecb00c07176c7a1b0dcbee24ccac3cbe5f786')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$target=Join-Path $root 'Docs\IskraSoT30'
$origin='https://raw.githubusercontent.com/serhiipriadko2-sys/iskra/'+$Commit+'/governance/releases/2026-07-30-sot30-v5-5-7-audit-repair/'
New-Item -ItemType Directory -Force -Path (Join-Path $target 'knowledge'),(Join-Path $target 'support') | Out-Null
function Fetch-Source([string]$relative) {
 $path=Join-Path $target ($relative.Replace('/','\'))
 $uri=$origin+$relative
 Invoke-WebRequest -Uri $uri -OutFile $path -UseBasicParsing -TimeoutSec 45
 if(-not (Test-Path $path)){throw "MISSING: $relative"}
}
Fetch-Source 'support/MANIFEST.json'
Fetch-Source 'support/PROJECT_INSTRUCTIONS_SOT30.md'
$manifest=Get-Content (Join-Path $target 'support\MANIFEST.json') -Raw -Encoding utf8 | ConvertFrom-Json
if($manifest.package_version -ne 'v5.5.7' -or $manifest.files.Count -ne 30){throw 'SOT30_MANIFEST_INVALID'}
foreach($entry in $manifest.files){
 Fetch-Source $entry.path
 $path=Join-Path $target ($entry.path.Replace('/','\'))
 $sha=(Get-FileHash -Algorithm SHA256 -Path $path).Hash.ToLowerInvariant()
 if($sha -ne $entry.sha256){throw "SOT30_HASH_MISMATCH $($entry.path)"}
 Write-Host ("VERIFIED "+$entry.path)
}
Write-Host ("SOT30_VERIFIED_FILES="+$manifest.files.Count)
Write-Host ("SOT30_TARGET="+$target)
