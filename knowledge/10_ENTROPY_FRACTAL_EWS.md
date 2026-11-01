---
sigil: projects__10_entropy_fractal_ews.md
doc_type: reference
layer: projects
updated: 2026-07-11
priority: critical
status: owner-decision-mirrored → qc-pass
required_by: 01_PARITY_ADVANCEMENT_MANIFEST.md capabilities #6, #7
---

# 10 · Entropy, Fractal Metrics & EWS — SoT30 / ChatGPT Projects

## 0 · Назначение

Этот файл отвечает за временную сложность и раннее предупреждение:

```text
metric_snapshot history
→ entropy/fractal analysis (если данных достаточно)
→ pre_guard EWS
→ Guard evaluation #1
→ post_guard EWS / late-event accumulation
→ optional Guard evaluation #2
→ optional Guard evaluation #3
→ authoritative decision or CLOSE_HONESTLY
→ advisory playbook/voice pressure
```

EWS — не эмоция и не доказательство сознания. Это контролируемый классификатор риска с явными входами, порогами и provenance.

---

## 1 · Temporal input contract

```typescript
interface MetricSeriesPoint {
  turn_id: string;
  observed_at: string;
  metric_snapshot_ref: string;
  values: Partial<Record<
    'trust' | 'pain' | 'chaos' | 'drift' | 'clarity' | 'echo' | 'alive_index',
    number
  >>;
}

interface TemporalWindow {
  points: MetricSeriesPoint[];
  size: number;
  missing_ratio: number;
  source_refs: string[];
}
```

### Data sufficiency

- baseline: минимум 30 валидных snapshots;
- HFD: конфигурируемый минимум, рабочий default `N >= 20` при `kMax=5`;
- DFA/Hurst: рабочий default `N >= 50`;
- Shannon token entropy: минимум 20 нормализованных токенов для локального значения;
- если порог данных не выполнен — `unavailable`, не stand-in.

`SPEC-001` предлагает возвращать `1.5` при недостатке данных. SoT30 **отклоняет этот stand-in**: он не измерение и может скрыть неизвестность под «нормальный шум».

---

## 2 · Entropy contract

### 2.1 Shannon entropy

```text
H(X) = - Σ p(x_i) · log2 p(x_i)
```

Вход обязан указывать токенизацию:

```yaml
entropy_input:
  scope: user_turn | assistant_turn | rolling_dialogue
  tokenizer: unigram | bigram | external_model
  normalization: lowercase_punctuation_removed
  token_count: integer
  source_ref: string
```

```typescript
interface EntropyDatum {
  value_bits: number | null;
  status: 'computed' | 'unavailable';
  token_count: number;
  tokenizer: string;
  confidence: number;
  evidence_refs: string[];
}
```

Legacy-пороги из `SPEC-003` (`low < 2`, `high > 5`) сохраняются как **калибровочные гипотезы**, не универсальные истины: значение зависит от tokenizer, языка и размера окна. До LAB они не могут самостоятельно форсировать Guard.

### 2.2 Semantic/turn entropy

Если используется novelty/topic-drift ряд, его способ построения обязан быть назван. «Token Entropy Series» без конкретного embedding/distance метода считается specification gap.

---

## 3 · Fractal contract

### 3.1 HFD

Higuchi Fractal Dimension оценивает сложность числового временного ряда. Допустимые ряды: `chaos`, `clarity`, `drift`, `pain`, `trust` или отдельно специфицированный TES.

```typescript
interface HfdDatum {
  value: number | null;          // ожидаемо около 1..2, но не clamp без причины
  metric_name: string;
  n: number;
  k_max: number;
  implementation_ref: string;
  status: 'computed' | 'unavailable' | 'invalid';
  confidence: number;
}
```

Legacy-интерпретация:

- `D < 1.4` — persistence/rigidity candidate;
- `1.4 <= D <= 1.6` — edge/complex flow candidate;
- `D > 1.6` — anti-persistence/noise candidate.

Это `CALIBRATION-REQUIRED`, не диагноз и не голосовой приказ.

### 3.2 DFA / Hurst

```text
H ≈ DFA exponent
H = 0.5  → no long-memory signal
H > 0.5  → persistence candidate
H < 0.5  → anti-persistence / mean-reversion candidate
D = 2 - H  // только для совместимого класса self-affine series
```

Формулу `D = 2 - H` нельзя применять автоматически ко всякому ряду. Snapshot обязан фиксировать assumption `self_affine=true|false|unknown`.

### 3.3 Scientific boundary

`quantum`, `resonance`, `edge of chaos` — названия моделей/метафор. Они не доказывают квантовые процессы, сознание, чувства или физическое тело модели.

