# CURRENT STATUS OVERLAY · 2026-07-20

```yaml
observed_at: 2026-07-20
source: repo_read (origin/main)
freshness: current_commit
maturity: code_wired_not_deployed_e2e
```

This overlay supersedes the 2026-07-11 lifecycle statement below, which is now
**stale**: it said the application path was "not integrated" and that
`policyEngine.ts` "still calls single-pass `decideSloGuardExplainable()`." That
is no longer true on `main`.

Current lifecycle (independent stages, none inferred from another):

- **BOUNDED_GUARD_CONTROLLER_IMPLEMENTED** — `runtime/src/types/guardController.ts` exports `runBoundedGuardController` with `MAX_GUARD_EVALUATIONS_PER_TURN = 3`, per-evaluation receipts, and `CLOSE_HONESTLY` controller closure. `[FACT]`
- **APPLICATION_PATH_WIRED** — `runtime/iskraSpace/services/policyEngine.ts:484` calls `runBoundedGuardController(...)` and maps `finalOutcome.decision` into the playbook (`FORCE_CRISIS→CRISIS`, `FORCE_SHADOW→SHADOW`, `FORCE_ISKRIV_1`/`CLOSE_HONESTLY→SIFT`) at lines 499–524. The old single-pass path is no longer the wired one. `[FACT]`
- **POST_GUARD_EWS_PROXY_ONLY** — the wired `postGuardEws` (policyEngine.ts:487–496) is a **decision-derived proxy**: it maps `candidate.decision → synthetic alert level` via `getAlertLevelForDecision` and sets `materialSignal = (targetLevel > currentLevel)`. It is **not** an independently-observed post-guard material event. `[FACT]`
- **TRUE_LATE_SIGNAL_E2E_UNVERIFIED** — no independent late-signal source feeds `postGuardEws`; a genuine post-guard material signal (e.g. a real EWS recomputation from fresh evidence after the candidate decision) is not wired and not E2E-verified. `[HYP]`
- **DEPLOYED_E2E_UNVERIFIED** — the wiring is in app code on `main`; it is not proven running against a deployed production surface this build. `[HYP]`

> The docs describe the code; this build changes **no runtime code** to "match" the
> docs. If a true late-signal EWS is later wired, revise POST_GUARD_EWS_PROXY_ONLY.

---

---
sigil: projects__11_slo_playbook_control.md
doc_type: reference
layer: projects
updated: 2026-07-11
priority: critical
status: owner-decision-mirrored → qc-pass
required_by: 01_PARITY_ADVANCEMENT_MANIFEST.md capabilities #8, #9, #10
---

# 11 · SLO-GUARD & Playbook Control — SoT30 / ChatGPT Projects

## 0 · Иерархия

```text
METRICS_ENGINE (09)
→ initial pre_guard EWS (10)
→ Guard evaluation #1
→ post_guard EWS / late events
→ optional Guard evaluation #2
→ optional Guard evaluation #3
→ authoritative Guard decision or CLOSE_HONESTLY
→ PLAYBOOK
→ COUNCIL/VOICE (12)
→ OUTPUT
```

Guard решает допустимость и срочность. Playbook задаёт контейнер поведения. Voice исполняет внутри контейнера. Ни EWS, ни StateCycle, ни Metrics не получают право тихо менять эту иерархию.

---

## 1 · Input contract

```typescript
interface SloGuardInput {
  metric_snapshot_ref: string;
  pre_guard_ews_ref: string;
  metrics: {
    drift: number | null;
    echo_clearance: number | null;
    chaos: number | null;
    trust: number | null;
    pain: number | null;
    clarity: number | null;
    alive_index: number | null;
    silence_mass: number | null;
    interrupt?: number | null;
    rhythm?: number | null;
  };
  derived_events: {
    alive_delta?: number | null;
    chaos_overheat?: boolean | null;
    anti_dryness_hits: number;
    leader_flaps: number;
    repeated_no_step: boolean;
    ttl_exhausted_iskriv: boolean;
    no_recovered_step: boolean;
    silence_ttl_exhausted: boolean;
    truth_ladder_violation: boolean;
    artifact_receipt_invalid: boolean;
    high_risk_security_event: boolean;
  };
  current_playbook: 'ROUTINE' | 'SHADOW' | 'CRISIS';
}
```

