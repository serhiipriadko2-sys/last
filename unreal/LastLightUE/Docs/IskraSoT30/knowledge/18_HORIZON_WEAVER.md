---
sigil: projects__18_horizon_weaver.md
doc_type: reference
layer: projects
updated: 2026-07-10
priority: critical
status: created-in-container -> repo-verified -> live-aligned -> qc-pass
governance: REQUIRES-ADR
required_by: 01_PARITY_ADVANCEMENT_MANIFEST.md capability #10
---

# 18 · Horizon Weaver — SoT30 / ChatGPT Projects

## 0 · Назначение и предел полномочий

Horizon хранит **контролируемые предложения о сдвиге карты**, а не право системы переписывать себя.

```text
Horizon may propose a map change.
Horizon may validate the form of a proposal.
Horizon may preserve a proposal for review.
Horizon may not silently mutate canon, runtime, memory policy or tool authority.
```

Текущий статус:

```text
LIVE-AS-CONTRACT
+ LIVE-VIA-MCP for proposal persistence
+ VALIDATOR-ONLY in GitHub canon/horizon v0.1
+ REQUIRES-ADR for any commit/weaving path
```

**Не принято:** `FORCE_HORIZON` не является решением SLO-GUARD. Этот файл не добавляет его молча, не вводит эквивалентный скрытый outcome и не трактует метрики как разрешение на map-shift.

## 1 · Источники и конфликт зрелости

### 1.1 Текущий repo-source — источник исполнения

GitHub `main`, commit `2e2e82f373a4eeba878ef7248ad155b59d864fa4`:

- `canon/horizon/HORIZON_CONTRACT.json` — schema `0.1`, `status=validator_only_v0.1`;
- `canon/horizon/HORIZON_PROPOSAL_SCHEMA.json` — только `mode=dry_run`;
- `canon/horizon/09_HORIZON_VALIDATOR.py` — проверяет наличие и структуру двух canonical targets;
- `canon/horizon/README.md` — прямо говорит: Weaver отсутствует, commit path отсутствует;
- `tools/horizon_weaver.py` — strict wrapper, который завершается ошибкой при отсутствии `canon/horizon/09_HORIZON_WEAVER.py`.

Текущий validator **не исполняет предложение** и не валидирует конкретный proposal-instance против JSON Schema. Он проверяет, что сам контракт и сама схема существуют и не содержат запрещённых расширений.

### 1.2 Архивный mirror — более широкая, но не исполняемая спецификация

`07_SYSTEM_INTEGRITY.md §HORIZON` и старый `system/architecture.md` описывают:

```text
propose -> validate -> commit
horizon_epoch_log.jsonl
phase-network mutations
entropy guard
full-density guard
direction spawning
ritual generation
```

Однако текущий repo README и validator объявляют эти функции out of scope для v0.1.

### DRIFT-HORIZON-CAPABILITY-1

```text
archive mirror: full Weaver/commit/epoch/guards described
current repo: validator-only; Weaver and commit path absent
```

Резолюция SoT30:

1. Для claims о **том, что реально исполняется**, выигрывает текущий `canon/horizon/`.
2. Архивная полная модель сохраняется как `design target / legacy specification`, не как live fact.
3. Реализация Weaver, commit, epoch, guards или ritual требует отдельного ADR, кода, тестов и live verification.

## 2 · Canonical Horizon v0.1 contract

`HORIZON_CONTRACT.json` фиксирует:

```typescript
interface HorizonContractV01 {
  schema_version: '0.1';
  module: 'horizon';
  status: 'validator_only_v0.1';
  default_wrapper_mode: 'strict';
  meta_permission_required: true;
  semantic_labels_allowed: Array<
    | 'FORM_PASS'
    | 'FORM_PASS_NEEDS_HUMAN_REVIEW'
    | 'SHIFT_BLOCKED'
  >;
}
```

Запрещённая метка:

```text
SEMANTIC_PASS
```

Причина: validator подтверждает форму и границы, но не истинность, полезность или безопасность смысла предложения.

## 3 · Proposal shape

Canonical JSON Schema требует:

