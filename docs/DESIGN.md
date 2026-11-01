# Design specification: LAST LIGHT v0.2

## Core thesis

In a conventional RPG, progression means acquiring power. In LAST LIGHT the player starts almost omnipotent and learns to preserve agency while sacrificing power. The mirror enemy is a record of the player's starting choices, not a random final encounter. The playable question is whether adaptation compensates for decline.

## Status and engine decision

This is a **fallback offline Windows browser runtime**, not a native Unreal game. The developer's Windows device was listed by the connector but all attempted directory/config reads failed with `Timeout - no response from device`. The GitHub repository was initially empty. Building or modifying unknown local files would have been unjustified. The self-contained perspective renderer allows immediate logic tests without any external runtime; this deliberately trades Unreal visuals and native EXE distribution for verifiable functionality. The native port remains open work.

The original proposal preferred Godot 4.7.2 (which was the stable version on 2026-11-01). The latest request references an Unreal installation and `E:\LastLightUnreal`; **no project-engine change has been applied to that unknown local project**.

## Vertical-slice loop

`Title -> choose 15 intrinsic arts -> 3D courtyard -> enemy encounter -> turn-based AP combat + active defense -> loot -> decline (1..5 levels) -> workshop/equipment/evolution -> repeat -> level 0 portal -> mirror boss -> win/lose`.

Player commands can be chosen by click or key. Two AP each turn. The enemy phase produces a patterned telegraph (slash, stab, crush, ignition, cross cut). A dodge/parry must coincide with the pattern's timing window. Assist mode extends the window. Misses damage vitality; counterattacks reward timing without making them mandatory.

## Data-driven systems

`js/data.js` exports the registry and all changeable constants. The 100 generated arts are the product of 10 elements and 10 action archetypes. Another 30 named variants offer baseline variety. Elements have different status tags, colors and weaknesses; all types share a small explicit effect grammar so 4,950+ hand-authored combination animations are not required.

Enemies = six behavioral families x five traits (30 definitions). They differ by base stats, telegraph patterns, weaknesses, element, appearance family and speed. These are variants, **not thirty independently authored animation trees**. Nine minibosses have tier thresholds 90, 80, ..., 10; the final boss uses the starter art list to pick recognizable attacks and initial average power to scale HP. This is an early model of a mirror, not a full replay of player decision history.

## Reverse progression math

Normal XP threshold: `XP_to_lose(L) = 3000 * 0.96^(100-L)` (clamped to at least one XP), with nominal reward 100 per battle. This means approximately 30 fights at level 100 vs roughly 1-2 near level zero, and about 737 equal-reward encounter equivalents for a full run before rounding. This long-form setting **has not been playtested**. The default *fast demonstration mode* bypasses this curve to apply exactly five losses per win, allowing 20 wins to reach level zero. Set `fastLevelsPerWin` in `js/data.js` to adjust.

At each loss when at least two intrinsic arts exist: player chooses one removal; a second distinct remaining art is removed at random; one random art is added. This decreases positive inventory by exactly one. When only one intrinsic art remains, removal and grant are skipped. Thus 15 starting arts reduce to one after 14 eligible losses; the remaining 86 levels do not deplete the final art. Fusing two intrinsic arts consumes one more art; it never creates an empty collection. **The originally proposed empty-build state is unreachable under these operations**. Basic steel attacks remain available independently of any arts; item-granted arts remain available.

Every five levels lost yields one evolution charge, for 20 possible charges over the run. Only charges with two available innate arts can be used to fuse. Under the original loss operation, many will remain unused; this is a deliberately exposed balance issue, not hidden by a fabricated resource sink.

## Enemy difficulty and accessibility

Scaling is capped: enemy health factor `1 + 0.66 * lost_fraction`, enemy damage factor `1 + 0.65 * lost_fraction`, defense-window factor `1 - 0.38 * lost_fraction`. The incoming hit is capped to 32 damage. Max player health declines from 125 to 90 (a minimum cap of 78). Assist expands timing windows. Actual enemy action count is capped at two per turn. These caps avoid multiplying every difficulty penalty without limit.

## Equipment and fusion

Kill -> generate one common/rare/relic drop. Items supply 1-3 arts, which are active when worn. Two items of the same equipment slot can be fused in the workshop. `P(break | source grade g) = min(0.80, 0.10+0.12*(g-1))`, grades 1-6. A failure destroys one source randomly and keeps the other; a success consumes the donor and upgrades the survivor. Grade-5 source to grade-6 has 58% break risk; grade-6 items cannot fuse. Sealed relic arts activate when successfully fused.

## Visual and narrative direction

A ruined courtyard surrounded by monolithic ash sculptures and torches; the horizon evokes late-day light disappearing. The player is always a recognizable silhouette. Objects are 3D mathematical polygons drawn with a perspective camera, not 2D sprite billboards; procedural walk/sword/idle poses and a cinematic battle camera convey movement. No third-party art pack or animation library is bundled. This is a prototype-quality visual benchmark only.

## Save/restore

After battles and workshop exits, a versioned save is written to browser `localStorage` under `lastlight.v02.save`. Battle state is intentionally not restored: load resumes in the courtyard, which prevents accidentally reopening a telegraph mid-fight. Schema migration, backup/export, save corruption UX and multi-slot saves remain future work.

## Next scientific experiment

With a verified native Windows build, recruit 5-8 players and measure: total play time; effective learning after each loss; voluntary use of fuse systems; frequency of timing failures; degree to which players recognize their own starter arts in the mirror encounter. The falsifier is straightforward: if players experience the second half as repeating the same optimal action and no meaningful choice, the central loop does not meet the design goal, irrespective of code completeness.
