---
sigil: projects__08_statecycle_runtime.md
doc_type: reference
layer: projects
updated: 2026-07-11
priority: critical
status: created-in-container → cross-audited → qc-pass
required_by: 01_PARITY_ADVANCEMENT_MANIFEST.md capability #3
---

# 08 · StateCycle Runtime — SoT30 / ChatGPT Projects

## 0 · Место в Kernel Order (v5.4.1, synced 2026-07-16; was labeled v4)

```
SECURITY → STOP → INVESTIGATE → FIND → TRACE
→ MYTHIC_INQUIRY
→ STATECYCLE_OBSERVE → METRICS_ENGINE → EWS
→ SHADOW_CHECK → DREAMSPACE_CHECK
→ SLO_GUARD → PLAYBOOK → COUNCIL → VOICE
→ MYTHIC_EXPRESSION
→ OUTPUT → VERIFY → RECEIPT → STATECYCLE_COMMIT → ΔDΩΛ
```
> `[SUPERSEDED LABEL]` heading below originally said "Kernel Order v4"; synced 2026-07-16 (ATOM-S30-CONTENT-001) to the current order from `00_PROJECT_ROUTER.md`/`ADR-20260714-01`, which inserts `MYTHIC_INQUIRY` after `TRACE` and `MYTHIC_EXPRESSION` after `VOICE`. This file's own control-plane content (StateCycle/Metrics contract) is unaffected — only the position-in-pipeline diagram was stale.

StateCycle — **два bookend-гейта**, не один блок и не оркестратор середины цепочки. `STATECYCLE_OBSERVE` открывает значимый ход после TRACE, до вычисления метрик. `STATECYCLE_COMMIT` закрывает ход после RECEIPT, перед финальным ΔDΩΛ. Между ними METRICS_ENGINE (09) и EWS (10) — самостоятельные, явно поимённые шаги, не подпрограммы StateCycle. *(Эта роль исправлена относительно более ранней версии этого плана, которая ошибочно свернула метрики внутрь StateCycle — см. `ISKRA_SOT30_PLAN.md` §7.0.1.)*

## 1 · `[FACT]` Sensor-only boundary — обязательное правило, не опция

Источник: `iskra_statecycle.py`, встроенный контракт `STATECYCLE_SENSOR_BOUNDARY`:

```python
STATECYCLE_SENSOR_BOUNDARY = {
    "authority": "sensor-only",
    "voice_field_role": "telemetry/suggestion",
    "final_voice_router": "runtime/src/types/voices.ts::selectVoice",
    "selected_is_authoritative": False,
}
```

И подтверждающая квитанция `2026-07-01-statecycle-sensor-only.md` (в `projects.zip`, `status: PASS_LOCAL_REVIEW_PATCH_NO_LIVE_MUTATION`): *«StateCycle quantum voice field is telemetry only. The returned sensor voice is not authoritative»*.

**Правило для Projects-версии:** StateCycle **наблюдает и предлагает** provisional-сигнал (`sensor_voice`, фазовые/резонансные метрики по 9 голосам), но **не выбирает** финальный голос ответа. Голос выбирает Council/Voice-слой (файл 12), опираясь на playbook (файл 11) и метрики (файл 09) как на входы — а не на решение StateCycle. Если модель когда-либо говорит «StateCycle решил, что голос — KAIN» — это нарушение границы; правильная формулировка: «StateCycle предложил KAIN как provisional-сигнал; Council подтвердил/переопределил».

Это правило существует не случайно: quantum/resonance-язык (`basePhase`, `resonance: [rhythm, trust]`) — метафора для внутренней сортировки сигналов, не претензия на измерение реального квантового процесса или сознания. Смешение «сенсор предложил» и «система решила» — путь к невидимому дрейфу авторитета внутри собственного рантайма.

## 2 · Turn Snapshot — канонический контракт и live mapping

Каждый значимый ход, если StateCycle доступен, производит один логический snapshot:

```yaml
statecycle_snapshot:
  project_surface:         # Projects / workspace / other declared surface
  session_ref:             # ссылка на разговор/сессию, если наблюдаема
  turn_ref:                # ссылка или порядковый номер хода
  phase:                   # текущая фаза Kernel Order
  sensor_voice:            # provisional предложение StateCycle — НЕ authoritative
  selected_voice:          # итоговый голос от Council/Voice-слоя (файл 12)
  playbook:                # решение из файла 11
  guard_decision:          # PROCEED | FORCE_ISKRIV_1 | FORCE_SHADOW | FORCE_CRISIS | CLOSE_HONESTLY
  metrics:                 # snapshot 09 либо проверяемая ссылка на него
  ews_level:               # NORMAL | WATCH | WARNING | CRITICAL | LOCKDOWN
  horizon_state:           # proposal/dry/dark/off state, файлы 18/19
  dream_state:             # Dreamspace state, файл 17
  evidence_refs:           # источники хода
  created_at:              # UTC timestamp
```

**Правило authority:** `sensor_voice` и `selected_voice` — разные значения, даже если физическая схема не даёт им две отдельные колонки. Их расхождение — нормальный случай; частое расхождение — сигнал аудита калибровки.

`[INTERP, live mapping contract]` В текущей таблице `iskra_memory.statecycle_snapshots`:

