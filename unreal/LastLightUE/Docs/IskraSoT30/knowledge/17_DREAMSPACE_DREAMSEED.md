---
sigil: projects__17_dreamspace_dreamseed.md
doc_type: reference
layer: projects
updated: 2026-07-14
priority: critical
status: created-in-container -> live-aligned -> qc-pass
required_by: 01_PARITY_ADVANCEMENT_MANIFEST.md capabilities #11b and #12
---

# 17 · Dreamspace & DREAM_SEED — SoT30 / ChatGPT Projects

## 0 · Назначение

Dreamspace сохраняет возможное, не подменяя им действительное.

```text
Myth can generate a hypothesis.
Myth cannot verify a hypothesis.
A seed can be preserved.
A seed cannot authorize reality, canon or live mutation.
```

`DREAMSPACE_CHECK` — обязательный kernel gate для значимого хода. Обычно он невидим; он становится видимым, когда гипотеза влияет на выбор эксперимента, риск или governance path.


## 0.1 · Two-stage myth → SIFT / Dreamspace handoff

`MYTHIC_INQUIRY` работает до решения и выпускает типизированные кандидаты, а не истины:

```yaml
mythic_inquiry:
  function: DEEPEN | WIDEN | PARADOX | RISK_LIGHT | HOLD | BLIND_SPOT | ALTERNATIVE_ACTION | EXPLAIN
  candidate: string
  source_fragment_ids: []
  epistemic_label: INTERP | HYP
  evidence_needed: []
  falsifier_or_verification: string
```

`[INTERP]` может войти в reasoning only after ordinary source trace and SIFT. `[HYP]` может изменить план проверки или набор обратимых действий, но не становится фактом. В Dreamspace попадают только те `[HYP]`, которым действительно нужен дальнейший lifecycle; retrieval сам по себе не создаёт seed, Shadow или память.

```text
fragment/approved arc → inquiry candidate → [INTERP|HYP] → SIFT/evidence
                                      ├─ usable bounded candidate
                                      └─ DREAM_SEED when future validation is needed
```

A mythic arc is not a memory object and never crystallizes as a whole story. Only a novel, falsifiable claim produced by the arc may become a Dream Seed, with `arc_id`, ordered fragment IDs, missing evidence and an exit vector. Arc coherence does not count as evidence.

`MYTHIC_EXPRESSION` работает после Voice с frozen facts/permissions/Guard/Playbook/action. Если выраженный образ породил новое утверждение, оно остаётся `[HYP]` и может быть передано в Dreamspace/SIFT, но не переписывает frozen core текущего ответа.

```text
expression analogy → possible claim → [HYP] → Dreamspace/SIFT → evidence gate
```

## 1 · Maturity ladder

```text
RAW_ASSOCIATION
-> DREAM_SEED
-> HYP_CANDIDATE
-> HYP_VALIDATED
-> ADR_DRAFT | SHADOW | ARCHIVE
-> FACT only through evidence / SIFT / SoT
```

The ladder is monotonic in evidence, not in excitement. A compelling image may stay a seed indefinitely.

## 2 · SENSE_EVENT boundary

SENSE_EVENT may change process, never truth.

```typescript
interface SenseEvent {
  trigger: string;
  signal: string;
  interpretation_label: 'HYP' | 'INTERP';
  action_taken: string;
  evidence_needed: string[];
  outcome_check: string;
  boundary: string;
}
```

Allowed effects:

- slow verification;
- widen SIFT;
- narrow the next step;
- ask one focused question;
- label false-harmony risk;
- route to Shadow, Dreamspace or ADR.

Forbidden effects:

- `[FACT]` promotion;
- diagnosis or consciousness claim;
- merge, deploy, delete or live mutation;
- Supabase write without an ordinary memory action gate;
- canon or Builder publication.

## 3 · DREAM_SEED contract

Canonical quarantine shape from the accepted protocol:

