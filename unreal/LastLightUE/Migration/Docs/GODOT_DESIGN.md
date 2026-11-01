# LAST LIGHT: DESCENT — Game Design Document (GDD)
Version: 0.4.0 · 2026-11-01 · status: runnable Windows research prototype with visual wayfinding overhaul, NOT commercial production release.

## Research question
Can inverse RPG progression produce agency, mastery and emotional attachment when the hero is objectively weaker in each subsequent chapter? The hypothesis (H1) is that controlled loss can be satisfying IF equipment substitution, system mastery, meaningful selections and an intelligible mirror finale compensate for numerical decay. Null H0: repeated loss overwhelms discovery and becomes frustrating monotonous attrition. This project is an experimental playable instrument, not evidence that H1 is true.

## Comparable works: what is and is not new
SINNER: Sacrifice for Redemption (2018) sacrifices stats before boss fights; its Steam entry explicitly advertises level-down play (https://store.steampowered.com/app/691280/SINNER_Sacrifice_for_Redemption/).
Clair Obscur: Expedition 33 uses reactive turn-based combat with real-time dodge/parry/counter. The developers describe the difficulty of tuning timings (https://www.unrealengine.com/developer-interviews/inside-the-development-journey-of-clair-obscur-expedition-33).
Therefore neither level-down nor turn-based-parry is an unprecedented technique. The research contribution is their explicit combination with a persistent immutable snapshot of the player's level-100 identity and gear-driven adaptive rebuilding. Originality of the complete combination has NOT been exhaustively proved.

## Vision and gameplay loop
The hero wakes with 100 levels, 50 of 100 obtainable innate skills, three equipped relics, and a choice: accelerate toward loss or avoid fights. Five low-poly ruin biomes form a contiguous floating road: Ember Garden; Glass Fields; Broken Temple; Silent Sea; Mirror Citadel. Thirty enemy archetypes populate the world; elite and two-species symbiotic enemy encounters can be selected; nine bosses unlock at 90/80/.../10. At 0 a former self returns using frozen starting signatures.
Core loop: explore / inspect enemies → choose cost of an encounter → 2-AP tactical turn → incoming telegraph + real-time Q-parry/E-dodge → win loot and XP → trigger level-loss packet(s) → sacrifice skill / accept fate / receive new skill → merge gear or innate skills → adapt.

## What actually ships in v0.4
Godot Engine 4.7.2 GDScript with GL Compatibility rendering, original code-generated 3D block geometry and 6 original synthesized SFX; 100 skills (30 named signature and 70 formula-derived), 30 mob data archetypes, 9 checkpoint minibosses, two-category elite/symbiote encounters, one mirror boss, five color-coded world zones, 50-of-100 searchable start picker, random build, innate/gear combat skill management, mutable gear slots and inventory, skill evolutions, risky forging, level-down, fast testing and full geometric modes, JSON save/load, accessibility timing assist.
The 3D character animation is procedural bobbing, turning, imitation steps, fading hero halo, windup scaling, impact/flash and cinematic camera change. Scene 0.3 adds toon materials, improved silhouettes, per-zone landmark meshes, gold arrows on walkable path, proximity-only readable world labels, a permanent 5-zone HUD compass, a full map, a Russian bestiary and localised action buttons. It is NOT rigged skeletal combat animation or authored motion capture. Base enemy archetypes have distinct silhouettes/stats/attack labels but NOT thirty individually authored behavior trees; the mini bosses share much of the turn loop. Cover, elevation tactics, dungeon navigation obstacles, cinematic narrative and authored cutscenes are design targets NOT implemented.

## Why Godot instead of installed Unreal Engine 5.8
Observed Windows host has UE 5.8, Blender, Python 3.14 and Git. Godot 4.7.2 can build a versioned cross-platform prototype fast using simple GDScript, procedural Geometry and JSON without expensive asset pipeline. Unreal is suitable for final cinematics but its packaging and shader/content pipeline add overhead before core inverse progression is validated. Godot 4.7.2 stable verified on 2026-11-01: https://godotengine.org/article/maintenance-release-godot-4-7-2/. Scope choice is reversible after playtests, but porting should not happen before prototype evidence.

## Exact inverse experience curve
For L in 1..100,
    XP_to_lose(L) = BASE * K^(100-L), default BASE=300, K=0.96.
For normal encounters XP award 10, elite 40. At level 100 loss threshold is 300 XP (about 30 basic enemies). At level 1 threshold ~300*0.96^99 ~=5.3 XP (one enemy). Full-sum discrete XP requirements without carry/rounding:
    SUM_{i=0}^{99} 300*0.96^i = 300*(1-0.96^100)/(1-0.96) ~= 7373 XP units, or roughly 738 basic-enemy equivalents. An independent in-engine normal-mode test confirmed 738 ordinary 10XP encounter awards to reach level 0 (including fractional XP carry); this is not a human playtime measurement.
In 'TEST CAMPAIGN ON' mode each victory instead queues exactly min(5,L) level-loss packets, so 20 victories cause exactly 100 losses. This is an explicit test-shortcut, not the intended pacing. One can toggle test mode at the start menu; the normal run may be very long. Each first victory adds a choice between manual and automatic sacrifice packets.

## Skill attrition mathematics and zero-skill contradiction
Innate-skill count N does NOT equal the global pool of 100. At every loss with N>=2, two distinct innate instances are removed and one new is granted from pool not currently held: net N->N-1. At N<2 the entire removal/grant packet is skipped; only level decreases. Hence without additional deletion, starting with 50 innate skills reaches N=1 after 49 loss packets, at L=51; with 15 it takes 14 losses, at L=86. Zero innate skills is NOT normally reachable from a positive start by the specified rules; although the engine supports baseline actions even with an empty innate list. This is a requirement contradiction rather than a fabricated zero.
Every lost multiple-of-five level (95,90,...0) grants one persistent evolution slot: 20 slots. Fusion consumes two innate skill nodes and returns one nested composite skill; it reduces N by an additional 1; removing composite deletes the entire graph as one indivisible skill. A naive strategy trying to fuse on the first available checkpoint will run out of eligible innate skills after approximately 2 fusions with 15 initial; 8 with 50; 16 with 100. More slots can stay unused indefinitely. The max nested depth is 4, controlled via balance.json; no cyclic skill graph is possible because fusion constructs new nodes only. Remaining evolution slots do not expire. The alternative 'spend spare slots to upgrade gear' remains a proposal (not code).

## Two skill pools and equipment
INNATE (starting 50): the only pool affected by level loss or evolution.
GEAR: three slots (weapon, armor, charm), each holds 1-3 skills in starting gear or loot; these skills do NOT count toward innate attrition, and the item remains unless forging breaks it. Some non-common loot seals its last granted skill until an item fusion succeeds. Battle also always exposes basic strike and guard.
In v0.2 all skill IDs are drawn from one shared pool of 100; level loss preferentially excludes current innate duplicates but gear and innate may share the same ID. Fully random loot is seeded via runtime RNG. Inventory cap 64.

## Item fusion exact contract
Risk p(g) = min(0.8,0.1+0.12*(g-1)), g in 1..5. Thus 10%,22%,34%,46%,58% on attempts at grades 1..5. Success replaces BOTH inputs with grade(max(g1,g2)+1), carries a union of at most 3 unsealed skills and optional slot inheritance from the first item. Failure removes exactly one random input. The player sees the chance and must confirm; maximum grade 6. Grade6 cannot be used as an input. The mathematical 70% chance at grade6 is thus unreachable and must NOT be advertised. An intentional first-item slot inheritance is the current rule; slot-safe subtype constraints should be reviewed after testing.

## Combat model
Player moves freely in simplified 3D exploration with WASD, chooses encounter with E or via menu; camera interpolates to over-the-shoulder. Turn: 2 AP, any active skill consumes 1; guard reserves next damage reduction. Enemy turn: 1 attack at high levels, 2 below45 and 3 below12; mirror has 2 then 3 at late phase. Each enemy strike has a telegraphed 1.6-second reaction phase (visual progress bar, strike at 72%); Q is strict parry with counter + Focus, E dodge is wider and grants Focus. Default full timing window (total): parry 0.27s at L100 falling to 0.17s at0; dodge 0.38s falling to 0.26s. Optional accessibility switch adds 0.14s to the window; failure causes direct damage and possibly status. Being able to defeat foes by skilled defense is intentional. Fire+frozen, lightning+wet, wind+burn and specific elemental weaknesses multiply tactical damage. Four status mechanics use timers.
Enemy damage base scales from 1.0 at L100 to1.6 at0 before effects. Hero base HP drops 120->70. Exact HUD and balance are configurable.

## Final mirror contract
At start, snapshot copies ALL chosen skills, starting gear and top 7 powerful signatures plus gear references. Snapshot is immutable during play and saved. The final mirror is unlocked only at level 0; its HP = round(120 + mean_start_skill_power*5). Its base damage = round(7+mean_start_skill_power*.8); each named attack cycles a frozen skill's own name, element, power, kind and status and is phase-modified. This avoids requiring 50 totally unique hand-keyframed moves while remaining dependent on the actual original build. Boss phase triggers at HP < 68% and < 33%. Full 50-skill tactical AI isn't shipped; that is a non-claim.

## Reproducible tests and limits
- tests/systems_test.gd checks data, 50-skill setup, loss packets, attrition, fusion, loot, item forging, parry, save/load, mirror.
- tests/world_test.gd checks 3D scene, five zones and 30 model actors.
- tests/gameflow_test.gd crosses all menus and 100 losses through 20 wins by forcing enemy HP to 1 for integration only. This proves state connectivity, NOT challenge balance.
- tests/normal_curve_test.gd verifies full normal mode: 738 ordinary 10XP combat rewards, all 100 loss packets, 20 evolution opportunities, and lawful one-innate floor.
- tests/balance_test.gd solves 20 real-HP battles with computer-perfect parry and valid attacks, all checkpoints and final mirror; tests mirror scaling across distinct start builds, and N=1 floor with 15-skill start. It is an agent simulation, NOT a human usability study.
- Windows binary launch produced live desktop window, proper 3D world and start-picker GUI, with no process crash. Final visual QA: eight deterministic 1600x900 Godot movie-maker captures (five biome scenes, combat, map, bestiary); see previews/.
- tests/navigation_test.gd validates all five zone transitions, 30 model placements, >=13 distinct base shape identities, proximity interaction, exclusive readable nearest-enemy label, map and bestiary opening. Mouse-mediated human hand playthrough still pending.
- A normal 100→0 campaign without demo mode has not been completed by a human and could be prohibitively long.

## Falsifiable research plan
Recruit at least 6 naive external players (not aware of implementation). Have three play fast mode then normal sample; three reverse order. Record losses per encounter, time per 10 levels, baseline attacks versus innate/gear attacks, player-chosen deletion regret, perceived agency on Likert 1-7, frustration 1-7, and whether mirror recognizes original build. Success criteria prior to scaling: >=4/6 understand how level loss works; >=4/6 choose gear adaptation intentionally; median perceived agency >=4/7; <=2/6 describe the last 30 levels as 'nothing new'; >=4/6 recognize a signature in boss moves. Failing criteria require changing loop/skills or encounter pacing BEFORE producing final 100 bespoke animated skills.
The test must log time and deaths without personal data and seek consent for any local telemetry; no network analytics exist in v0.2.

## Source references / review date
- Godot 4.7.2 stable: https://godotengine.org/article/maintenance-release-godot-4-7-2/
- Godot Windows exporting: https://docs.godotengine.org/en/stable/tutorials/export/exporting_for_windows.html
- Godot export package distinction: https://docs.godotengine.org/en/4.7/tutorials/export/exporting_projects.html
- Godot MIT obligations: https://godotengine.org/license/
- Sinner level-down precedent: https://store.steampowered.com/app/691280/SINNER_Sacrifice_for_Redemption/
- Expedition 33 developer interview: https://www.unrealengine.com/developer-interviews/inside-the-development-journey-of-clair-obscur-expedition-33
Current external facts reviewed 2026-11-01. Historical product claims are used as context, not proof of game uniqueness.

## v0.4 implementation decision / position, navigation and motion
Six unique Blender-generated rigged archetypes (hero, knight, wolf, golem, wasp, wraith) replace basic procedural shapes for suitable enemy families. Godot Skeleton3D/AnimationPlayer imports 7 clips each, proven to move bone pose in runtime tests. Uncovered shapes keep their procedural 3D actors. Editable Blend in parent blender_sources/v0.4, runtime glTF in project/assets/characters. Source generator tools/build_rigged_assets.py.

3D exploration hero root is CharacterBody3D with CapsuleShape3D. WASD relative to camera azimuth (forward=-sin,-cos; right=cos,-sin), diagonal normalization; shift sprint, Space jump, RMB orbit over 3D, wheel zoom, R camera reset. _physics_process calls move_and_slide. A five-zone raised side route network offers actual colliders and two access ramps each. Seam-stabilization step-up code guided_ramp_y works on specific ramp areas while collisions continue via the Godot physics world. Ten stone cover bodies prevent walk-through. Terrain buffs read positional state at battle entry: elevated +15% outgoing damage; covered one incoming failed defense reduced by 35% once. This is no dynamic battle-positioning system. Interaction E now rejects unreachable mobs with vertical gap >1.45 m.

Tests: rig_import_test.gd, rig_motion_test.gd, traversal_test.gd, input_route_test.gd, and all legacy tests. Godot Movie Maker captures nine 1600x900 stage screenshots in previews, including upper bypass. Pre-existing normal-mode pacing 738 baseline reward equivalents still remains a risk. Human assessment of orbital mouse controls and character readability not yet performed. Reference ANIMATION_TRAVERSAL_V04.md.

## v0.5 architecture — independent combat scenes, reactive offensive timing
Combat is created through a PackedScene (scenes/battle_arena.tscn), not by summoning an opponent inside the exploration scene. Map is preserved and hidden during encounter, position frozen, own Camera3D and per-biome Environment with physical 3D stage and original hero/enemy meshes, then the battle instance is freed on victory/defeat/cancel. Dynamic stage instances are spatially located at y=120 to prevent occlusion by the map. New combat_arena uses same six Blender-skin rig archetypes and procedural fallback actor animations.

The player chooses two skills per round. Offensive strikes/dash/curse launch timed QTE (Space at 67%, +25% true skill damage, Enter/timeout normal hit), while guard/heal act immediately. Three learned deterministic enemy telegraph rhythms (63%, 72%, 81%) have differing attack durations and damage multipliers; Q parry has narrower window + counter than E dodge. HUD ProgressBar gold target marker changes with attack rhythm; animation windup syncs to target. Start-of-battle cover/height bonus remains. Technical tests: tests/battle_arena_test.gd 78 checks; tests/combat_rules_extended_test.gd 218 checks; standard 100→0 gameflow updated. More details in COMBAT_ARENA_V05.md; manual human player tests still required.

## v0.6 — Orientation and camera/UI repair, stable repository-derived upgrades

The source of the 180-degree facing regression was GLB imported visual forward +Z versus Godot actor/gameplay forward -Z. The skinned visual in scripts/rig_actor.gd rotates PI around Y, and the non-skinned procedural geometries have a visual facing child rotated PI/2 for shapes originally drawn along +X. World movement uses unchanged physical CharacterBody3D; default camera yaw=-PI/2, so W now follows the primary +X road. The independent battle arena calls look_at on both character roots, facing -Z toward each other regardless of spawn/biome. Do not reintroduce direct root-motion rotation on the physics body.

Presentation now prioritizes objective, health, action points and explicit timing prompts. World advanced actions are collapsed; H toggles full controls; combat actions/skills filtered and paged, with sticky end-turn outside skill list. Existing 100 skill IDs preserved; name_ru is presentation-only and is used in UI/logs. Combat+HUD tests cover 5 arenas, 12 avatar angles, 100 names and key controls.

Practical approaches from 10 GitHub repos were reviewed. Native Jolt Physics, 10 FastNoiseLite cliffmeshes, 380 MultiMeshInstance3D props, native material settings, CPUParticles3D, weapon rings and camera animation are integrated. Terrain3D, Effekseer, godot_voxel, Recast, ozz, The Forge, gltfpack/meshoptimizer and FastNoise2 C++ plugins were **not** installed. See REPOSITORY_INTEGRATION_V06.md for exact eligibility, licenses and risks. This is a verified interactive Windows prototype, not AAA or full open world. User-playtesting and controller/mobile/accessibility QA remain.
