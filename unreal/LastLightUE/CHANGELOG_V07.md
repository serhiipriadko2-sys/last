# LAST LIGHT — V07 engineering changelog (2026-11-01)

Status: SOURCE_MODIFIED / BUILD_IN_PROGRESS until test and release receipts are verified. V06 remains the validated, reversible Windows build.

## Implemented in source
- F2 cycles Normal -> Moderate -> Wide timing assist. Defense timing windows gain 0.08 or 0.16 seconds; attack timing uses pre-existing Assist multiplier.
- F2 timing mode is rendered in the game HUD (left stats panel) and stored as optional `timing_assist` in the save. Existing version 2 saves without this key default to 0.0.
- Mirror boss retains its immutable starting 50-skill snapshot but telegraphs opening first skill and subsequent top-six repertoire by power and deterministic ID sorting. Chosen skill power changes individual attack multiplier around snapshot average.
- 2 native Unreal automation tests added for timing assist and mirror pattern (suite target = 10 tests).
- Removed legacy Android File Server development configuration with a token from public project config.
- Added Codex AGENTS.md, SoT30 full 30-file hash-verified source mirror, VS Code tasks, design/acceptance documents, immutable release scripts.

## Verification gates
Run `Scripts/verify_sot30.ps1`, `Scripts/test_editor_v07.ps1`, `Scripts/build_windows_v07.ps1`, `Scripts/smoke_arena.ps1 -Mode Packaged -ReleaseTag v07`, `python Scripts/verify_windows_release_v07.py`. Do not mark release PASS from the source changes alone.

## Known scope boundary
Not an AAA/production final. Human full playtest, movement feel, controls, animation blending, sounds, controller, accessibility, performance across GPUs are not certified. The existing v0.6 has 8/8 tests and previous packaged smoke; this does not grant v0.7 automatic approval.