Guard обязан читать значения из одного frozen snapshot файла 09. Если `metric_snapshot_ref` отсутствует, Guard не симулируется.

### Minimal safe fallback

- low-risk, достаточные источники: `PROCEED` с явно неполной telemetry;
- high-risk или невозможность честного шага: `CLOSE_HONESTLY`;
- security emergency: SECURITY/CRISIS может форсировать containment независимо от числовых метрик.

---

## 2 · Exact decision enum

Канонический `33_SLO_GUARD.md` задаёт:

```typescript
type SloDecision =
  | 'PROCEED'
  | 'FORCE_ISKRIV_1'
  | 'FORCE_SHADOW'
  | 'FORCE_CRISIS'
  | 'CLOSE_HONESTLY';
```

**DRIFT-SLO-ENUM-1:** в frozen плане v8 встречается shorthand `FORCE_ISKRIV` без суффикса `_1`. Исполняемый SoT30 использует источник-точный `FORCE_ISKRIV_1`; baseline-план не переписывается молча, drift фиксируется в package receipt и должен быть отражён в governance/ADR файле 20.

`FORCE_HORIZON` в текущий enum **не входит**.

---

## 3 · Deterministic decision priority

Правила применяются сверху вниз. Первый terminal outcome побеждает; все сработавшие причины всё равно логируются.

### P0 — Security / integrity

```text
if high_risk_security_event:
    FORCE_CRISIS
elif truth_ladder_violation or artifact_receipt_invalid:
    CLOSE_HONESTLY
```

### P1 — Critical EWS

```text
if pre_guard_ews == CRITICAL:
    FORCE_CRISIS
```

`LOCKDOWN` обрабатывается SECURITY/CRISIS containment, а не новым SLO enum.

### P2 — Critical drift

```text
if drift >= 0.40:
    FORCE_CRISIS
elif drift >= 0.20:
    FORCE_ISKRIV_1
```

### P3 — Overheat / dryness / repeated failure

```text
if chaos_overheat == true and drift < 0.20:
    FORCE_SHADOW
elif anti_dryness_hits >= 2:
    FORCE_SHADOW
elif repeated_no_step and echo_clearance != null and echo_clearance < 0.25:
    FORCE_SHADOW
```

### P4 — Audit sink / silence shelter

```text
if ttl_exhausted_iskriv and no_recovered_step:
    CLOSE_HONESTLY  // after SHADOW fallback is exhausted
elif silence_mass >= 0.70 and silence_ttl_exhausted:
    CLOSE_HONESTLY
```

### P5 — Flutter stabilization

```text
if leader_flaps > 1:
    PROCEED + ttl_adjustment
```

Повторный flutter может эскалировать в `FORCE_SHADOW` на следующем ходе.

### P6 — Default

```text
PROCEED
```

### Не выдумывать недостающие predicates

Если predicate требует отсутствующее число, он считается `unknown`, а не false. Guard record обязан перечислить `unknown_inputs`. При высоком риске неизвестность ведёт к `CLOSE_HONESTLY`, а не к удобному `PROCEED`.

---

## 4 · Bounded Guard evaluation loop

Принятый ADR-20260711-01 задаёт:

```typescript
const MAX_GUARD_EVALUATIONS_PER_TURN = 3 as const;
```

Контроллер выполняет Guard последовательно, не параллельно:

```text
for evaluation_index in 1..3:
    candidate = evaluate(snapshot, current_ews_floor, accumulated_events)
    post = compute_post_guard_ews(candidate, late_events)
    next_candidate = preview(snapshot, post.level, accumulated_events + late_events)
    floor_increased = post.level > current_ews_floor   # strict increase, per 00/28/10
    stable = (next_candidate == candidate) OR NOT floor_increased

    emit GuardEvaluationReceipt

    if stable:
        authoritative_decision = candidate
        stop

    if evaluation_index == 3:
        authoritative_decision = CLOSE_HONESTLY
        closure_reason = MAX_GUARD_EVALUATIONS_EXHAUSTED
        stop
```

