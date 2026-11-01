# Unreal Engine native Windows port: gated plan, not a completion claim

## Surface status

- `E:\LastLightUnreal` was provided by the owner as the expected working directory.
- Remote Desktop Commander device `DESKTOP-ITUAMNF` was shown as online but was not answering file/config calls; both returned `Timeout - no response from device` on 2026-11-01.
- Repository `serhiipriadko2-sys/last` was initially empty.
- No `UnrealEditor.exe`, `.uproject`, Visual Studio SDK/toolchain, VS Code session or Windows packaged `.exe` was observed in this turn.

## Phase gates

**G0 / inspect**: reconnect authorized desktop, read directory without mutation, `git status`, `.uproject`, UE version, UAT path, VS compiler, disk free. Create a backup or branch before editing. STOP on unrelated user files, missing permissions or secret exposure.

**G1 / deterministic logic**: port `js/data.js` into DataTable/DataAsset or JSON schema, preserve exact game invariants; unit-test XP thresholds, item break probability, removal of 2 and grant 1, atomic fused nodes, snapshot integrity, 9 miniboss thresholds. Set up Unreal Automation Tests for rules, compile Editor target.

**G2 / 3D slice**: use UE Character + Enhanced Input, GameMode/PlayerController, UMG widgets, CombatComponent with AP turn machine, timed defense, item/inventory components, save game. Use engine built-ins: Chaos Physics, NavMesh, Niagara, Animation Blueprints. Make one complete playable arena, one normal mob, one mini and mirror before importing hundreds of assets.

**G3 / content and polish**: author approximately 30 distinct enemy behaviors beyond registry variants, procedural animation state graphs, character rigging/retargeting with permitted assets, distinct elemental FX, camera transitions, positional combat, multiple locations, accessibility, SFX and optimization. Test no-parry victory path.

**G4 / packaging**: set default game map, Windows Development target and staged data files; use installed `Engine/Build/BatchFiles/RunUAT.bat BuildCookRun`, capturing log and exit status. Explicitly run the resulting `.exe` and smoke test selection -> combat -> 100 to 0 -> boss -> victory -> restart -> save/load. Hash all release files and produce a receipt. No `[DONE]` without actual Win64 exe bytes and launch evidence.

See [Epic's packaging documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/packaging-your-project) and [UAT build operations](https://dev.epicgames.com/documentation/unreal-engine/build-operations-cooking-packaging-deploying-and-running-projects-in-unreal-engine) for current native release procedures.

## Stop / rollback conditions

Any native port must not overwrite the existing remote `E:\LastLightUnreal` blindly. Keep fallback browser prototype reproducible. All game-specific architecture decisions go in `docs/DESIGN.md`, while a change to Iskra SoT30 or Codex permission policy requires a separate owner-accepted governance ADR. A successful compile is not equivalent to a playable executable; successful packaging is not a complete playthrough.