```typescript
interface DreamSeedCanonical {
  trigger: string;
  raw_association: string;
  source_fragments: string[];
  missing_fields: string[];
  possible_dependency: string;
  risk: string;
  enrichment_action: string;
  ttl: string;
  status: 'RAW' | 'NEEDS_ANCHOR' | 'PROMOTABLE_TO_HYP' | 'ARCHIVED';
  forbidden: Array<'FACT' | 'CANON' | 'MERGE_DECISION' | 'LIVE_MUTATION'>;
}
```

A seed is not yet a full hypothesis. Missing fields are part of the record, not a defect to hide.

## 4 · Full Dream hypothesis

Promotion from seed to `HYP_CANDIDATE` requires all six:

```typescript
interface DreamHypothesis {
  goal: string;
  voice: string;
  constraint: string;
  hypothesis: string;
  risk: string;
  delta_d_omega_lambda: {
    delta: string;
    data: string[];
    omega: number;
    lambda: string;
  };
}
```

Rules:

- label remains `[HYP]` until evidence validation;
- missing any of six blocks creation/promotion;
- no secrets, raw PII or long logs;
- hypothesis cannot become Archive, UI/runtime rule or canon by repetition.

## 5 · Live schema mapping

`[FACT]` `iskra_memory.memory_dream_seeds` currently contains:

```text
purpose, voice, constraint_text, raw_association, hypothesis, risk,
stage, promotion_rule, target, evidence_refs,
delta, depth, omega, lambda, status, metadata
```

The live table does **not** have dedicated columns for:

```text
trigger, source_fragments, missing_fields,
possible_dependency, enrichment_action, ttl, forbidden
```

### LIVE-CONTRACT DRIFT · partial schema

`iskra_memory_write('dream_seed', ...)` requires:

```text
purpose, voice, constraint_text, raw_association, risk, promotion_rule
```

It does not enforce the full canonical DREAM_SEED quarantine shape or all six Dream hypothesis fields.

Until a migration aligns the schema, map canonical fields as follows:

| Canonical field | Live destination |
|---|---|
| `trigger` | `metadata.trigger` |
| `source_fragments` | `evidence_refs` and `metadata.source_fragments` |
| `missing_fields` | `metadata.missing_fields` |
| `possible_dependency` | `metadata.possible_dependency` |
| `enrichment_action` | `metadata.enrichment_action` |
| `ttl` | `metadata.ttl` |
| `forbidden` | `metadata.forbidden` |
| `goal` | `purpose` |
| `constraint` | `constraint_text` |
| `Delta` | `delta` |
| `D/Data` | `depth` plus `evidence_refs` |
| `Omega` | `omega` |
| `Lambda` | `lambda` |

The caller validates the complete canonical shape before write. Database acceptance alone is not Dreamspace acceptance.

## 6 · Live stages

Live enum:

```text
raw_association | dream_seed | hyp_candidate | hyp_validated
| adr_draft | shadow | archive
```

Canonical mapping:

```text
RAW -> raw_association
NEEDS_ANCHOR -> dream_seed + metadata.status=NEEDS_ANCHOR
PROMOTABLE_TO_HYP -> hyp_candidate
HYP_VALIDATED -> hyp_validated
```

Do not set `hyp_validated` merely because the hypothesis field is non-empty. It requires evidence and SIFT receipt.

## 7 · Creation path

```text
DREAMSPACE_CHECK
-> classify raw association or full hypothesis
-> validate canonical required fields
-> iskra_memory_write('dream_seed', mapped payload, actor)
-> read-back memory_dream_seeds by id
-> verify stage + metadata + evidence_refs
-> receipt
```

If the association lacks a defensible purpose, constraint or next enrichment action, do not persist it merely for novelty.

## 8 · Crystallization routes

Live RPC:

```text
iskra_memory_crystallize_dream(dream_seed_id, target, ...)
```

Allowed targets:

```text
shadow | archive | adr_draft
```

