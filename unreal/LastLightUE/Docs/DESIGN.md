# LAST LIGHT: DESCENT — Game Design & Engineering Decisions
Edition: v0.7 development (2026-11-01). [FACT] Current playable baseline is Unreal Engine 5.8.4 v0.6, not a finished commercial game.

## Pillar: decline as authored choice
Start at level 100 with 50 selected skills from 100. Battles give loot but accelerate the irreversible fall. At level 0 the player fights an image of the starting self. A player should survive by changing tactics and forging items, not by grinding infinitely.

## Engine decision
The original design brief allowed Godot 4 or Unity; an already working Unreal Engine 5.8 native Windows migration v0.6 now exists in this folder. Preserve it instead of restarting. This is a **game production decision**, not a change to Iskra's canon. Original Godot source in Documents/LastLightGame remains intact.

## Existing vertical slice
- World: Explore map with 5 zones, 30 enemy placements; transition to separate Arena map.
- Data: Content/Data/skills.json (100), mobs.json (30), minibosses.json (9), starter_gear.json and balance.json.
- Character art: original Blender joined skeletal meshes for hero, knight, wolf, wasp, golem and wraith; seven imported animation clips per mesh. 30 enemy records currently share six visual families.
- Player: WASD, orbit camera, jump/sprint, equipment, miniboss gates, inventory, forge, evolution and saved run.
- Combat: 2 AP per turn; timed attack, Q parry, E dodge; weakness/status effects; enemy multiattack escalates in the final levels. Final boss HP/damage depends on frozen 50-skill starter snapshot.
- Win/loss: 20 accelerated wins with five level losses each form a practical demo route, with a manual skill-loss choice after each loss; saved pending losses resume after load.

## Progression math and proven limitation
Production-style XP_to_lose(L) = 300 * 0.96^(100-L). In demo mode each victory causes min(5,L) losses regardless of XP, for a 20-encounter full descent. These values are currently coded in C++ even though balance.json exists. **Do not claim every balance parameter is already live-editable**; moving all values into validated runtime config is a separate improvement.
- Rule: if innate count >=2, remove selected skill, then one random skill, then add one random base skill. Net -1. With one skill, skip removals and grant, so the last skill persists.
- With 50 innate skills, no evolution, after 49 losses the count is 1; losses 50–100 do not remove it. An ordinary empty innate book is unreachable from this starting state.
- Every fifth level lost grants an evolution slot, totalling 20 from 95 through 0. Fusion of two innate skills into one uses one slot and reduces count by one; slots are optional and may remain unused. Multiple fusions can accelerate the approach to the last skill, never drop it to zero through standard loss.
- Equipment-granted skills persist as compensation and do not count as innate. Two starter pieces may be merged, with risk preview; successful result removes both and adds one upgraded item. Probability: min(0.80,0.10+0.12*(max grade-1)); max grade 6 cannot merge.

## V07 decisions
1. Accessible timing: F2 cycles 0 / 0.08 / 0.16 seconds additive leniency for defense; offensive timing gets a smaller proportional extension. Gameplay state is saved in the optional timing_assist field; loading v0.6 saves with the field absent defaults to zero. Players can keep strict original difficulty.
2. Final mirror: preserve all 50 starting skills, open with the player's first selected skill, then use a deterministic six-skill strongest repertoire ordered by power and ID. Attack names continue to identify source skills; skill power offsets individual hit multipliers around mean power. This addresses the audit's unreadable 50-move attack-cycle without erasing the snapshot.
3. The V06 version is the immutable rollback. V07 output goes into Releases_v07 outside Git and must pass new native tests plus packaged smoke.

## External GitHub technology evaluation
No dependency is copied merely because it exists on a ten-item list. Use evidence and compatibility gates:
- Jolt Physics: separate physics stack; Unreal already has Chaos. A replacement increases risk more than a vertical slice benefits.
- Terrain3D and Godot Voxel Tools: **Godot add-ons**, not plugins for an Unreal .uproject. Terrain prototyping should use Unreal Landscape/PCG first.
- Material Maker: possible offline texture authoring workflow; exported textures require provenance/license check.
- Effekseer: separate VFX runtime; prefer native Unreal Niagara for the existing build until a compatible plugin and license are tested.
- Recast Navigation: Unreal navigation uses Recast concepts; test native navigation first rather than importing its source.
- ozz-animation: independent animation runtime; imported UE skeletal rigs are better validated with Animation Blueprints before replacing them.
- The Forge: rendering framework not intended as a UE game plugin; no reason to replace UE's renderer.
- meshoptimizer: candidate offline mesh preprocessing if profiling reveals geometry bottlenecks; not a gameplay engine.
- FastNoise2: candidate offline procedural terrain/noise generator when world streaming is in scope; not needed for fixed five-zone prototype.

## Threats to game quality and falsifiers
- Risk: the last 51 level losses do not remove innate skills. Falsifier: compare perceived variety of encounters late in a human playtest and measure player exits; redesign progression only with design decision and tests.
- Risk: turn combat may be repetitive; target 3–5 distinct enemy telegraph families per zone and explicit safe tells.
- Risk: novice players fail strict parry timing; F2 opt-in must be tested by humans, not inferred from C++ formulas.
- Risk: six visual bases across 30 enemy types reduce readability; add unique silhouette/telegraph treatment after measurements.
- Risk: direct-play animation has no blend trees; further work on Animation Blueprint, locomotion transitions and hit reactions.
- Risk: visuals/audio/FPS/controller/save recovery not yet certified on multiple PCs. These remain open gates.
- Risk: user progress data. Never overwrite real SaveGames for tests; null-RHI smoke uses special flag and a separate test lifecycle.

## Art direction target (proposal, not completed)
A stylized fading world: ash garden → ice glass → forgotten temple → eternal sea → mirror citadel. Strong enemy silhouettes, readable windup colors, differentiated biome lighting and restrained particles. Every added asset must be reproducible and licensed.

## Done boundary
A release requires source compiled, UE native tests PASS, UAT Cook/Stage/Package PASS, packaged EXE Explore→Arena PASS and a hash receipt; only a human full run can certify fun, usability, control and complete experiential quality.
