# LAST LIGHT V07 — verified Windows prototype (2026-11-01)

## Acceptance verdict
**PASS: 26/26 release checks** on the user's Windows computer. This is a verified, launchable **3D game prototype**, not a production-complete or AAA game. A full manual 100→0 playthrough and a usability/FPS study on multiple GPUs are not certified.

## Run the installed playable build
- Launch helper: `E:\LastLightUnreal\Releases_v07\PLAY_LASTLIGHT_V07.cmd`.
- Packaged Win64 executable: `E:\LastLightUnreal\Releases_v07\Windows\LastLightUE.exe`.
- The complete Windows folder must remain together. Development binaries are not substitutes for the packaged build.

## Concrete receipts
| Verification | Observed status |
| --- | --- |
| Clean V07 Unreal Editor C++ compile | PASS, 14 compile/link actions, UnrealBuildTool Succeeded |
| Clean V07 Unreal Game Win64 compile | PASS, 18 actions, UnrealBuildTool Succeeded |
| Native UE automation tests | **10/10 PASS**, 0 failed, `E:\LastLightUnreal\TestReportsV07\index.json` |
| Win64 Cook, Stage, Pak, Package, Archive | **PASS**, `UAT_V07_BUILDCOOKRUN.log`, AutomationTool ExitCode 0 |
| Fresh packaged gameplay smoke | **PASS**, world Explore → arena with imported hero and enemy skeletal rigs; 30 enemy rigs; 0 critical errors |
| Release filesystem, PAK and runtime QA | **26/26 PASS**, 53 package files, **950,847,196 bytes** |
| Full SoT30 v5.5.7 | **30/30 verified**, 4,054,425 corpus bytes, every SHA-256 matched |
| Codex read-only self-check | **PASS**, independently loaded AGENTS and verified 30/30 SoT files + canonical Kernel Order |
| Release verification receipt | `Docs/Receipts/RELEASE_RECEIPT_UE_V07.json`; 14,964 bytes, SHA-256 `1de9b7433bf70a40306d1b9421720969297c3e2c10dcddc5807a8f9c1794f097` |

## Visual proof (actual packaged Unreal screenshots, 1280 × 720)
- `Docs/Evidence/Arena_v07.png`: 562,458 bytes, SHA-256 `c0912f44acd4d0adbff5005db25dc53633c8b57753563d3b00d89e5a8d0b3551`.
- `Docs/Evidence/Explore_v07.png`: 719,701 bytes, SHA-256 `34cbd0067279924b55b92ac29486643bdbc952f1d6863af6c256b79f715cc174`.
- Checked visually: third-person low-poly world with 30 enemy rigs; turn-based battle interface; player/enemy models; F2 accessibility prompt. Visual design remains basic prototype fidelity.

## Progression and player experience
- The game starts at level 100 with a 50-of-100 selected skill build.
- Skills are lost as the level declines to 0. Loot, equipment, forge and evolution preserve tactical choices.
- Turn-based attacks, resource points, timed Q parry/E dodge, nine miniboss definitions, and mirrored starter build.
- V07 F2 timing assists (normal/moderate/wide), readable six-skill mirror boss move cycle with original starter snapshot preserved.
- The 10 C++ tests validate deterministic mechanics and mathematical full descent, but do not substitute for a human full-run playtest.

## Evidence boundary / open items
1. **Human playthrough at all milestones, controller support, FPS and performance on low-end GPU remain unverified.**
2. Full animation blending, audiovisual polish, UX localization/contrast and accessibility certification are unfinished.
3. Historical Godot data comparison: five JSON files differ in byte-level representation but all five parse to **semantically identical JSON**; skills=100, mobs=30, minibosses=9. Byte-parity validator returned 27/32 PASS, but semantic audit passed 5/5.
4. Live workspace `E:\LastLightUnreal\LastLightUE` is simultaneously being developed as V08 and **must not be treated as identical to this clean V07 release**. The verified V07 source is frozen in `E:\LastLightUnreal\_v07_cleanbuild\LastLightUE` and this branch.
5. GitHub PR #1 was merged by a separate action during concurrent development. This V07 correction lives in **draft PR #2** and should be reviewed before merge; no force updates to `main`.

## Rollback
Verified earlier packaged V06: `E:\LastLightUnreal\Releases_v06\Windows\LastLightUE.exe`; 26/26 prior release checks PASS. No user SaveGames deleted.

## Reproduce in VS Code
Open the checked-out `unreal/LastLightUE` project, start a new Codex thread, and read `AGENTS.md`; run `Scripts/verify_sot30.ps1`, `Scripts/test_editor_v07.ps1`, `Scripts/build_windows_v07.ps1`, packaged smoke and release verifier. The complete SoT30 files are versioned at `Docs/IskraSoT30/knowledge`.
