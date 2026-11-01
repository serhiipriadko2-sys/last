# LAST LIGHT / Unreal migration changelog — v0.4
Date: 2026-11-01. Target: Unreal Engine 5.8.4, Windows x64 Development.

## Main changes
1. Rebuilt original Blender characters into six complete skeletal meshes and imported their seven source animation clips into Unreal.
2. Replaced primitive enemy markers in the exploration world with 30 skinned encounters and preserved five traversable zones.
3. Fixed procedural static-root attachment, visual player scale and orientation, and double processing of E during defense.
4. Fixed full map package paths; both Explore and Arena are cooked and present in .pak. Live packaged transition now passes.
5. Repaired start-of-run item reset, pending level-loss save/resume, and equipped IDs after forge destruction.
6. Added two-stage forge UX: first and second item, exact grade-dependent break chance preview, mandatory confirmation or non-destructive cancellation.
7. Expanded native Unreal Automation to 8 tests, including deterministic 100-to-0 descent, 20 evolution slots, strong-vs-weak mirror scaling, and forge risk acceptance.
8. Added versioned Windows BuildCookRun script, bundled archive inventory, SHA-256 receipt and packaged-game smoke logs.

## Verification evidence
- Source C++ Build: E:\LastLightUnreal\UBT_EDITOR_V04_BUILD.log
- Unreal Automation 8/8: E:\LastLightUnreal\TestReports08\index.json
- Windows BuildCookRun: E:\LastLightUnreal\UAT_V04_BUILDCOOKRUN.log, BUILD SUCCESSFUL
- Packaged Explore-to-Arena: E:\LastLightUnreal\SMOKE_PACKAGED_ARENA_V04.log, PASS
- Release manifest: E:\LastLightUnreal\Releases_v04\RELEASE_RECEIPT_UE_V04.json, 25/25 PASS

## Verification boundary
All 8 native tests are programmatic; 100-to-0 is a state-machine simulation, not a human playthrough of 20 encounters. Packaged runtime tests were conducted without image rendering using -nullrhi. This receipt does not certify image composition, walking direction of source meshes, full animation fidelity, UX accessibility, graphics FPS, controller support or music.

## Known next issues
- Provide verified in-engine screenshot or screenshot functional test before stating visual-pass; an external desktop capture was inconclusive.
- Build proper Animation Blueprint / BlendSpaces and enemy variation across 30 types.
- Expand enemy AI, attack telegraphs, alternate world paths and readability.
- Test save/load through physical GUI and full campaign with real inputs.
- Reconcile the prototype's fixed demo 5-losses-per-win with configurable production XP curves.
- Add user-facing skill search/filters and more accessible text scaling.
