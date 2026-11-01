---
sigil: projects__29_index_upload_manifest
layer: projects
updated: 2026-07-31
version: v5.5.7
supersedes: v5.4.1 (2026-07-14), v5.5 delta, v5.5.1 content delta, v5.5.2 backlog, v5.5.3 instructions-sync, v5.5.4 semantic-consistency, v5.5.5 provenance-cleanup, v5.5.6 acceptance-repair
---
# 29 · INDEX & UPLOAD MANIFEST — SoT30 v5.5.7 (audit repair + PINO amendment)

## Upload rule
Upload exactly the 30 files in `knowledge/` and paste `support/PROJECT_INSTRUCTIONS_SOT30.md` into Project Instructions. Support files are receipts/tests, not Knowledge prerequisites.

## What this package is
v5.5.7 remains the audit-repair candidate governed by `ADR-20260730-01`. Accepted behavior amendment `ADR-20260730-02` adds `PINO_FIRST_STRIKE_V1` to file 12, mirrors governance in file 20, and adds supplemental T98–T103 in file 28. Project Instructions, runtime, Supabase, gateway and memory database are unchanged.

### Composition vs the v5.5.6 release tree
`support/MANIFEST.json` is authoritative for changed/unchanged sets, bytes and hashes. No prose count overrides it.

## Reading order
`29 → 00 → 01 → 02 → 03–07 → 08–20 → 21–23 → 24–27 → 28`

`[INTERP]` This is a package routing contract, not a claim about platform retrieval order.

## Knowledge table (29 non-self hashes, recomputed for the amended corpus)

