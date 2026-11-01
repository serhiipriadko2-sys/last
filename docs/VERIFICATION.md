# Verification receipt -- 2026-11-01

## Local execution evidence

- `node --check js/data.js js/renderer.js js/game.js` (checked individually): PASS
- `node tests/data.test.js`: PASS, 130 skills / 30 enemy variants / nine minibosses, curves, forge risks
- `python tests/browser_smoke.py`: PASS, selection -> fights -> 100 level losses -> mirror -> victory, zero JS exceptions
- `python tests/interaction_smoke.py`: PASS, timing parry and failed dodge, loss, fuse, save/load
- `python tests/battle_playthrough.py`: PASS, genuine battle win via ordinary attacks and timed parries
- `python tools/verify_sot30.py`: PASS, all 30 source hashes and lengths
- Packaged ZIP: `LastLight_Prototype_v0.2.0_Windows.zip` (external chat artifact), 4,493,114 bytes, SHA-256 `aca25812fbffedbbd7965666adf9d8f66cce0f7ec641fe7773a1a1b8018da4d6`, 59 files, ZIP CRC + path-safety + round-trip SHA checks: PASS.

## GitHub readback

Canonical knowledge files 00--29 copied to `knowledge/`. Git tree blob SHA-1 values for all 30 were compared with local `git hash-object` of the originals, and matched, including the 2,831,441-byte file 24. The large file was reconstructed from segments before the final Git blob comparison. This is **static repository parity**, not a Project upload nor an active Codex runtime attestation.

## Boundaries

Tests ran in a Linux-container Chromium sandbox; direct `file://` and localhost navigation were blocked by administrator policy, so browser tests injected the locally authored scripts into a DOM. Windows Explorer double-click launch, Unreal Engine project compilation, native .exe packaging, and complete manual Windows playthrough remain **NOT VERIFIED**.

Remote Desktop Commander: DESKTOP-ITUAMNF listed online; read requests timed out with no response. No local VS Code changes claimed. Do not collapse source checks into executable release readiness.
