# Engineering and experience risk audit (2026-11-01)

The claims here are game-design risk hypotheses until player testing; technical findings are labeled by existing code/test evidence. Each risk has a falsifier, mitigation, owner and rollback.

| ID | Severity | Risk and load-bearing premise | Evidence / test to falsify | Owner / bounded mitigation / stop condition |
| --- | --- | --- | --- | --- |
| R01 | CRITICAL | The normal `3000 * .96^(100-L)` curve creates about 737 combat-equivalent victories. Novelty may expire far before the ending. | Capture median playtime and distinct decisions over ten full runs; fewer than 20% meaningful choices in second half falsifies pacing thesis. | Design lead: keep fast-demo setting until observed balancing; revise XP curve rather than pad the world. |
| R02 | CRITICAL | 15 starter skills with net -1 stop at a single intrinsic skill by level 86. Most of the 100-level collapse cannot remove more skills. | Test count after 14 losses and at level 0; inspect player survey about late progression. | Systems lead: preserve basic attack/equipment abilities; introduce other cost axes only after prototype evidence and a written ADR. |
| R03 | HIGH | Combining weakness, reduced max health and smaller parry windows can make combat unfair, especially with accessibility needs. | Run no-parry victories across late enemies; compare assist toggled and default; identify impossible attack loops. | Combat lead: limit damage and actions, allow Q dodge and heal, retain assist; reject build if a sequence requires perfect parries. |
| R04 | HIGH | Mirror could feel random rather than personally recognizable. | Log each mirror attack identifier, compare it to immutable initial snapshot, conduct player recognition test. | Narrative/systems lead: use starting arts for boss telegraphs and scale HP from starting power; add authored pattern constraints in native port. |
| R05 | HIGH | Save corruption, browser storage clearing or schema changes can erase hours of meaningful losses. | Inject malformed save; verify safe refusal without overriding it; multi-session load test. | Persistence lead: current save is prototype-only; native version needs slot backups, atomic writes and schema migration before release. |
| R06 | HIGH | Thirty generated enemy variants might be marketed as thirty individually unique opponents. | Inspect registry and family/pattern grammar; enumerate actual unique telegraphs/animations. | Production lead: describe honestly as 30 variants, 5 base telegraphs, 6 families; require new animation trees in content milestone. |
| R07 | HIGH | Borrowing ten third-party projects without compatibility review can create ABI, licensing and maintenance failures. | Check exact release/engine compatibility and licenses in each upstream repo before importing a line. | Tech lead: zero third-party runtime dependencies in this build. Treat list as research candidates only. |
| R08 | CRITICAL | Windows Unreal build readiness is unknown because Remote Desktop Commander host timed out. | Read `E:\LastLightUnreal`, identify `.uproject`, editor version, build toolchain, then run UAT + executable. | Release lead: BLOCK native status; never claim launch, VS Code visibility or Windows exe without host receipt. |
| R09 | MEDIUM | Browser injection tests are not equivalent to production Windows offline deployment. | On actual Windows Edge open `PLAY_WINDOWS.bat`, inspect developer console, verify save, input and game completion. | QA lead: classify local test as PARTIAL for distribution; preserve file-launch fallback and server launcher. |
| R10 | MEDIUM | Player with one innate art accumulates unused evolution charges; rewards may feel broken. | Count unused charges in ten late-game runs, inspect forge use and player interviews. | Design lead: consider charge-to-gear exchanges only after a tested economy change; don't pretend this is already implemented. |

## Failure modes for research comparison

- **Sinner: Sacrifice for Redemption** explored losing strength before bosses; reverse progression is not novel by itself.
- **Hero must die. again** also begins with a powerful character who declines.
- **Clair Obscur: Expedition 33** demonstrates turn-based combat with timing-based defense; its visual and combat content is not copied.

The distinctive hypothesis to test is the combination of *chosen loss + item-mediated adaptation + immutable mirror snapshot*. The strongest counter-hypothesis is that repeated losses become chores and gear upgrades erase meaningful weakness. Run the experiment before scaling to expensive Unreal content production.

## Security and operational boundary

No network requests, account access, asset downloads, telemetry or plugin auto-installation are needed to play. Browser storage is local to the player's profile; do not store sensitive information there. SoT30 sources are bundled as inert project knowledge, not executed as game runtime. Any future import of tool output or external snippets must be treated as untrusted data and reviewed before compilation.