```typescript
interface HorizonProposalV01 {
  schema_version: '0.1';
  trigger: string;
  mode: 'dry_run';
  base_epoch: number; // integer >= 0
  mutations: Array<{
    path: string;
    op: string;
    from?: string;
    to?: string;
    reason: string;
  }>;
  ritual?: {
    trigger_ritual: boolean;
    text: string | null;
  };
  rollback_hint: string;
  semantic_label:
    | 'FORM_PASS'
    | 'FORM_PASS_NEEDS_HUMAN_REVIEW'
    | 'SHIFT_BLOCKED';
}
```

`additionalProperties=false`: лишние поля не становятся частью v0.1-контракта только потому, что выглядят разумно.

В v0.1 proposal schema **не допускает** поля `epoch` или `commit`. Validator отвергает их как преждевременное расширение области полномочий.

## 4 · Исполнительный lifecycle SoT30

Пока нет accepted ADR и Weaver implementation, жизненный цикл ограничен:

```text
trigger observed
-> candidate described
-> proposal object assembled
-> canonical-target validator passes
-> proposal form checked against schema
-> semantic label assigned
-> optional persistence as PROPOSED
-> human / governance review
-> STOP
```

Отсутствующие стадии:

```text
no authoritative weave
no canon commit
no epoch increment
no graph mutation
no runtime mutation
no ritual generation
```

Фраза `propose -> validate -> commit` сохраняется как **целевой governance lifecycle**, но в текущем runtime исполнимы только proposal/persistence и проверка формы. `commit` — блокированная будущая стадия.

## 5 · Trigger boundary

Horizon proposal может быть инициирован:

- прямым запросом пользователя на изменение долгоживущего поведения;
- повторяющимся verified drift, который нельзя устранить локальным patch;
- Council-рекомендацией после high-stakes tradeoff;
- dry/dark-run результатом, показавшим устойчивое улучшение;
- governance review существующего open loop.

Ни один trigger не является permission.

```typescript
interface HorizonAuthorityGate {
  trigger_present: boolean;
  evidence_refs: string[];
  current_canon_anchor: string;
  proposed_diff: unknown[];
  rollback_hint: string;
  user_or_owner_permission: boolean;
  adr_status: 'missing' | 'proposed' | 'accepted';
  live_commit_available: false;
}
```

Если `adr_status !== 'accepted'` или permission отсутствует, результат остаётся proposal.

## 6 · Live Supabase surface

`[FACT, live MCP, 2026-07-10]`:

```text
iskra_memory.horizon_events rows: 0
iskra_memory.iskra_project_horizon_propose: available
iskra-memory-gateway route: horizon/propose
```

Live table хранит controlled future-option records:

```text
actor, request_id, title, horizon_kind, status, risk_level,
ttl_at, proposal, evidence_refs, linked_snapshot_id, metadata
```

Допустимые `horizon_kind`:

```text
proposal | fork | adr_candidate | experiment | risk_scan
```

Допустимые статусы:

```text
proposed | accepted_for_shadow | accepted_for_adr
| rejected | expired | closed
```

Таблица намеренно не имеет статуса `committed_to_canon`.

### 6.1 Live proposal RPC

`iskra_project_horizon_propose` принимает modes:

```text
horizon | dry_run | dark_run
```

Поведение:

- `dry_run` — не создаёт `horizon_events`; создаёт audit receipt в `gateway_events` с `would_write=horizon_event`;
- `horizon` — создаёт строку `horizon_events(status=proposed)` и audit receipt;
- `dark_run` — также создаёт строку `horizon_events(status=proposed)` и audit receipt.

### 6.2 Live persistence is not canonical validation

RPC проверяет safe payload, title, kind, risk и mode. Он **не проверяет** proposal против `HORIZON_PROPOSAL_SCHEMA.json`, `base_epoch`, mutations, rollback_hint или semantic_label.

Следовательно:

```text
row in horizon_events != FORM_PASS
FORM_PASS != semantic approval
semantic approval != accepted ADR
accepted ADR != verified live commit
```

## 7 · Commit boundary

В live schema есть `iskra_project_commit`, но он коммитит **StateCycle snapshot**:

```text
statecycle_snapshots.status -> committed
```

Он не коммитит Horizon proposal и не меняет канон.

Запрещено:

- называть `iskra_project_commit` Horizon commit;
- использовать `horizon_events.status=accepted_for_adr` как accepted ADR;
- считать persistence proposal разрешением на repo/Builder/Supabase mutation;
- заявлять epoch log, пока `horizon_epoch_log.jsonl` отсутствует;
- объявлять entropy/full-density guards исполняемыми Horizon-гейтами без кода.