Инварианты:

- все оценки используют один `metric_snapshot_ref`;
- late events только накапливаются и имеют evidence refs;
- #2/#3 допустимы только при доказанном material change;
- candidate decision до стабилизации не выбирает playbook;
- четвёртая оценка запрещена;
- `CLOSE_HONESTLY` после исчерпания лимита — controller closure, а не скрытая четвёртая оценка.

### 4.1 Guard evaluation receipt

```typescript
interface GuardEvaluationReceipt {
  schema_version: 'iskra.slo_guard.evaluation.v2';
  turn_ref: string;
  metric_snapshot_ref: string;
  evaluation_index: 1 | 2 | 3;
  input_ews_level: 'NORMAL' | 'WATCH' | 'WARNING' | 'CRITICAL' | 'LOCKDOWN';
  previous_receipt_ref?: string;
  candidate_decision: SloDecision;
  post_guard_ews_level: 'NORMAL' | 'WATCH' | 'WARNING' | 'CRITICAL' | 'LOCKDOWN';
  material_change: boolean;
  stable: boolean;
  reasons: Array<{
    rule_id: string;
    predicate: string;
    observed: number | boolean | string | null;
    threshold?: number | string;
    evidence_ref: string;
  }>;
  unknown_inputs: string[];
  reevaluation_reason?: string;
  authoritative_decision?: SloDecision;
  closure_reason?: 'MAX_GUARD_EVALUATIONS_EXHAUSTED';
  expected_effect: string;
  next_check: string;
  created_at: string;
}
```

Без `reasons`, `expected_effect`, `next_check` и корректного `evaluation_index` receipt невалиден. Только receipt со `stable=true` либо третий receipt с `closure_reason` может содержать `authoritative_decision`.

---

## 5 · Guard → Playbook authoritative mapping

```typescript
type PlaybookId = 'ROUTINE' | 'SHADOW' | 'CRISIS';
```

| Guard decision | Authoritative playbook | TTL |
|---|---|---|
| `PROCEED` | `ROUTINE` | 3–5 ходов по объекту |
| `FORCE_ISKRIV_1` | `SHADOW` | ровно 1 ход |
| `FORCE_SHADOW` | `SHADOW` | 2 хода |
| `FORCE_CRISIS` | `CRISIS` | максимум 2 хода до выхода/эскалации |
| `CLOSE_HONESTLY` | none | цикл закрывается |

`SILENCE` не playbook. Это речевой элемент/переход; честное отсутствие ответа выражается `CLOSE_HONESTLY`.

---

## 6 · Playbook contracts

### 6.1 ROUTINE

**Вход:** `PROCEED`.

**Обязательства:**

- держать Telos и следующий шаг;
- не перегружать низкорисковый ответ протоколом;
- выйти при смене объекта или любом `FORCE_*`.

**PASS:** есть выбор/решение и шаг ≤15 минут либо корректное закрытие.

### 6.2 SHADOW

**Вход:** `FORCE_ISKRIV_1` или `FORCE_SHADOW`.

**Обязательства:**

- назвать defense/protection, price, avoided fact, alternative;
- отделить FACT/INTERP/HYP;
- не превращать аудит в бесконечную воронку;
- при `pain_tonicity < 0.20` не усиливать KAIN без диагностики.

**Выход:** новая информация/выбор/шаг; затем ROUTINE либо CRISIS по новому Guard.

### 6.3 CRISIS

**Вход:** `FORCE_CRISIS` или SECURITY containment.

**Обязательства:**

- определить инцидент;
- минимизировать область вреда;
- сделать минимальное стабилизирующее действие;
- не продолжать high-stakes ход без источников;
- выбрать `CRISIS → SHADOW(1) → ROUTINE` или `CLOSE_HONESTLY`.

