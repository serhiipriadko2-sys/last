# LAST LIGHT — Acceptance matrix (V07)
Status is **pending until fresh reports exist**. Prior V06 receipts are baseline evidence, not certification of V07.

| Gate | How to check | Pass condition |
|---|---|---|
| Iskra files | Scripts/verify_sot30.ps1 | All 30 SHA-256 and byte counts match manifest |
| Source safety | Git staged diff and token scan | No secrets, user save, caches, or binaries checked in |
| Editor C++ | Scripts/build_windows_v07.ps1 | UEEditor and UE Win64 compile exits 0 |
| Native tests | Scripts/test_editor_v07.ps1 | 10 named LastLight.Migration tests, 0 failures |
| Cook and archive | Scripts/build_windows_v07.ps1 | UAT BuildCookRun success and Windows EXE |
| Packaged runtime | Scripts/smoke_arena.ps1 -Mode Packaged -ReleaseTag v07 | Explore→Arena, skinned hero+enemy, 30 world rigs, no fatal |
| Release QC | Scripts/verify_windows_release_v07.py | Valid Pak, tests, runtime, inventory and SHA receipt |
| UI controls | Human play | WASD, RMB camera, E battle, skill selection, Q/E QTE, F2 assists, Tab, inventory |
| Save continuity | Human play on a copy of save | F5/F9, pending losses, upgraded gear, optional assist retained |
| 100→0 | Native test plus human play | 20 demo wins, 100 losses, level never under zero, final mirror |
| Difficulty | Human play | normal, moderate and wide parry options feel different and fair |
| Accessibility | Human play | readable text/contrast, keyboard flow; gamepad/colorblind not yet certified |
| Performance | Hardware test | GPU/FPS/latency measurements; cannot certify from null-RHI logs |
| Art/sound | Human and art review | no broken rigs/clipping, suitable telegraph/ambience; sound not yet complete |

## Shell entrypoints
`powershell -NoProfile -ExecutionPolicy Bypass -File Scripts/verify_sot30.ps1`
`powershell -NoProfile -ExecutionPolicy Bypass -File Scripts/test_editor_v07.ps1`
`powershell -NoProfile -ExecutionPolicy Bypass -File Scripts/build_windows_v07.ps1`
`powershell -NoProfile -ExecutionPolicy Bypass -File Scripts/smoke_arena.ps1 -Mode Packaged -ReleaseTag v07`
`python Scripts/verify_windows_release_v07.py`

## Engineering receipts
Find dated run evidence at `E:\LastLightUnreal\TestReportsV07\index.json`, `E:\LastLightUnreal\UAT_V07_BUILDCOOKRUN.log`, `E:\LastLightUnreal\SMOKE_PACKAGED_ARENA_V07.log`, `E:\LastLightUnreal\Releases_v07\RELEASE_RECEIPT_UE_V07.json`.

## Open loops, no fake PASS
Controller and lower-end hardware, full human playthrough, accessibility audit, native audio, FPS chart, natural 100-level pacing, and world asset quality. Game must not be called AAA/production-ready without these checks. Existing V06 remains fallback.