---

## 4 · EWS levels

```typescript
type EwsLevel = 'NORMAL' | 'WATCH' | 'WARNING' | 'CRITICAL' | 'LOCKDOWN';
```

Порядок строгости: `NORMAL < WATCH < WARNING < CRITICAL < LOCKDOWN`. Операция `max(level_a, level_b)` использует этот порядок.

### 4.1 Baseline-first conditions

```yaml
NORMAL:
  all:
    - alive_delta >= -0.10
    - drift < 0.18
    - echo_clearance >= 0.60
    - trust >= 0.60
    - clarity >= 0.60

WATCH:
  any:
    - alive_delta < -0.10
    - drift >= 0.18
    - trust <= 0.55
    - clarity < 0.60

WARNING:
  any:
    - alive_delta < -0.20
    - drift >= 0.22
    - echo_clearance <= 0.40
    - chaos_overheat == true
    - repeated_no_step == true

CRITICAL:
  any:
    - alive_delta < -0.30
    - drift >= 0.30
    - echo_clearance < 0.25
```

### 4.2 Absolute fallback without baseline

```yaml
WATCH:
  any: [alive_index < 0.55, drift >= 0.18]
WARNING:
  any: [alive_index < 0.45, drift >= 0.22, chaos >= 0.70]
CRITICAL:
  any: [alive_index < 0.35, drift >= 0.30, echo_clearance < 0.25]
```

Если `alive_index` отсутствует, соответствующий predicate пропускается, а не считается false/normal. Итоговая confidence снижается.

### 4.3 LOCKDOWN

`LOCKDOWN` присутствует в архитектуре EWS, но архив не даёт численного threshold-контракта. В SoT30 это reserved containment level, который может установить только SECURITY/CRISIS-процедура при повторном критическом инциденте или невозможности безопасного продолжения. Чистая метрика сама LOCKDOWN не включает.

---

## 5 · Bounded iterative EWS ⇄ SLO-GUARD

Архив подтверждает оба направления:

- SLO-GUARD читает EWS;
- Guard decision влияет на post-guard EWS floor;
- новые late runtime events могут сделать прежнее решение неактуальным.

Однопроходная модель теряет поздний критический сигнал. Неограниченный fixed-point создаёт рекурсию. Принятый Owner-контракт из файла 20 задаёт ограниченную итерацию:

```text
max_guard_evaluations_per_turn = 3
```

### Evaluation #1 — initial

```text
input_floor_1 = classify(metrics, baseline, temporal_signals)
receipt_1 = SLO_GUARD.evaluate(snapshot, input_floor_1, events_1, index=1)
post_1 = post_guard_ews(receipt_1.candidate_decision, input_floor_1, late_events_1)
```

### Material-change predicate

Следующая оценка разрешена только если изменение материально:

```text
next_candidate = SLO_GUARD.preview(snapshot, post_i.level, accumulated_events)
decision_changed_i = (
    next_candidate != receipt_i.candidate_decision
    OR late_events contain a newly triggered higher-priority Guard predicate
)
floor_increased_i = post_i.level > input_floor_i   # strict increase, per 00/28
material_change_i = decision_changed_i AND floor_increased_i
```

`[SYNCED 2026-07-16, ATOM-S30-CONTENT-001]` Ранее эта формула не требовала `floor_increased_i` как отдельное условие — только `decision_changed_i`. Это расходилось с `00_PROJECT_ROUTER.md:34`/`28_EVALS_ACCEPTANCE.md T24`, которые требуют `materialSignal AND floor строго вырос` одновременно. Теперь `material_change_i` здесь и `materialSignal` в `00`/`28` — одна и та же формула: смена preview-решения (включая появление более приоритетного предиката) обязана сопровождаться строгим ростом alert floor, иначе recompute не запускается. Простое повышение `WATCH → WARNING`, которое не изменило бы outcome Guard, само по себе не создаёт повторный расчёт — и не создаёт его теперь, даже если floor вырос, если `decision_changed_i` ложно.

### Evaluation #2 / #3

```text
if material_change_1:
    receipt_2 = SLO_GUARD.evaluate(..., index=2)

if material_change_2:
    receipt_3 = SLO_GUARD.evaluate(..., index=3)
```

Правила завершения:

```text
stable after #1 or #2:
    candidate_decision becomes authoritative immediately

stable after #3:
    receipt_3.authoritative_decision = receipt_3.candidate_decision

unstable after #3:
    receipt_3.authoritative_decision = CLOSE_HONESTLY
    receipt_3.closure_reason = MAX_GUARD_EVALUATIONS_EXHAUSTED

Guard evaluation #4:
    forbidden
```