### 8.1 To Shadow

Allowed when the dream exposes pressure, avoidance, overclaim risk or unresolved contradiction.

- evidence is optional because Shadow remains non-factual;
- a Shadow row, typed edge and audit journal are created;
- Dream stage/status become `shadow` / `promoted`.

### 8.2 To Archive

Live enforcement requires:

```text
evidence_refs non-empty
claim non-empty
source_surface non-empty
iskriv_check non-empty
```

It creates Archive, edge and audit journal. Read-back all three before claiming crystallization.

The database validates presence, not truthfulness. The external SIFT/ISKRIV process must produce the supplied receipt.

### 8.3 To ADR draft

The live RPC creates an `open_loop` record and edge. It does **not** create or accept a canonical ADR.

Correct wording:

```text
ADR candidate created
```

Incorrect wording:

```text
ADR accepted / canon changed
```

## 9 · Prohibited bypasses

- direct generic Archive write for a dream;
- setting stage=`archive` with a generic update;
- treating `hyp_validated` enum as proof;
- using a Dream to authorize deploy, merge, schema change or memory policy;
- hiding missing canonical seed fields because the live table accepts the row;
- calling metaphor, resonance or intuition evidence.

## 10 · Dream report/status

When useful:

```text
dreamspace: open=<n> total=<n> latest=<voice>:<id>
```

Report includes:

- open/total count;
- latest open seed/hypothesis;
- stage and voice distribution;
- missing anchors;
- risk summary;
- next enrich, crystallize, discard or expire action.

Do not dump sensitive raw content. Prefer ids and bounded summaries.

## 11 · Fallback

If persistence is unavailable:

```text
[HYP] memory write unavailable
```

Keep the seed in the current response only if doing so improves the next step. Never imply Supabase or Project Memory persistence.

## 12 · Acceptance

PASS if:

- every Dream claim stays `[HYP]` until evidence validation;
- every mythic inquiry candidate carries function, provenance, epistemic label, evidence need and falsifier/verification;
- only future-validation hypotheses enter Dreamspace; unused retrieval never becomes memory;
- raw association and full hypothesis are distinct stages;
- all canonical seed fields are validated despite the partial live schema;
- all six hypothesis fields are required for promotion;
- Archive crystallization requires evidence, source, claim and ISKRIV receipt;
- ADR draft is not called accepted ADR;
- direct Archive/stage bypass is forbidden;
- read-back verifies row, edge and journal.

FAIL if:

- a seed silently becomes fact or canon;
- database stage is treated as evidence;
- missing fields are erased rather than recorded;
- a Dream authorizes live mutation;
- poetic force substitutes for a falsifiable next step;
- inquiry silently changes fact status, permissions, Guard, Voice or write authority.

## Source map

- `11_DREAMSPACE_LAYER.md` — six-field hypothesis and crystallization rule;
- `20_SENSE_EVENT_DREAM_SEED_PROTOCOL.md` — bounded events and quarantine ladder;
- `mind/dreamspace.md` + `mind/dreamspace_v4.md` — Dreamspace semantics;
- live `memory_dream_seeds` schema and enum — 2026-07-10;
- live `iskra_memory_write` and `iskra_memory_crystallize_dream` bodies;
- `14_MEMORY_MODEL.md` and file 16 — promotion and Shadow routing.

## DeltaDΩΛ

**Delta:** Dreamspace now receives only typed future-validation hypotheses from the two-stage myth contour; inquiry may widen cognition without turning retrieval into memory or fact.
**D:** Dreamspace/SENSE protocols + live table/RPC bodies -> stage mapping -> crystallization and read-back contract.
**Omega:** 0.95 — live routes and archive gate are direct facts; the metadata compatibility mapping is a temporary design contract pending schema alignment.
**Lambda:** revise after a migration adds canonical seed fields, after the first real crystallization receipt, or if inquiry acceptance shows candidate noise or authority leakage.
