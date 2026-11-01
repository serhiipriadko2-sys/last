# LAST LIGHT: THE DESCENT

**Playable offline Windows browser prototype / version 0.2.0 / 2026-11-01.**

A low-poly perspective 3D turn-based RPG about reverse progression: start at level 100, forget your arts, adapt through items and fusion, reach level zero and face an enemy based on your original build. The 3D scene is generated from geometry in JavaScript, with procedural animations. No CDN, account, installation or internet access is required to play.

> IMPORTANT: This is NOT a packaged Unreal Engine project and does NOT contain a native Windows `.exe`. The authorized desktop did not respond to Remote Desktop Commander, so its `E:\LastLightUnreal`, installed Unreal Engine and VS Code could not be inspected. The game below is a verified fallback vertical slice; **native Unreal packaging is BLOCKED**, not claimed complete.

## Play on Windows

1. Extract the full ZIP, keeping the `js/` and `css/` directories next to `index.html`.
2. Double-click `PLAY_WINDOWS.bat` (or `index.html`). Microsoft Edge or Chrome opens the game locally.
3. Select **BEGIN DESCENT**. Select or randomize 15 abilities; click **ENTER THE FALL**.
4. Move toward the red enemy silhouette; press **E** to initiate a turn-based battle. Win battles to lose levels. Visit the golden chest at the right side with **E** to open the workshop.
5. At level 0, approach the stone portal and press **E** to fight your starting self.

If your browser restricts local scripts, run `SERVE_WINDOWS.bat` (requires Python installed) and visit `http://127.0.0.1:8765/`.

## Controls

| Exploration | Combat | Other |
| --- | --- | --- |
| W/A/S/D: move | 1: basic strike | H: heal during combat |
| E: interact | 2-5: use arts | G: workshop/inventory |
| G: workshop | SPACE: finish turn | ESC: pause/close workshop |
| | Q: timed dodge | TAB: next page of arts |
| | F: timed parry | Mouse: click action cards |

**Battle:** each turn gives two action points. During the enemy turn, read the telegraph and press Q/F as its moving marker crosses the highlighted section. Successful parrying counters; dodging restores resolve. Timing assistance can be enabled at the workshop.

## Systems in this build

- 130 data-driven arts: 100 archetype-by-element arts and 30 authored named variants. A searchable, filterable build selection screen.
- 30 enemy variants, 9 tier-specific minibosses at levels 90 to 10, and one mirror final boss using the initial build snapshot for its attack choices and scaling.
- Reverse progression: for each eligible level loss, the player chooses one art to remove, fate chooses another, and a random art is granted. With fewer than two innate arts, no removal or grant occurs. The intrinsic collection can never become empty through these rules alone.
- Default accelerated demo: five level losses per battle, so a complete 100-to-0 run takes 20 battle victories. This is for testing the full arc, not final game pacing. Switch to **NORMAL CURVE** in the workshop for slower XP-based progression.
- Item drops, equipment slots, equipment-granted abilities, random forge failures, grade 1-6 upgrades, fused art chains, healing, saves in browser local storage, death/recovery and victory screen.
- Animated procedural figures, perspective 3D environment, enemy telegraphs, camera change for combat, atmospheric lighting and basic hit feedback. Art is placeholder quality.

## Repository layout

- `index.html`, `css/style.css` - interface and overlays
- `js/data.js` - game data, difficulty and progression curves
- `js/renderer.js` - offline perspective 3D renderer and procedural geometry
- `js/game.js` - UI, battle state machine, progression, loot, forge, save/load
- `tests/data.test.js` - deterministic registry and math tests
- `tests/browser_smoke.py` - end-to-end game flow using browser automation
- `knowledge/00_...29_...` - full SoT30 project corpus (not game assets)
- `support/MANIFEST.json` - canonical corpus hashes
- `AGENTS.md` - Codex entrypoint for the full Iskra/SoT30 protocol
- `docs/` - architecture, design, risks, acceptance and lifecycle
- `tools/verify_sot30.py` - hash/size validator of 30 SoT files

## Automated validation

Run `node tests/data.test.js` for registry invariants, and `python tests/browser_smoke.py` for the 100->0->mirror flow if Playwright and Chromium are available. The browser test constructs the UI in a sandbox page and injects the local scripts; this checks runtime logic but is not identical to opening a local file in the target Windows machine. The verification report in `docs/VERIFICATION.md` marks each boundary.

## Native Unreal next gate

Once Remote Desktop Commander responds, read and back up `E:\LastLightUnreal`, detect the installed UE version and Visual Studio toolchain, then port the verified state machine into the UE gameplay framework (GameMode, PlayerController, stateful Actor components, UMG widgets) and implement native physics/camera/animation/assets. Package using the engine's `RunUAT.bat BuildCookRun`, launch the resulting `.exe`, complete a real playthrough, and record logs and SHA-256 receipts. **Do not claim those steps occurred before running them.**

License: project source is original experimental prototype code. No third-party game-engine libraries or proprietary assets are embedded in the package. The bundled SoT30 corpus retains its own provenance and is not game code.