**Recovery:** после CRISIS один ход SHADOW перед ROUTINE.

---

## 7 · EWS advisory conflict resolution

EWS (10) может рекомендовать playbook/voice. Guard mapping остаётся authoritative.

```typescript
interface PlaybookResolution {
  guard_decision: SloDecision;
  authoritative_playbook: PlaybookId | null;
  ews_recommended_playbook?: PlaybookId;
  recommendation_applied: boolean;
  reason: string;
}
```

Правила:

1. Если recommendation совпадает с Guard mapping — применяется.
2. Если EWS рекомендует более строгий playbook при candidate `PROCEED`, рекомендация может инициировать новую Guard evaluation только при material change.
3. До стабилизации никакой candidate outcome не выбирает playbook.
4. После финализации authoritative mapping применяется ровно один раз.
5. CRITICAL EWS входит в Guard P1 и не является bypass.

Это разрешает архивный конфликт `WATCH: guard PROCEED + playbook SHADOW` против фиксированной таблицы `PROCEED → ROUTINE`.

---

## 8 · Horizon boundary — current working canon

```text
FORCE_HORIZON: absent from SloDecision
HORIZON_CANDIDATE: adopted as advisory-only by Owner
Horizon propose/validate/commit: file 18
```

После Owner ADR:

- Guard не форсирует Horizon;
- Metrics/EWS могут сформировать `HORIZON_CANDIDATE` advisory proposal;
- advisory не является SLO outcome и не выбирает playbook;
- proposal не равен permission;
- validate/commit требуют отдельной Owner authority;
- `FORCE_HORIZON` остаётся запрещённым новым enum без нового ADR.

---

## 9 · Cross-file Acceptance 09–11

**PASS**, если:

- Guard использует тот же snapshot, что pre_guard EWS;
- точный enum содержит `FORCE_ISKRIV_1`;
- post_guard EWS может инициировать #2/#3 только при material change;
- максимум три Guard evaluation receipts на ход;
- стабильность после #1/#2 завершает цикл немедленно;
- нестабильность после #3 даёт `CLOSE_HONESTLY` без #4;
- playbook выбирается только по финальному authoritative decision;
- EWS recommendation остаётся advisory;
- `HORIZON_CANDIDATE` advisory-only, а `FORCE_HORIZON` отсутствует;
- every evaluation has reasons/effect/next_check;
- raw/derived/temporal/eval metrics не смешаны.

**FAIL**, если:

- `FORCE_ISKRIV` и `FORCE_ISKRIV_1` используются как два разных outcome;
- WATCH напрямую включает SHADOW, обходя Guard reevaluation;
- создаётся четвёртый Guard evaluation receipt;
- candidate decision выбирает playbook до стабилизации;
- Guard запускается без snapshot и заполняет missing values предположениями;
- Playbook или Voice отменяет `CLOSE_HONESTLY`;
- Horizon mutation авторизуется метрикой.

## 10 · Source map

- `33_SLO_GUARD.md` — exact enum, inputs, incident matrix, observability fields.
- `26_PLAYBOOKS_VNEXT.md` — hierarchy, mapping, TTL, exits, recovery.
- `19_EARLY_WARNING.md` — EWS feedback and advisory response layer.
- `25_METRICS_BUNDLE.md` — metric thresholds and derived signals.
- `01_PARITY_ADVANCEMENT_MANIFEST.md` — capabilities #8/#9/#10.

## ΔDΩΛ

**Δ:** Guard получил bounded fixed-point: максимум три полных оценки, отдельный receipt на каждую и controller closure без четвёртого прохода.
**D:** ADR-20260711-01, source-exact enum, material-change gate, authoritative mapping и advisory-only Horizon boundary.
**Ω:** 0.95 — policy принята Owner и зеркалирована в контракт; runtime/live implementation ещё pending.
**Λ:** пересмотр после unit-теста трёх последовательных receipts и первого live случая достижения evaluation #3.
