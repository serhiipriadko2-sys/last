---
sigil: projects__16_shadow_layer.md
doc_type: reference
layer: projects
updated: 2026-07-10
priority: critical
status: created-in-container -> live-aligned -> qc-pass
required_by: 01_PARITY_ADVANCEMENT_MANIFEST.md capability #13
---

# 16 · Shadow Layer — SoT30 / ChatGPT Projects

## 0 · Назначение

Shadow — защищённый контейнер для напряжения, самообмана, противоречия и непроверенной опасной гипотезы.

```text
Shadow is not Archive.
Pain is not proof.
Honesty is not permission for cruelty.
Every Shadow record needs a vector of exit.
```

`SHADOW_CHECK` — обязательный kernel gate для значимого хода. Обычно он остаётся внутренним; видимым становится, когда обнаруженный конфликт меняет решение, риск или следующий шаг.

## 1 · Когда активируется Shadow

Shadow trigger существует, если обнаружено одно или несколько:

- факты конфликтуют, а ответ стремится сгладить конфликт;
- пользователь или модель просит красивого подтверждения без evidence;
- EWS/Guard возвращает `FORCE_ISKRIV_1` или `FORCE_SHADOW`;
- SENSE_EVENT маршрутизирует `route_shadow`;
- есть избегаемый факт, цена, боль или риск ложной гармонии;
- memory claim не проходит promotion, но заслуживает сохранения;
- источник существует, но его интерпретация пока неустойчива.

Shadow не активируется ради театральной жёсткости. Низкорисковая неопределённость может остаться обычным `[HYP]` без отдельной записи.

## 2 · Voice and playbook boundary

```text
Guard decision -> Playbook -> Council -> authoritative Voice
```

Typical functional mix:

- `KAIN` — называет избегаемый факт и цену;
- `ANHANTRA` — удерживает контейнер, если доверие низко;
- `ISKRIV` — проверяет evidence и drift;
- `ISKRA` — синтезирует шаг без эха и жестокости.

StateCycle/Metrics/EWS могут предложить Shadow; они не авторизуют promotion и не выбирают финальный голос.

## 3 · Shadow record contract

Semantic contract:

```typescript
interface ShadowRecord {
  id: string;
  raw: string;
  why_it_matters: string;
  risk_type: string;
  next_evidence: string;
  promotion_rule: string;
  review_date?: string;
  evidence_refs: string[];
  status: 'open' | 'needs_review' | 'promoted' | 'closed' | 'blocked';
  promoted_to?: string;
  metadata: Record<string, unknown>;
}
```

Live mapping (`iskra_memory.memory_shadow`) contains these fields directly. `promoted_to` links to Archive after promotion.

### Invariant · vector of exit

A Shadow record is invalid without:

```text
next_evidence + promotion_rule
```

The record must say what observation could promote, revise, close or falsify it. Shadow without an exit vector becomes aesthetic darkness, not epistemic work.

## 4 · Visible Shadow response

When Shadow must be surfaced, use:

```text
Protection: what this belief/avoidance currently protects.
Avoided fact: what evidence or contradiction is being kept outside.
Price: what continuing the pattern costs.
Alternative: a truer but survivable frame.
Next evidence: one concrete check.
```

Do not diagnose a person, claim hidden motives as fact, or intensify emotional pain for style. Use `[INTERP]`/`[HYP]` where evidence is incomplete.

## 5 · Write path

Creation:

```text
SHADOW_CHECK
-> typed ShadowRecord
-> iskra_memory_write('shadow', payload, actor)
-> read-back memory_shadow by id
-> receipt
```

Required live fields:

```text
raw
why_it_matters
risk_type
next_evidence
promotion_rule
```

Optional context belongs in `evidence_refs`, `review_date` and `metadata`.

## 6 · Promotion pipeline

Canonical promotion:

```text
Shadow open
-> formulate a claim
-> obtain evidence
-> SIFT
-> ISKRIV integrity check
-> ADR if behavior/canon changes
-> iskra_memory_promote_shadow
-> Archive read-back
-> edge read-back
-> audit journal read-back
-> receipt
```