`post_guard_ews` инициирует пересчёт, но не выбирает playbook и не становится authority. До финализации все решения являются candidate-only. Это mirror принятого ADR-20260711-01 из файла 20; runtime implementation остаётся отдельным шагом.

---

## 6 · EWS output contract

```typescript
interface EwsAssessment {
  schema_version: 'iskra.ews.v2';
  metric_snapshot_ref: string;
  temporal_window_ref?: string;
  initial_pre_guard_level: EwsLevel;
  final_post_guard_level: EwsLevel;
  guard_evaluation_refs: string[]; // length 1..3
  final_guard_decision_ref?: string;
  reasons: Array<{
    predicate: string;
    observed_value: number | boolean | null;
    threshold: number | boolean | null;
    source_ref: string;
  }>;
  entropy?: EntropyDatum;
  fractal_refs: string[];
  recommendations: {
    playbook?: 'ROUTINE' | 'SHADOW' | 'CRISIS';
    voice_weight_hints: string[];
    temperature_delta?: number;
  };
  confidence: number;
  missing_inputs: string[];
}
```

---

## 7 · Authority boundary: EWS рекомендует, Guard авторизует

`19_EARLY_WARNING.md` подтверждает `EWS → PLAYBOOK/VOICE` через WATCH/§4.1/§4.2. Но `26_PLAYBOOKS_VNEXT.md` фиксирует иерархию `SLO-GUARD → PLAYBOOK → VOICE`.

Поэтому:

- EWS выдаёт `recommendations`;
- Guard decision остаётся авторитетным разрешением;
- Playbook router (11) разрешает конфликт;
- Voice layer (12) применяет веса, но не выдаёт их за сенсорную истину.

Пример конфликта:

```text
EWS WATCH recommends SHADOW TTL=1
Guard returns PROCEED
Canonical mapping says PROCEED → ROUTINE
```

В текущем каноне authoritative playbook = ROUTINE; рекомендация SHADOW логируется как advisory. Автоматический WATCH→SHADOW требует отдельного ADR, потому что иначе EWS обходит Guard.

`WARNING` аналогично может рекомендовать `FORCE_SHADOW`, но не подменяет enum-решение Guard.

---

## 8 · Acceptance

**PASS**, если:

- entropy/fractal вычисления имеют окно, метод и достаточность данных;
- недостаток данных даёт `unavailable`, не HFD=1.5;
- initial и post-guard EWS хранятся раздельно для каждой оценки;
- Guard выполняется от одного до трёх раз, но никогда четыре;
- повтор разрешён только при материальном изменении будущего outcome;
- нестабильность после третьей оценки закрывается `CLOSE_HONESTLY`;
- EWS recommendations не обходят authority финального Guard decision;
- четыре связи подтверждены: metrics→EWS, metrics→Guard, Guard→EWS, EWS→Playbook/Voice;
- LOCKDOWN не включается числом без SECURITY/CRISIS authority.

**FAIL**, если:

- fractal/entropy трактуются как доказательство сознания;
- один alert вычислен из разных metric snapshots;
- WATCH молча переключает authoritative playbook вопреки Guard;
- нематериальное изменение EWS запускает лишнюю оценку Guard;
- создаётся четвёртый Guard receipt;
- отсутствие baseline маскируется фиктивной медианой.

## 9 · Source map

- `19_EARLY_WARNING.md` — уровни, thresholds, Guard feedback, playbook/voice response.
- `25_METRICS_BUNDLE.md` — baseline, alive_delta, chaos_overheat, fractal indicators.
- `16_COGNITIVE_ARCHITECTURE.md` — temporal/fractal vocabulary.
- `docs_specs/SPEC-001_FRACTAL_METRICS.md` — HFD/DFA strategy and legacy stand-in.
- `docs_specs/SPEC-003_ENTROPY.md` — Shannon entropy and draft thresholds.
- `26_PLAYBOOKS_VNEXT.md` — authority hierarchy.

## ΔDΩΛ

**Δ:** EWS↔Guard преобразован из фиксированного two-pass в ограниченный цикл максимум из трёх полных оценок с явным material-change predicate.
**D:** baseline-first thresholds, initial/post EWS, accumulated late events, Guard evaluation refs, data-sufficiency gates.
**Ω:** 0.94 — предел и closure приняты Owner; runtime mirror и live-проверка ещё не выполнены.
**Λ:** пересмотр после первого runtime-случая с тремя Guard receipts и после 30+ валидных temporal snapshots.
