$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$source=Join-Path $root 'Docs\IskraSoT30'
$manifestPath=Join-Path $source 'support\MANIFEST.json'
if(-not(Test-Path $manifestPath)){throw 'SOT30_MANIFEST_MISSING'}
$manifest=Get-Content $manifestPath -Raw -Encoding UTF8 | ConvertFrom-Json
if($manifest.package_version -ne 'v5.5.7' -or $manifest.files.Count -ne 30){throw 'SOT30_MANIFEST_INVALID'}
$sum=[long]0
foreach($entry in $manifest.files){
 if($entry.path -notmatch '^knowledge/[0-2][0-9]_[A-Za-z0-9_]+\.md$'){throw "UNSAFE_MANIFEST_PATH: $($entry.path)"}
 $p=Join-Path $source ($entry.path.Replace('/','\'))
 if(-not(Test-Path $p)){throw "MISSING $($entry.path)"}
 $info=Get-Item $p
 $sha=(Get-FileHash $p -Algorithm SHA256).Hash.ToLowerInvariant()
 if($info.Length -ne $entry.bytes -or $sha -ne $entry.sha256){throw "DRIFT $($entry.path)"}
 $sum+=$info.Length
}
if($sum -ne $manifest.corpus_bytes){throw "CORPUS_BYTES_DRIFT: $sum"}
$instructions=Join-Path $source 'support\PROJECT_INSTRUCTIONS_SOT30.md'
if(-not(Test-Path $instructions)){throw 'SOT30_INSTRUCTIONS_MISSING'}
if((Get-Item $instructions).Length -ne $manifest.project_instructions_bytes){throw 'SOT30_INSTRUCTIONS_BYTES_DRIFT'}
Write-Host ("SOT30_MANIFEST_PASS files=30 bytes="+$sum)
Write-Host ("SOT30_PACKAGE="+$manifest.package_version)