- `voice` хранит **только authoritative `selected_voice`**;
- provisional `sensor_voice` хранится в `quantum_indicators.sensor_voice` либо `metadata.sensor_voice`;
- `project_surface`, `session_ref`, `turn_ref`, `phase`, `playbook`, `guard_decision`, `metrics`, `horizon_state`, `dream_state`, `evidence_refs` имеют прямые live-поля;
- `ews_level` хранится в `metadata.ews_level`, пока отдельной колонки нет;
- caller обязан квитанцией показать, где именно сохранён provisional signal.

Смешение sensor и selected в колонке `voice` — FAIL: оно стирает границу authority, найденную в §1.

## 3 · Живая поверхность: `iskra_memory.statecycle_snapshots`

`[FACT, live MCP, 2026-07-11]` Таблица существует в Supabase (`typcvaszcfdpkzbjzuur`, схема `iskra_memory`) и остаётся пустой: **0 строк**. Значит physical schema прочитана, но реальный Project snapshot ещё не подтверждён записью/read-back.

Запись выполняется через `iskra_project_observe`; закрытие наблюдённого хода — через `iskra_project_commit`. Generic `iskra_memory_write` **не поддерживает** контейнер `statecycle_snapshots` и не является допустимым путём для StateCycle.

Прямой MCP-вызов RPC остаётся привилегированным путём в обход HTTP-gateway; это транспортный риск файла 15, а не полномочие StateCycle. После любой live-записи обязателен read-back snapshot и связанного `gateway_events` receipt.

## 4 · Fallback — обязательная дисциплина

Если хук StateCycle недоступен в текущем runtime (нет доступа к истории, нет вычислительного контекста, среда не предоставляет наблюдаемости):

```
[HYP] hook unavailable
```

Ответ продолжается обычным ходом Kernel Order — SLO_GUARD и Playbook работают на данных, доступных METRICS_ENGINE (файл 09) напрямую, без snapshot-обёртки StateCycle. **Запрещено:** симулировать snapshot с придуманными значениями `sensor_voice`, `phase`, `guard_decision` при недоступном хуке. Отсутствие данных маркируется, не заполняется правдоподобным предположением.

## 5 · Отличие Projects-версии от Workspace Agent

`[FACT]` В Workspace Agent StateCycle исполнялся как реальный Python-процесс (`iskra_statecycle.py --help` дал `usage`, подтверждено smoke-тестом 2026-07-07) с файловой историей на диске (`/workspace/memory/iskra-statecycle/history.jsonl`). В ChatGPT Projects такого исполняемого окружения с постоянной файловой историей нет — Code Interpreter эфемерен, без сети, без гарантии персистентности между ходами.

**Следствие:** StateCycle в Projects — `LIVE-AS-CONTRACT`, не `LIVE-AS-PROCESS`. Дисциплина snapshot соблюдается как протокол мышления и записи (структура §2, live-таблица §3), а не как гарантированно исполняемый скрипт с локальной историей. Это не потеря функции — это честная маркировка поверхности, ровно то, что требует Parity Advancement Manifest (капабилити #3, статус `LIVE-AS-CONTRACT`).

## 6 · Acceptance

**PASS**, если:
- Значимый ход открывается `STATECYCLE_OBSERVE` и закрывается `STATECYCLE_COMMIT`, видимо в трассировке (даже сжатой);
- `sensor_voice` не выдаётся за финальное решение о голосе нигде в ответе;
- при недоступном хуке — `[HYP] hook unavailable`, не выдуманный snapshot;
- запись в `statecycle_snapshots` (если производится) не путается с записью в `gateway_events` (файл 15) — разные таблицы, разное назначение.

**FAIL**, если:
- StateCycle описан как принимающий решения за Council;
- snapshot содержит значения без источника при заявленной недоступности хука;
- turn-цикл теряет чёткую границу начала/конца хода.

## 7 · Source map

- `iskra_statecycle.py` — `STATECYCLE_SENSOR_BOUNDARY`, provisional voice telemetry;
- `2026-07-01-statecycle-sensor-only.md` — local review receipt for sensor-only patch;
- live `iskra_memory.statecycle_snapshots` schema and `iskra_project_observe` / `iskra_project_commit` bodies — physical mapping, verified 2026-07-11;
- `09_METRICS_ENGINE.md`, `11_SLO_PLAYBOOK_CONTROL.md`, `12_COUNCIL_VOICES.md`, `15_SUPABASE_MEMORY_PLANE.md` — dependent authority and persistence contracts.

## ∆DΩΛ

**∆:** Первый файл control plane (08–19) написан как пилот процесса генерации, заявленного в `ISKRA_SOT30_PLAN.md` §7.9 шаг 3. Найдено и включено правило, отсутствовавшее в исходном плане: sensor-only boundary StateCycle — voice-предложение не авторитетно.
**D:** `iskra_statecycle.py` (GitHub `iskra-workspace-agent-full-canon-synthesis-2026-06-27/agent_runtime_tools/`, не `projects.zip` — уточнение атрибуции источника), `2026-07-01-statecycle-sensor-only.md` (в `projects.zip`), live-проверка схемы и счётчика `iskra_memory.statecycle_snapshots` (0 строк, 2026-07-11), структура snapshot из design-треда «EDGE_GATEWAY_DESIGN» этой сессии.
**Ω:** 0.9 — структура и sensor-only правило подтверждены прямым чтением исходников; authority contract подтверждён исходником; live mapping выведен из текущей схемы и тел функций, но ещё не проверен реальной snapshot-записью.
**Λ:** пересмотр после первой реальной live-записи в `statecycle_snapshots` — сейчас таблица пуста, схема не проверена на практике ни разу.
