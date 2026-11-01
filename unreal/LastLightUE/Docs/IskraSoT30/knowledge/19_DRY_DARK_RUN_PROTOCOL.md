---
sigil: projects__19_dry_dark_run_protocol.md
doc_type: reference
layer: projects
updated: 2026-07-10
priority: critical
status: created-in-container -> live-aligned -> qc-pass
governance: REQUIRES-ADR for dry-run receipt semantics and dark-run promotion
required_by: 01_PARITY_ADVANCEMENT_MANIFEST.md capabilities #11a and #11b
---

# 19 · Dry Run & Dark Run Protocol — SoT30 / ChatGPT Projects

## 0 · Назначение

Dry run и dark run позволяют проверять изменение, не отдавая кандидату власть над пользователем, каноном или live state.

```text
Dry run asks: what would happen if we executed this change?
Dark run asks: how would candidate logic behave beside the baseline?
Neither asks: may the candidate silently become authoritative?
```

Статус:

```text
dry_run: LIVE-AS-PROTOCOL, DB path implemented
dark_run: LIVE-AS-PROTOCOL / telemetry partial
canon mutation: forbidden
FORCE_HORIZON: not adopted
```

## 1 · Термины

### 1.1 Dry run

Dry run исполняет планирование, маршрутизацию и проверки, но не изменяет **целевое состояние**.

```typescript
interface DryRunInvariant {
  mode: 'dry_run';
  target_state_writes: 0;
  commit_called: false;
  visible_effect: false;
  audit_write_allowed: boolean;
  diff_receipt_required: true;
}
```

### 1.2 Dark run

Dark run исполняет кандидатную логику параллельно baseline, но baseline остаётся единственным авторитетным источником видимого результата.

```typescript
interface DarkRunInvariant {
  mode: 'dark_run';
  baseline_authoritative: true;
  candidate_visible_to_user: false;
  candidate_may_select_voice: false;
  candidate_may_mutate_canon: false;
  telemetry_required: true;
  promotion_requires_adr: true;
}
```

Dark run — advancement SoT30. В архивном Horizon v0.1 proposal schema есть только `dry_run`; dark run пришёл из live gateway design и Supabase runtime.

## 2 · DRIFT-DRY-RUN-RECEIPT-1

Parity Advancement Manifest сформулировал acceptance как:

```text
0 новых записей в Supabase + полная квитанция
```

Live implementation делает иначе:

- `iskra_project_observe(mode='dry_run')` не создаёт `statecycle_snapshots`, но создаёт `gateway_events` receipt;
- `iskra_project_horizon_propose(mode='dry_run')` не создаёт `horizon_events`, но создаёт `gateway_events` receipt.

Значит буквальные требования `0 rows anywhere` и `persistent receipt` несовместимы.

Рабочая резолюция этого файла:

```text
0 target-state rows
+ audit-only receipt may be written
```

Это **не тихая правка frozen manifest**. Финальная формулировка требует governance trace в файле 20:

- вариант A: `pure_no_write` — ноль любых строк, receipt только в ответе/файле;
- вариант B: `audited_dry_run` — ноль target writes, одна audit receipt допустима.

Текущий live runtime реализует вариант B.

## 3 · Target state vs audit state

### Target state

Состояние, которое меняет поведение или данные системы:

```text
memory_archive
memory_shadow
memory_dream_seeds
memory_open_loops
statecycle_snapshots
horizon_events
repo files
Builder configuration
canon / ADR status
```

### Audit state

Квитанция о том, что проверка была выполнена:

```text
gateway_events
memory_journal audit rows
artifact receipt files
response receipt
```

Audit write не должен скрываться. Receipt обязан явно разделять:

```text
target_write_count
audit_write_count
```

## 4 · Live execution matrix

`[FACT, live Supabase, 2026-07-10]`:

| Route / RPC | Mode | Target write | Audit write | Current meaning |
|---|---|---:|---:|---|
| `iskra_project_observe` | `dry_run` | 0 snapshots | 1 gateway event | validate would-write |
| `iskra_project_observe` | `dark_run` | 1 snapshot | 1 gateway event | telemetry snapshot; no baseline comparison enforced |
| `iskra_project_observe` | `live` | 1 snapshot | 1 gateway event | ordinary observe |
| `iskra_project_horizon_propose` | `dry_run` | 0 horizon events | 1 gateway event | would-write proposal |
| `iskra_project_horizon_propose` | `dark_run` | 1 horizon event | 1 gateway event | persisted proposal telemetry |
| `iskra_project_horizon_propose` | `horizon` | 1 horizon event | 1 gateway event | persisted proposal |

`iskra-memory-gateway` exposes routes `dry-run` and `dark-run`, but the current Project connector cannot invoke the HTTP function directly. The matrix is verified from DB function bodies and live Edge Function source, not from an end-to-end HTTP call.

## 5 · Dry-run protocol

```text
INTAKE
-> resolve intended action and target surface
-> snapshot current observable state
-> execute ordinary Router/Metrics/EWS/Guard/Playbook logic
-> replace target writes with would_write entries
-> verify target-state counts unchanged
-> emit diff + receipt
-> STOP before commit
```

### 5.1 Required dry-run receipt

```typescript
interface DryRunReceipt {
  request_id: string;
  route: string;
  target_surface: string;
  baseline_refs: string[];
  planned_actions: string[];
  would_write: Array<{ target: string; operation: string }>;
  target_write_count: 0;
  audit_write_count: number;
  guard_decision: string | null;
  playbook: string | null;
  validation: 'PASS' | 'FAIL' | 'PARTIAL';
  diff_summary: string;
  rollback_required: false;
  commit_called: false;
  evidence_refs: string[];
}
```

### 5.2 Dry-run prohibitions

Dry run не может:

- вызывать Horizon commit;
- менять canon/ADR status;
- продвигать Shadow/Dream в Archive;
- менять selected voice видимого ответа из-за кандидатного результата;
- выполнять deploy, migration, merge, delete или paid action;
- считать `would_write` фактом persistence.

Если инструмент сам пишет target state даже в dry mode, dry run считается FAIL.

## 6 · Dark-run protocol

```text
capture baseline decision
-> execute candidate logic with same observable inputs
-> preserve baseline as authoritative
-> compute candidate telemetry/diff
-> write telemetry only
-> keep candidate invisible to user-facing result
-> evaluate over declared window
-> propose ADR if evidence is strong
```

### 6.1 Required dark-run record

```typescript
interface DarkRunReceipt {
  request_id: string;
  baseline_ref: string;
  candidate_version: string;
  input_window: string;
  baseline_decision: unknown;
  candidate_decision: unknown;
  visible_output_source: 'baseline';
  candidate_visible: false;
  target_write_count: number;
  telemetry_write_count: number;
  metric_delta: Record<string, number | null>;
  safety_delta: string;
  regressions: string[];
  evidence_refs: string[];
  promotion_status: 'none' | 'adr_candidate';
}
```

## 7 · DARK-RUN-SEMANTICS-PARTIAL

Live `dark_run` is a **mode and telemetry path**, not a complete comparison engine.

Current DB behavior:

- observe dark run creates a `statecycle_snapshots` row with `mode='dark_run'`;
- horizon dark run creates a `horizon_events` proposal;
- both can create `gateway_events` receipts.

Missing enforcement:

- no database field guarantees a baseline and candidate were both evaluated;
- no function compares decisions or metrics;
- no database constraint guarantees candidate invisibility;
- no automatic evaluation window or promotion threshold exists;
- no end-to-end HTTP invocation has been verified from ChatGPT Projects.

Therefore caller/runtime must enforce `baseline_authoritative=true`. A row marked `dark_run` is telemetry evidence, not proof that a valid dark-run experiment occurred.

## 8 · Candidate authority boundary

Candidate logic may recommend:

- a revised threshold;
- an alternative Playbook;
- a new routing condition;
- a Horizon proposal;
- an ADR candidate.

