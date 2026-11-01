# LAST LIGHT — Godot v0.6 → Unreal Engine 5.8.4

Target: Windows x64. Source: `%USERPROFILE%\Documents\LastLightGame\project`. New project: `E:\LastLightUnreal\LastLightUE`.

## Project state (truth ceiling)

This is an ENGINE MIGRATION, not a claim that existing GDScript executes in Unreal.
All gameplay now lives in original Unreal Engine **C++**. Original game data JSON is preserved in `Content/Data/` and six original `*.glb` character source files in `Migration/GodotAssets/`. The original Godot v0.6 Windows build remains unmodified in Documents as fallback.

An actual Windows release is DONE **only** when `LastLightUE.exe` plus cooked content are built, launched, runtime gameplay smoke checks pass, and a sha256 file inventory/receipt exists. A successful Unreal Editor DLL compile alone is *not* enough.

## Source-to-target mapping

| Godot v0.6 | Unreal Engine 5.8 | Status |
|---|---|---|
| `scripts/systems.gd` | `Source/LastLightUE/LLGameInstance.h/.cpp` | Reimplemented, requires parity tests |
| `scripts/world.gd` | `LLStage.cpp` + `LLHero.cpp` | Native procedural exploration rebuilt |
| `scenes/battle_arena.tscn` | `/Game/Maps/Arena` + `LLStage::BuildArena` | Independent Unreal .umap with its own lighting and actor visuals |
| `scenes/main.tscn` | `/Game/Maps/Explore` + `LLGameMode.cpp` | Unreal scene/map lifecycle |
| Godot Canvas UI | `LLHUD.cpp` | Native custom Canvas HUD; mouse-click target regions |
| Godot `move_and_slide` | `ALLHero::AddMovementInput`, `UCharacterMovementComponent` | UE native collision, spring-arm camera |
| Godot rig animator | `Migration/GodotAssets/*.glb`, Unreal Interchange importer | Imported assets are a separate gate; C++ proxies exist as fallback |
| `data/skills.json` | `Content/Data/skills.json` | Source data copied byte-for-byte, 100 skills including Russian names |
| `data/mobs.json` | `Content/Data/mobs.json` | Source data copied, 30 species |
| `data/minibosses.json` | `Content/Data/minibosses.json` | Source data copied, 9 minibosses |
| `data/starter_gear.json` | `Content/Data/starter_gear.json` | Source data copied, 3 starting items |
| `data/balance.json` | `Content/Data/balance.json` | Copied as evidence; initial runtime constants mirrored in C++ |
| Godot save format | Unreal JSON save `Saved/SaveGames/LastLightUE.json` | New format; original Godot saves are not modified or compatible |

## Core loop and controls

Initial menu: choose 50 of 100 skills, R randomize, TAB paging, Enter start. Exploration: WASD camera-relative movement, Shift sprint, Space jump, RMB orbit, wheel zoom. E engages the nearest enemy if close, F forces a normal encounter (test/fallback); B challenges unlocked miniboss, K only at level 0 triggers mirror. Five biomes along a 100-metre road, 30 visible encounter sites, optional raised paths.

Combat is a *different map asset*: `Explore.umap` → `Arena.umap` through `UGameplayStatics::OpenLevel`. `ULLGameInstance` persists HP, skill book, level, loot, equipped gear, minibosses and world position. Arena actors face one another; turns have 2 action points; offensive QTE Space 67% +25% damage; Enter normal attack; timed defensive Q/E at 63/72/81%. F ends turn; 1-7 skills, Tab pages. Enemy hits have real damage; guard/dodge/parry counter. Hero defeat can retry, victory yields loot/degeneration and returns to Explore.

Level loss: player's chosen skill removed, another random skill removed, a random skill awarded if 2 deletions possible, otherwise skips; demo default 5 losses/victory; 5 levels yields evolution slot; V fuses two skills, G fuses inventory items with grade risk cap 80%. Main boss at level 0 uses frozen snapshot of starting build for damage/HP/signature attack naming.

**Differences to fix later**: Godot 100-level geometry and tile positioning are not pixel-identical; animations, material fidelity and UX parity are separate gates. The current UE native proxy bodies are original primitives until Interchange skeletal imports are built into runtime. User gamepad, commercial QA, Steam release, performance across GPUs: not yet proved.

## Commands

Editor compile:

```powershell
& "C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" LastLightUEEditor Win64 Development -Project="E:\LastLightUnreal\LastLightUE\LastLightUE.uproject" -WaitMutex -NoHotReloadFromIDE
```

Create maps (Editor Python plug-in enabled in .uproject):

```powershell
& "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "E:\LastLightUnreal\LastLightUE\LastLightUE.uproject" -run=pythonscript -script="E:\LastLightUnreal\LastLightUE\Scripts\create_maps.py" -unattended -nosplash
```

Windows packaging per Epic documentation:

```powershell
& "C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -project="E:\LastLightUnreal\LastLightUE\LastLightUE.uproject" -platform=Win64 -clientconfig=Development -build -cook -allmaps -stage -pak -package -archive -archivedirectory="E:\LastLightUnreal\Releases" -nop4 -utf8output
```

## Sources

Epic Interchange (glb/FBX): https://dev.epicgames.com/documentation/unreal-engine/importing-assets-using-interchange-in-unreal-engine

Epic Python Editor scripting: https://dev.epicgames.com/documentation/unreal-engine/scripting-the-unreal-editor-using-python

Epic BuildCookRun: https://dev.epicgames.com/documentation/unreal-engine/build-operations-cooking-packaging-deploying-and-running-projects-in-unreal-engine

## Verification

Read `Reports` and `RELEASE_RECEIPT_UE.json` after actual compilation, maps, build/cook/package, smoke launch, and any asset import. `PASS` applies separately to source data, C++ compilation, maps, game runtime, Windows packaged release and art parity. No automatic conversion claim for .gd, .tscn or .blend.

ΔDΩΛ: Δ native Unreal architecture; D commands, logs and SHA receipts; Ω bounded to verified stages; Λ any errors in Editor/Unreal packaging, player control and real human testing.
