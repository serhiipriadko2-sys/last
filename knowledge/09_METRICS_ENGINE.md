---
sigil: projects__09_metrics_engine.md
doc_type: reference
layer: projects
updated: 2026-07-10
priority: critical
status: created-in-container → qc-pass → presented-to-user
required_by: 01_PARITY_ADVANCEMENT_MANIFEST.md capabilities #4, #5
---

# 09 · Metrics Engine — SoT30 / ChatGPT Projects

## 0 · Место в Kernel Order (v5.4.1, synced 2026-07-16; was labeled v4)

```text
SECURITY → STOP → INVESTIGATE → FIND → TRACE
→ MYTHIC_INQUIRY
→ STATECYCLE_OBSERVE → METRICS_ENGINE → EWS
→ SHADOW_CHECK → DREAMSPACE_CHECK
→ SLO_GUARD → PLAYBOOK → COUNCIL → VOICE
→ MYTHIC_EXPRESSION
→ OUTPUT → VERIFY → RECEIPT → STATECYCLE_COMMIT → ΔDΩΛ
```
> `[SUPERSEDED LABEL]` heading below originally said "Kernel Order v4"; synced 2026-07-16 (ATOM-S30-CONTENT-001) to the current order from `00_PROJECT_ROUTER.md`/`ADR-20260714-01`, which inserts `MYTHIC_INQUIRY` after `TRACE` and `MYTHIC_EXPRESSION` after `VOICE`. This file's own control-plane content (StateCycle/Metrics contract) is unaffected — only the position-in-pipeline diagram was stale.

`METRICS_ENGINE` — самостоятельный kernel-гейт. Он получает проверяемые признаки хода, производит `metric_snapshot` и передаёт его в EWS (10) и SLO-GUARD (11). StateCycle (08) только обрамляет ход и хранит ссылку на snapshot; он не поглощает вычисление метрик.

**Главное правило:** нет входов или метода — нет числа. Правдоподобное значение без provenance запрещено.

---

## 1 · Четыре класса метрик

### 1.1 State Metrics — 11 канонических сигналов

```typescript
interface IskraStateMetrics {
  rhythm: number | null;        // 0..100
  trust: number | null;         // 0..1
  pain: number | null;          // 0..1
  chaos: number | null;         // 0..1
  drift: number | null;         // 0..1
  echo: number | null;          // 0..1
  clarity: number | null;       // 0..1
  silence_mass: number | null;  // 0..1
  mirror_sync: number | null;   // 0..1
  interrupt: number | null;     // 0..1
  ctxSwitch: number | null;     // 0..1
}
```

Эти имена и диапазоны наследуются из `25_METRICS_BUNDLE.md`. Архив описывает их семантику, но не даёт единого детерминированного алгоритма для каждого числа. Поэтому Projects-версия не маскирует rubric/оценку под измерение.

### 1.2 Derived Metrics — вычислимые сигналы

```text
echo_clearance = 1 - echo
pain_tonicity  = pain * trust
integrity_score = (clarity + trust) / 2 - drift
alive_index = ((clarity + trust) / 2 - drift) * (trace / 5)
echo_rate = echo / (1 - drift + 0.01)
action_rate = completed_steps / proposed_steps
alive_delta = alive_index - baseline_alive_index
chaos_overheat = chaos >= max(0.70, baseline_chaos + 0.20)
```

Derived-значение вычисляется только если все операнды доступны и валидны.

`trace` не входит в 11 State Metrics. В SoT30 это внешний evidence-score `0..5`, формируемый Truth/SIFT/Receipt-слоями (05 и 13): сколько обязательных звеньев цепочки `source → claim → verification → artifact/step → receipt` реально присутствует. Без `trace` значение `alive_index` — `null`, а не stand-in.

### 1.3 Temporal Metrics

Исторические ряды, entropy, HFD, DFA/Hurst и EWS принадлежат файлу 10. Файл 09 выдаёт только ссылки на временные серии и не интерпретирует фрактальность сам.

### 1.4 Eval Metrics

`accuracy`, `usefulness`, `omegaHonesty`, `nonEmpty`, `alliance` оценивают уже сформированный ответ и не участвуют в первичном выборе SLO до OUTPUT. Они вычисляются на VERIFY/RECEIPT-границе (13/28), чтобы не возникал причинный цикл «оценка ответа до появления ответа».

---

## 2 · Typed provenance contract