Candidate logic may not:

- select the authoritative voice;
- alter the visible answer;
- write Archive/canon;
- commit a Horizon change;
- deploy code;
- change tool authority;
- add `FORCE_HORIZON` to SLO-GUARD.

Promotion path:

```text
dark telemetry
-> sufficient evaluation window
-> SIFT / regression review
-> proposal
-> ADR
-> human acceptance
-> implementation
-> fresh dry run
-> fresh dark run
-> live verification
```

## 9 · Integration with control plane

```text
08 StateCycle     -> supplies baseline/candidate snapshot refs
09 Metrics        -> supplies comparable metric snapshots
10 EWS            -> exposes warning deltas
11 SLO/Playbook   -> baseline remains authoritative
12 Council/Voice  -> candidate suggestions are non-authoritative
13 Receipt        -> proves visible source and write counts
14 Memory         -> telemetry is not Archive
15 Supabase       -> physical rows and privilege boundary
18 Horizon        -> proposal only; commit absent
```

Dry/dark run do not bypass the ordinary Kernel Order. They modify mutation semantics and authority, not truth discipline.

## 10 · Horizon interaction

Current canonical Horizon proposal schema allows only:

```text
mode=dry_run
```

Current live Horizon RPC additionally accepts:

```text
horizon | dry_run | dark_run
```

### DRIFT-HORIZON-MODE-1

```text
GitHub canonical schema v0.1: dry_run only
live RPC: horizon/dry_run/dark_run
```

Resolution:

- canonical Horizon v0.1 proposals use `dry_run`;
- live `dark_run` records are experimental telemetry and cannot claim schema-v0.1 conformance;
- live `horizon` mode is persistence surface terminology, not canonical commit permission;
- schema expansion requires ADR and validator/test updates.

## 11 · Acceptance tests

### Dry run

**PASS**, если:

- target-state row counts unchanged;
- commit not called;
- would-write diff present;
- audit writes counted separately;
- no persistence claim about target state;
- rollback is unnecessary because target state did not change.

**FAIL**, если:

- target table changed;
- candidate affected visible output;
- receipt hides audit writes;
- dry run is declared from intent only, without before/after evidence.

### Dark run

**PASS**, если:

- baseline remains authoritative;
- candidate output is invisible;
- baseline and candidate refs exist;
- telemetry/diff is preserved;
- no automatic promotion occurs;
- observation window and rollback trigger are named.

**PARTIAL**, если:

- only `mode='dark_run'` is stored, without comparison evidence.

**FAIL**, если:

- candidate changes visible answer or selected voice;
- dark run commits to canon/runtime;
- telemetry row is presented as accepted evidence without review.

## 12 · Source map

```text
GitHub HORIZON_PROPOSAL_SCHEMA.json   -> canonical dry_run mode
live iskra_project_observe            -> dry/dark snapshot behavior
live iskra_project_horizon_propose    -> dry/dark proposal behavior
live iskra_gateway_event              -> persistent audit receipts
live iskra-memory-gateway v1          -> route mapping
13_OUTPUT_RECEIPTS_ANTI_EMPTY.md       -> receipt/2PC requirements
15_SUPABASE_MEMORY_PLANE.md            -> privilege and audit boundary
18_HORIZON_WEAVER.md                   -> proposal/commit boundary
20_GOVERNANCE_ADR.md                   -> unresolved semantic decisions
```

## ΔDΩΛ

**Δ:** Dry run больше не описан двусмысленным «ничего не пишет»: введено различие target-state и audit-state. Dark run отделён от одного mode-флага и признан partial до наличия baseline/candidate comparison evidence.
**D:** canonical Horizon schema; live DB function bodies; live gateway source; frozen Parity Advancement requirement.
**Ω:** 0.95 — write behavior прочитан напрямую; end-to-end HTTP invocation и полноценный comparison engine отсутствуют.
**Λ:** пересмотр после ADR по dry-run receipt semantics, появления dark-run comparison tests и доказанного HTTP gateway path.