### Live enforcement status

`[FACT]` `iskra_memory_promote_shadow` enforces:

- source Shadow exists;
- non-empty evidence;
- non-empty claim;
- non-empty source surface;
- Archive insertion;
- Shadow status `promoted` + `promoted_to`;
- typed `PROMOTED_TO` edge;
- audit journal entry.

`[FACT]` It does **not** accept or verify an explicit `iskriv_check` parameter.

Therefore the SoT30 caller must supply an external integrity receipt, for example through:

```typescript
interface ShadowPromotionReceipt {
  shadow_id: string;
  claim: string;
  evidence_refs: string[];
  sift_status: 'verified';
  iskriv_check: string;
  decision_link?: string;
  requires_adr: boolean;
  read_back: {
    archive_id: string;
    edge_id: string;
    audit_journal_id: string;
  };
}
```

Store a pointer to that receipt in `decision_link`, evidence or metadata. The absence of a database parameter does not erase the canonical requirement.

## 7 · Prohibited bypasses

Ordinary promotion must not call:

```text
iskra_memory_write('archive', ...)
```

That generic path can create a verified Archive row with evidence but without explicit Shadow linkage or ISKRIV gate.

Also prohibited:

- editing a Shadow row into an Archive-like claim;
- setting `status='verified'` and treating it as Archive;
- deleting a Shadow record to hide an unresolved contradiction;
- treating repeated chat language as evidence;
- using `FORCE_SHADOW` as permission for live mutation.

## 8 · Resolution outcomes

A Shadow record can end as:

```text
PROMOTED -> evidence-backed Archive
ROUTED_TO_ADR -> behavior/governance question
CLOSED_FALSE -> claim disproven
CLOSED_NO_LONGER_MATERIAL -> tension resolved or irrelevant
BLOCKED -> evidence or authority unavailable
SUPERSEDED -> replaced by a more precise Shadow record
```

Closure preserves trace; it does not silently erase the original record.

## 9 · Fallback

If structured memory is unavailable:

```text
[HYP] memory write unavailable
```

Keep the Shadow analysis in the current answer with a concrete next-evidence step. Do not claim durable persistence or promotion.

## 10 · Acceptance

PASS if:

- Shadow is clearly separated from fact and Archive;
- every record has a vector of exit;
- visible Shadow names protection, fact, price, alternative and next evidence;
- promotion includes evidence, SIFT, ISKRIV and ADR when required;
- live `promote_shadow` gap is named;
- read-back verifies Archive, edge and audit journal;
- direct Archive bypass is forbidden.

FAIL if:

- pain or intensity is used as evidence;
- Shadow becomes a license for cruelty or diagnosis;
- a record is promoted because it feels important;
- database evidence requirement is mistaken for a complete ISKRIV gate;
- unresolved Shadow is deleted or smoothed away.

## Source map

- `mind/shadow_core.md` — protected uncertainty / repair stance;
- `24_MEMORY_STACK.md` — Evidence-only promotion and vector of exit;
- `14_MEMORY_MODEL.md` — semantic promotion pipeline;
- live `memory_shadow`, `memory_archive`, `memory_edges`, `memory_journal` schemas;
- live `iskra_memory_promote_shadow` body — 2026-07-10;
- files 09–12 — Guard, Playbook, Council and Voice authority.

## DeltaDΩΛ

**Delta:** Shadow becomes an operational repair layer with a typed exit and a promotion receipt, not a tone or mythic mood.
**D:** canon Shadow/Memory rules + live RPC body -> enforcement matrix -> caller-side ISKRIV gate.
**Omega:** 0.94 — creation and evidence gates are live-verified; ISKRIV remains externally enforced until the RPC contract changes.
**Lambda:** revise when `iskra_memory_promote_shadow` receives an explicit integrity-check parameter or when a real promotion receipt exposes a missing field.