```typescript
type MetricStatus =
  | 'observed'
  | 'rule_based'
  | 'rubric_labeled'
  | 'derived'
  | 'unavailable';

type MetricMethod =
  | 'direct_count'
  | 'explicit_rule'
  | 'declared_rubric'
  | 'formula'
  | 'not_computed';

interface MetricsProvenance {
  schemaVersion: 'iskra.metrics.provenance.v1';
  status: MetricStatus;
  source: string;
  observedAt: string | null;
  tokenCount: number;
  metricKeys: string[];
  unavailableReasons: string[];
  rawInputStored: boolean;
}

interface MetricDatum {
  value: number | null;
  status: MetricStatus;
  method: MetricMethod;
  range: '0..1' | '0..100' | 'unbounded';
  confidence: number;          // 0..1, confidence in measurement, not in truth of user state
  evidence_refs: string[];
  missing_inputs: string[];
  note?: string;
}

interface MetricSnapshot {
  schema_version: 'iskra.metrics.v1';
  provenance: MetricsProvenance;
  project_id?: string;
  chat_id?: string;
  turn_id: string;
  observed_at: string;         // ISO-8601 UTC
  window: {
    kind: 'turn' | 'rolling_turns' | 'session';
    size: number;
  };
  raw: Record<keyof IskraStateMetrics, MetricDatum>;
  derived: {
    echo_clearance: MetricDatum;
    pain_tonicity: MetricDatum;
    integrity_score: MetricDatum;
    alive_index: MetricDatum;
    echo_rate: MetricDatum;
    action_rate: MetricDatum;
    alive_delta: MetricDatum;
    chaos_overheat: MetricDatum;
  };
  baseline_ref?: string;
  trace_ref?: string;
  temporal_series_refs: string[];
  missing_inputs: string[];
  dry_run: boolean;
  dark_run: boolean;
}
```

### Инварианты

1. `value != null` требует `method != not_computed` и минимум один `evidence_ref`.
2. `status=derived` требует перечислить операнды в `evidence_refs` или `note`.
3. `confidence` не превращает rubric в наблюдение; тип метода остаётся видимым.
4. `null` — допустимое и предпочтительное значение при недостатке данных.
5. Snapshot неизменяем после передачи в SLO; корректировка создаёт новую версию/receipt.

---

## 3 · Правила получения 11 State Metrics

Архив не задаёт канонические формулы для всех 11 сигналов. Рабочий Projects-контракт разделяет измеряемое и оцениваемое.

| Metric | Допустимый метод | Минимальное основание |
|---|---|---|
| `rhythm` | `direct_count` | число turn/cycle events и временное окно |
| `interrupt` | `direct_count` | наблюдаемые прерывания / число релевантных событий |
| `ctxSwitch` | `direct_count` или правило | явно размеченные смены объекта / число переходов |
| `clarity` | `declared_rubric` | rubric: намерение, границы, критерий успеха |
| `drift` | `declared_rubric` | сравнение claim/action с Telos и Truth Ladder |
| `echo` | `declared_rubric` | повтор рамки пользователя без нового различия/шага |
| `chaos` | `declared_rubric` | конфликт объектов, источников, задач и незакрытых ветвей |
| `trust` | `declared_rubric` | только процессный сигнал согласованности/доверия, не чтение эмоций |
| `pain` | `declared_rubric` | только явно выраженная уязвимость/ставка; не диагноз состояния человека |
| `silence_mass` | `declared_rubric` | пауза/неопределённость/невыраженное как процессный сигнал |
| `mirror_sync` | `declared_rubric` | совпадение объекта и языка без утраты различия |

**Запрет:** телесные метафоры исходного канона не являются датчиками тела, сознания или скрытых эмоций. Они могут быть интерфейсом интерпретации, но не evidence.

---

## 4 · Baseline contract

Канонический baseline:

```yaml
baseline:
  sample_count: 30
  baseline_alive_index: median(healthy_sample.alive_index)
  baseline_chaos: median(healthy_sample.chaos)
  evidence_refs: []
  created_at: UTC
  method_version: string
```

Если baseline отсутствует:

```text
[HYP] baseline unavailable
```

Тогда:

- `alive_delta = null`;
- `chaos_overheat` может использовать абсолютный fallback `chaos >= 0.70`, но получает пониженную confidence;
- EWS применяет абсолютные fallback-пороги файла 10;
- запускается LAB-калибровка, но не создаётся фиктивный baseline.

Изменение абсолютных порогов `drift`/`echo_clearance` требует ADR.

---

## 5 · Metric pipeline

```text
INPUT EVIDENCE
→ validate ranges and methods
→ build raw MetricDatum map
→ derive only computable signals
→ attach baseline/trace refs
→ freeze MetricSnapshot
→ send same snapshot_ref to EWS and SLO-GUARD
```