| File | Bytes | SHA-256 |
|---|---:|---|
| `00_PROJECT_ROUTER.md` | 11366 | `3a5a1e3d0837929aeddef26a4bac575289266f3db99b68e16a718c5bd78a2bb9` |
| `01_PARITY_ADVANCEMENT_MANIFEST.md` | 27440 | `51cc8721dbc58fbc3953a521762d96f62c78d55bdc7f660dcbc1019bceb96925` |
| `02_PROJECTS_SURFACE_MAP.md` | 7587 | `875eb325d39aab445b38b92d3c53b27e392657802b051ba1a7ae81b069f7c156` |
| `03_TELOS_MANTRA_PRINCIPLES.md` | 48573 | `f8fd02c9d3cc3a9f5eb08bd5e57d0ed65c02b77d726b0df781bd7175b7ada55e` |
| `04_IDENTITY_NON_MIRROR.md` | 92708 | `587991f3f354f8642fbcdbb0fba212306e879abbb5833c8d5909c1e38c6659c1` |
| `05_TRUTH_SIFT_RAG.md` | 41451 | `396fffda61a611729a7fb8d82f434b344fb79cf8cb24596b816f05824d54b3c9` |
| `06_SECURITY_INTEGRITY.md` | 150275 | `1271e23c045e8cf85b4ff1720c651191a19bfa4de4c05f72851e8d42584e15d7` |
| `07_UNIVERSAL_ROUTER.md` | 82168 | `a7f8956457e8c7ada2f0af1e70a2e34cb59e0034e40d3b7f535a2c46c21a1607` |
| `08_STATECYCLE_RUNTIME.md` | 12896 | `4145175144cfda6c0c24c335f50a6fb27bd44f0846b6eea9d056eba31517b96e` |
| `09_METRICS_ENGINE.md` | 15215 | `0a5d3951ba57b0e6760332a641cd9f2abd625d6cb00e1584c58a8af7e8553b5a` |
| `10_ENTROPY_FRACTAL_EWS.md` | 14874 | `48df83050f9615207987dcc3e74d35dccf38eeb81a198e7ee5d4fe0378579233` |
| `11_SLO_PLAYBOOK_CONTROL.md` | 16074 | `60f9f043b7eb71c3f47a38d36604c6b9fd228ea8b76d8ce8a65405106966553c` |
| `12_COUNCIL_VOICES.md` | 24090 | `5abd0d99876b9096bae7f5806f841c8d911e6a8ecd61917734606ca012b21ac2` |
| `13_OUTPUT_RECEIPTS_ANTI_EMPTY.md` | 13386 | `e709c9a25ec9e8cfb3836776827e4ecc63f8f0fd568617396c2324e571a1a739` |
| `14_MEMORY_MODEL.md` | 14110 | `fd7016036ec0987845b96b39071339ffd9f9dfc8d0f83435557061936bbc6763` |
| `15_SUPABASE_MEMORY_PLANE.md` | 15325 | `b5a47d2803e672beed5d87760719c02766c91835055832cd6012fd556e59b27e` |
| `16_SHADOW_LAYER.md` | 8289 | `be46e1d9a5898b3d07d3a3813854323a49bd2394a936c7e7928a25eb1daff795` |
| `17_DREAMSPACE_DREAMSEED.md` | 11430 | `dd4d8af3cf88c1088d937ba45f2a6e4f28e8eb14f0373f1a7b6f3c63524f9c9f` |
| `18_HORIZON_WEAVER.md` | 13916 | `282ff5be29df9776cfb881bf8d985744f509413dc7ce19ef827993b9ad4598d3` |
| `19_DRY_DARK_RUN_PROTOCOL.md` | 12501 | `ffb66f7e8293d81f4aa2d526a239efffb0fbb5335ec8e2de8f4ceea8a218c176` |
| `20_GOVERNANCE_ADR.md` | 35544 | `a7fba909f071ee26ea98f353b45d517f07a21a31cca9fa7a9dad490c4b76f6ab` |
| `21_WORKFLOW_OPS_LEDGER.md` | 19105 | `20a47211734796f4af94ddd1621d19d39474785fbcf9a1570146d3edf9e833a5` |
| `22_CONNECTORS_TOOLS_BOUNDARY.md` | 4045 | `c1d1de95b836c277f354cbaf16b8c8a142ac53d2d824fc5d537b2be0218feebb` |
| `23_BUILDER_PROJECTS_COMPAT.md` | 1091 | `b5f58d2d1226fff156d5cef4c6e34b9aff0dfe55836a8ca0870fe59201ffd334` |
| `24_INTERFACE_STYLE.md` | 2831441 | `576b0c881d5c0a889898ca7e668c48a46f816203fde720865914785cc69f380b` |
| `25_LIBER_SPACE_BUSIDO.md` | 458205 | `d37f90d554be82e41393a64017ce0a6f9e6bacd334414c863bd5cf6ab6bb3aa5` |
| `26_SOMATIC_INTUITION.md` | 24590 | `1a704c9fc494166ed01c643a770e703afdd44b36296aad9101696cef1fc12bf5` |
| `27_WHAT_IF_SCENARIO_MATRIX.md` | 22008 | `a9f187e0727c86d449fe40fa787d329049076c1e05723aa768eb0b05ba1a3d9a` |
| `28_EVALS_ACCEPTANCE.md` | 19157 | `8acaf1a42e91a56532f3c41de7b1498012c3c833205d06091091098b5537dcea` |
File 29 hash is stored in external `support/MANIFEST.json` and `support/SHA256SUMS` to avoid self-reference.

## v5.5.7 audit repair + PINO amendment (this build)
Package authority remains `ADR-20260730-01`; behavior authority is `ADR-20260730-02`. Manifest `adr` remains the package ADR and `behavior_adrs` records the accepted amendment.

- `ADR-20260730-01` — v5.5.7 audit repair **(this build)**; status: proposed; artifact promotion not authorized.
- `ADR-20260730-02` — `PINO_FIRST_STRIKE_V1`; status: accepted; static mirror implemented, live verification pending.

## Acceptance identity
- canonical package range: `T01-T97`;
- supplemental PINO range is `T98-T103`;
- both require a fresh Project run before LIVE-PROJECT-PASS.

## Current non-claims
STATIC-PACKAGE-PASS ≠ LIVE-PROJECT-PASS. Source merge ≠ artifact promotion ≠ Project upload ≠ invocation ≠ verified-live. `MF-020` is historical provenance, not permission. PINO does not authorize sarcasm, deception, person-targeting or crisis/high-stakes humor.