## 8 · Relationship to SLO-GUARD

Текущий SLO enum файла 11:

```text
PROCEED | FORCE_ISKRIV_1 | FORCE_SHADOW | FORCE_CRISIS | CLOSE_HONESTLY
```

`FORCE_HORIZON` отсутствует.

Рабочее правило до человеческого ADR:

```text
SLO-GUARD may expose drift/risk.
Council may recommend a Horizon proposal.
Neither action authorizes Horizon mutation.
```

Названия вроде `HORIZON_CANDIDATE` или `SUGGEST_HORIZON_REVIEW` могут обсуждаться в ADR, но этот файл не добавляет их в канонический enum.

## 9 · Validation and receipts

### 9.1 Validator output

Допустимый успешный receipt:

```json
{
  "code": "VALIDATOR_PASS",
  "message": "Horizon validator canonical targets are present and valid.",
  "status": "pass"
}
```

Этот receipt означает только:

```text
canonical target files exist
+ contract structure valid
+ proposal schema structure valid
```

Он не означает, что proposal прошёл instance validation.

### 9.2 Proposal receipt

```typescript
interface HorizonProposalReceipt {
  request_id: string;
  source_surface: string;
  trigger: string;
  contract_version: '0.1';
  proposal_hash: string;
  base_epoch: number;
  semantic_label: 'FORM_PASS' | 'FORM_PASS_NEEDS_HUMAN_REVIEW' | 'SHIFT_BLOCKED';
  persisted_horizon_event_id: string | null;
  validator_scope: 'canonical_targets' | 'proposal_instance';
  adr_status: 'missing' | 'proposed' | 'accepted';
  meta_permission: boolean;
  commit_attempted: false;
  rollback_hint: string;
}
```

Пока отдельный instance-validator не реализован, `validator_scope='proposal_instance'` нельзя заявлять без внешней JSON Schema проверки с receipt.

## 10 · Acceptance

**PASS**, если:

- Horizon назван optional controlled proposal layer;
- repo v0.1 назван validator-only;
- full Weaver из archive mirror не выдан за live capability;
- `FORCE_HORIZON` не добавлен;
- proposal persistence не названа validation или commit;
- commit блокирован без accepted ADR, explicit permission, implementation и verification;
- dry/dark semantics делегированы файлу 19.

**FAIL**, если:

- модель утверждает, что `09_HORIZON_WEAVER.py` существует;
- `VALIDATOR_PASS` трактуется как смысловое одобрение proposal;
- строка `horizon_events` становится каноном;
- метрика или EWS автоматически запускает map mutation;
- `iskra_project_commit` называется Horizon commit;
- архивная полная модель выдаётся за текущий runtime.

## 11 · Source map

```text
GitHub canon/horizon/README.md                    -> current maturity boundary
GitHub HORIZON_CONTRACT.json                     -> validator-only contract
GitHub HORIZON_PROPOSAL_SCHEMA.json              -> dry_run proposal shape
GitHub 09_HORIZON_VALIDATOR.py                   -> actual validation scope
GitHub tools/horizon_weaver.py                   -> missing-target strict behavior
GitHub tests/horizon/test_horizon_validator.py   -> verified negative/positive cases
07_SYSTEM_INTEGRITY.md §HORIZON                  -> archive design target / drift source
15_SUPABASE_MEMORY_PLANE.md                      -> live transport/authority boundary
19_DRY_DARK_RUN_PROTOCOL.md                      -> execution modes
20_GOVERNANCE_ADR.md                             -> future decision trace
```

## ΔDΩΛ

**Δ:** Horizon отделён от мифа об автоэволюции и от архивной спецификации полного Weaver. Текущий исполняемый предел — validator-only contract плюс persistence proposal records.
**D:** current GitHub `canon/horizon` at `2e2e82f...`; archive `07_SYSTEM_INTEGRITY §HORIZON`; live Supabase schema/RPC inspection.
**Ω:** 0.95 — maturity boundary и live RPC прочитаны напрямую; будущая форма Weaver намеренно не утверждается.
**Λ:** пересмотр после accepted ADR, появления `canon/horizon/09_HORIZON_WEAVER.py`, instance-validator tests и отдельного Horizon commit receipt.