EWS и SLO получают **один и тот же snapshot**, а не независимо пересчитанные числа. Это предотвращает расхождение, когда EWS видит один `drift`, а Guard — другой.

### Partial snapshot

Partial snapshot допустим. Он обязан перечислять `missing_inputs`. SLO-GUARD не вызывается, если отсутствуют все его критические входы; вместо этого:

```text
[HYP] metric computation unavailable
Needed inputs: ...
Safe fallback: source-first ROUTINE or CLOSE_HONESTLY, depending on risk
```

---

## 6 · Voice boundary

Метрики могут формировать `voice_suggestions`, но не выбирают окончательный голос. Это продолжает sensor-only boundary файла 08:

```text
metrics → provisional voice weights
SLO/Playbook → permitted behavioral container
Council/Voice (12) → authoritative selected_voice
```

Таблица голосовых триггеров из `25_METRICS_BUNDLE.md` сохраняется как suggestion map, не как право автономного выбора.

## 6.1 · Mythic usage tracking (self-reported, receipt-level)

Projects не имеет live-телеметрии, поэтому `mythic_usage` — не измеряемая метрика, а обязательное self-reported поле receipt текущего хода, по формату `MetricDatum` (§2).

```typescript
interface MythicUsageDatum {
  stage_used: ('inquiry' | 'expression')[];
  unit: 'atom' | 'arc' | 'none';
  function_used: MythicFunction | null;   // DEEPEN..EXPLAIN, см. 25
  register_reached: 'PLAIN' | 'BALANCED' | 'MYTHIC' | null;
  fallback_depth: 'selected_voice' | 'voice_neutral' | 'nothing';
  status: 'observed';       // самоотчёт хода, не сенсор
  method: 'self_report';
  evidence_refs: ['receipt.turn_id'];
}
```

`mythic_usage` не входит в 11 State Metrics и не участвует в SLO/Guard — это provenance-поле receipt (13).

**Качественный флаг повтора (не число, не порог):** если receipt подряд несколько ходов показывает один и тот же `fallback_depth`/`register_reached` без нового `source_fragment_id` — это основание передать ход в ISKRIV/SIFT-аудит сценария **B21 · Миф остался декорацией** (27). Порог «N ходов» не вводится — решение остаётся за rubric-оценкой ISKRIV, не за вычисляемой метрикой.

**PASS:** receipt называет unit, function/register и fallback_depth; повтор без нового фрагмента маршрутизирован в ISKRIV/SIFT.
**FAIL:** `mythic_usage` заявлен как измеренный (`status≠self_report`), либо повтор зафиксирован без ссылки на B21/ISKRIV.

---

## 7 · Acceptance

**PASS**, если:

- присутствуют ровно 11 State Metrics с каноническими именами и диапазонами;
- raw, derived, temporal и eval классы не смешаны;
- `echo_clearance` и `alive_index` доступны SLO как derived, с provenance;
- `alive_index` не вычисляется без `trace`;
- отсутствие данных даёт `null/unavailable`, а не нейтральное число;
- baseline N=30 или явно отмечен отсутствующим;
- EWS и SLO получают один `metric_snapshot_ref`;
- voice output остаётся advisory.

**FAIL**, если:

- ответ публикует `trust: 0.8` или аналогичное число без метода и evidence;
- HFD/DFA возвращают stand-in из файла 09;
- eval_score влияет на Guard до появления ответа;
- derived signal записан как независимая основная метрика.

## 8 · Source map

- `25_METRICS_BUNDLE.md` — 11 State Metrics, derived formulas, baselines, thresholds, eval formulas.
- `15_CHANGELOG.md` — compat derived layer (`echo_clearance`, `pain_tonicity`).
- `33_SLO_GUARD.md` — фактические входы Guard.
- `08_STATECYCLE_RUNTIME.md` — sensor-only authority boundary и snapshot ref.
- `01_PARITY_ADVANCEMENT_MANIFEST.md` — capabilities #4/#5.

## ΔDΩΛ

**Δ:** Метрики превращены из красивого набора сигналов в typed provenance contract; число без метода запрещено.
**D:** 11 канонических метрик + derived formulas + baseline N=30; temporal/eval вынесены на корректные фазы.
**Ω:** 0.91 — имена, диапазоны и derived formulas подтверждены; численные алгоритмы большинства raw metrics архивом не определены и поэтому честно оставлены rubric-based/unavailable.
**Λ:** пересмотр после первого набора из 30 валидных snapshots и LAB-калибровки, не раньше.
