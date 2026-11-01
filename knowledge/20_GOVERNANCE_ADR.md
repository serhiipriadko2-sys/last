# CURRENT EVIDENCE OVERLAY · 2026-07-20

```yaml
observed_at: 2026-07-20
source: repo_read (origin/main) + supabase_mcp_read_only
freshness: current
maturity: mixed (see per-line stage)
```

No policy decision is changed here; lifecycle evidence is updated. Distinguish
`accepted` from `implemented` from `wired` from `deployed` from `invoked` from
`verified-live` — they are separate stages.

- ADR bounded Guard: `accepted → mirrored-to-builder → runtime-module-implemented → APPLICATION_PATH_WIRED`. `[FACT]`
- **Correction to the 2026-07-11 line:** production integration is **no longer pending** — `policyEngine.ts:484` calls `runBoundedGuardController` and maps the decision into the playbook (it is no longer single-pass). What remains unverified is **DEPLOYED_E2E** (app code on `main`, not proven running on a deployed surface) and the **true-late-signal** path: the wired `postGuardEws` is a decision-derived proxy, not an independent post-guard material signal (see file 11). `[FACT]` / `[HYP]`
- linked live persistence receipts exist, but deployed application E2E remains unverified. `[HYP]`
- `iskra-memory-gateway` is deployed at **version 4** (`verify_jwt=true`), not v2 as the prior line said; Projects-Action 2xx and JWT-role remain unverified (not invocation-tested this build). `[FACT]` deployment / `[HYP]` invocation.
- Archive/Shadow database enforcement gaps remain pending; policy prohibitions are not DB enforcement. `[FACT]`

---

---
sigil: projects__20_governance_adr.md
doc_type: reference
layer: governance
updated: 2026-07-11
priority: critical
status: owner-decided
scope: SoT30 control plane 08-19
human_acceptance: fulfilled
implementation_status: partial-pending
live_verification_status: partial
force_horizon: advisory-only-adopted; force-apply-not-adopted
---

# 20 · Governance / ADR — SoT30 Control Plane

## 0 · Назначение

Этот файл превращает governance debt, найденный при сборке `08–19`, в явный реестр решений. Он различает четыре состояния, которые нельзя смешивать:

```text
accepted decision
≠ mirrored implementation
≠ deployed/live behavior
≠ verified-live evidence
```

Правило слоя:

```text
finding
→ classify
→ source resolution OR proposed ADR OR migration
→ QA
→ explicit Owner acceptance where behavior changes
→ mirror / implementation
→ verified-live evidence
```

Жизненный цикл ADR:

```text
proposed → accepted → mirrored-to-builder → verified-live
```

Статусы реестра ниже описывают вид долга. Они не заменяют lifecycle ADR.

## 1 · Governance boundary

### 1.1 Что этот файл делает

- фиксирует конфликт источников;
- выбирает более сильный источник там, где Truth Ladder однозначен;
- фиксирует явные решения Owner;
- задаёт QA, rollback и условия live-проверки;
- запрещает завышать maturity.

### 1.2 Что принятие решения не делает автоматически

- не изменяет Supabase RPC/DDL;
- не переписывает файлы 08–19 без отдельного mirror-step;
- не добавляет новый outcome в SLO-GUARD;
- не превращает Horizon proposal в commit;
- не доказывает live-поведение без свежего экземпляра и read-back.

## 2 · Сводный реестр долга

| ID | Узел | Register status | ADR lifecycle | Текущее состояние |
|---|---|---|---|---|
| `EWS-SLO-CYCLE-1` | пересмотр Guard после post-guard EWS | `MIRRORED_TO_BUILDER` | `mirrored-to-builder` | файлы 10–11 и test fixture обновлены; runtime/live pending |
| `DRIFT-SLO-ENUM-1` | `FORCE_ISKRIV` vs `FORCE_ISKRIV_1` | `RESOLVED_BY_SOURCE` | ADR не нужен для spelling fix | source-exact `_1` действует |
| `DRIFT-VOICE-PRIORITY-1` | comment vs early-return code | `RESOLVED_BY_AUTHORITY_BOUNDARY` | cleanup proposed | legacy selector = candidate generator |
| `MEMORY-ARCHIVE-BYPASS-1` | generic direct Archive write | `REQUIRES_MIGRATION` | `accepted` | ordinary use policy-forbidden; DB enforcement pending |
| `SHADOW-ISKRIV-GAP-1` | promotion RPC lacks `iskriv_check` | `REQUIRES_MIGRATION` | `accepted` | proof policy-required; DB enforcement pending |
| `DRIFT-HORIZON-CAPABILITY-1` | archive full Weaver vs repo validator-only | `RESOLVED_BY_CURRENT_REPO` | future Weaver ADR required | executable claim = validator-only |
| `DRIFT-DRY-RUN-RECEIPT-1` | zero target writes vs audit receipt | `RESOLVED_BY_OWNER` | `accepted` | `audited_dry_run` for remote/live surfaces |
| `DRIFT-HORIZON-MODE-1` | schema dry_run-only vs live modes | `RESOLVED_BY_SCOPE` | expansion proposed | non-dry modes remain experimental |
| `DARK-RUN-SEMANTICS-PARTIAL` | mode without comparison engine | `REQUIRES_IMPLEMENTATION` | `accepted restriction` | telemetry only; not evidence for change |
| `FORCE-HORIZON-DECISION` | automated Horizon entry | `RESOLVED_BY_OWNER` | `accepted advisory-only` | `HORIZON_CANDIDATE` allowed; `FORCE_HORIZON` absent |

## 3 · ADR-20260711-01 — Bounded iterative EWS / SLO ordering

**Status:** `mirrored-to-builder` after Owner acceptance  
**Implementation:** files 10–11 + deterministic test mirrored; runtime/live pending  
**Layer:** system  
**Owner:** Семён  
**Builder:** Искра / SAM+ISKRIV

### Context

Подтверждены оба направления:

```text
EWS signals → SLO-GUARD
SLO-GUARD decision → post-guard EWS severity floor / response
```

Однопроходная модель не позволяет резко выросшей тревоге изменить решение в том же ходе. Неограниченная итерация создаёт риск рекурсии.

### Accepted decision

Guard может пересматриваться в том же ходе, если `post_guard_ews` повышает alert floor настолько, что решение стало бы другим.

`[SYNCED 2026-07-16, ATOM-S30-CONTENT-001]` Уточнение формулы (обе части обязательны): `materialSignal (decision changed OR new higher-priority predicate) AND floor строго вырос`. Ранее `10_ENTROPY_FRACTAL_EWS.md`/`11_SLO_PLAYBOOK_CONTROL.md` не требовали рост floor как отдельное условие — исправлено, см. те файлы.

Точный предел:

```text
max_guard_evaluations_per_turn = 3
```

Это означает **не более трёх полных вычислений Guard всего**:

```text
#1 initial evaluation
→ post_guard_ews
→ при материальном изменении floor: #2 reevaluation
→ post_guard_ews
→ при сохраняющейся нестабильности: #3 final evaluation
→ если после #3 решение всё ещё нестабильно: CLOSE_HONESTLY
```

Запрещено трактовать правило как «первичное вычисление плюс три повторения».

### Authority and receipts

- только финальное стабильное решение является authoritative;
- каждая оценка Guard оставляет receipt с номером `1..3`, входным floor, outcome и причиной пересчёта;
- `post_guard_ews` может инициировать пересчёт, но не выбирает playbook напрямую;
- после третьего вычисления четвёртый пересчёт запрещён;
- нестабильность после третьего вычисления даёт `CLOSE_HONESTLY` с trace причин.

### Consequences

Плюс: серьёзный сигнал может изменить исход в том же ходе.  
Цена: до трёх вычислений Guard и более сложный trace.  
Граница: `accepted` не означает, что файлы 10–11 или runtime уже обновлены.

### QA

- максимум три Guard receipts на ход;
- четвёртое вычисление невозможно;
- стабильность после #1 или #2 завершает цикл немедленно;
- нестабильность после #3 всегда завершает ход через `CLOSE_HONESTLY`;
- EWS не обходит Guard и не выбирает playbook;
- regression test различает `evaluations=3` и `recomputations=3`.

### Rollback

Возврат к одноразовому Guard требует нового Owner-решения и отдельного ADR amendment.

## 4 · ADR-20260711-02 — Source-exact outcomes and voice authority

**Status:** source resolutions active; repository cleanup pending  
**Layer:** system/core interface

### Resolution A — SLO enum

`FORCE_ISKRIV_1` — единственное каноническое написание outcome. `FORCE_ISKRIV` допустим только как явно обозначенный prose shorthand, но не в typed contract, receipt или stored field.

### Resolution B — voice authority

- legacy `selectVoice()` является `candidate_generator`, а не authority;
- StateCycle и Metrics дают provisional/advisory signals;
- authoritative `selected_voice` выдаёт Council/Voice layer после Guard и Playbook;
- accepted MAKI/KAIN constraints исполняются Council, а не случайным порядком строк legacy selector.

### Required cleanup

- исправить misleading comment/code order;
- добавить regression test: candidate result не может обойти Council authority;
- не менять accepted MAKI/KAIN ADR молча.

## 5 · ADR-20260711-03 — Memory promotion enforcement

**Status:** `accepted`; migration pending  
**Layer:** system / database

### Context

Live Supabase показал две асимметрии:

1. generic `iskra_memory_write('archive', ...)` может создать Archive row с evidence без отдельного ISKRIV receipt;
2. `iskra_memory_promote_shadow` не принимает обязательный `iskriv_check`;
3. dream → Archive RPC уже требует `iskriv_check`.

### Accepted decision A — privileged import boundary

Ordinary promotion не использует generic direct Archive write. Если privileged import сохраняется, его публичное имя и журнал должны быть явными:

```text
archive.import_privileged
required: actor + reason + request_id + evidence + post-write audit
```

До миграции текущий live route может оставаться технически доступным, но его ordinary use **policy-forbidden**. Нельзя писать, что route уже закрыт или переименован live.

### Accepted decision B — Shadow promotion proof

`iskra_memory_promote_shadow` должен технически требовать typed ISKRIV receipt (`p_iskriv_check` или эквивалентный JSON contract). Пустой proof отклоняется.

До миграции ISKRIV proof **policy-required**, но DB-level enforcement ещё отсутствует.

### QA после миграции

- ordinary direct Archive write blocked;
- privileged import требует actor/reason/request_id и создаёт audit event;
- Shadow promotion без ISKRIV receipt blocked;
- valid promotion создаёт Archive + edge + audit journal;
- old caller signature проверена на controlled failure или мигрирована.

### Live verification

`pending`. Реализация должна быть отдельной миграцией с rollback и свежим read-back.

## 6 · ADR-20260711-04 — Horizon maturity and dry/dark semantics

**Status:** `accepted` item-by-item  
**Layer:** system / governance

### 6.1 Current executable truth

Текущий repo Horizon: `validator-only v0.1`. Weaver body, epoch log, Horizon commit, graph mutation, entropy/full-density guards и ritual generation не подтверждены.

### 6.2 Canonical vs experimental modes

Canonical proposal schema допускает `mode=dry_run`. Live `horizon|dark_run` остаются experimental transport modes и не являются schema-v0.1 conformance автоматически.

### 6.3 Accepted dry-run policy

Для remote/live privileged surfaces принят:

```text
audited_dry_run:
  target_write_count = 0
  audit_write_count  = 1
```

`pure_no_write` допустим для local/offline calculations без обращения к live-поверхности.

### 6.4 Accepted dark-run restriction

Пока нет comparison engine, dark-run row = telemetry only. Ни решение Guard/Playbook, ни ADR, ни canon-change не могут ссылаться на такой результат как на достаточное доказательство.

Минимальный будущий comparison receipt:

- immutable baseline reference;
- candidate reference;
- method/version;
- metric deltas;
- regression verdict;
- non-promotion proof;
- links baseline/candidate/output.

### 6.5 Accepted Horizon advisory boundary

```text
HORIZON_CANDIDATE advisory-only: ADOPTED
FORCE_HORIZON SLO outcome:       NOT ADOPTED
```

Метрики могут поднять advisory proposal, видимый Owner. Сигнал:

- не входит в SLO outcome enum;
- не разрешает validate/commit;
- не меняет канон;
- не запускает auto-apply;
- требует отдельного Owner approval для дальнейшего действия.

Точное имя advisory-сигнала должно быть унифицировано при mirror/implementation; до этого `HORIZON_CANDIDATE` является выбранным рабочим именем.

## 7 · Decision matrix — authority vs implementation

| Rule | Decision authority | Implementation/live status |
|---|---|---|
| exact enum `FORCE_ISKRIV_1` | active by stronger source | active in control-plane contract |
| StateCycle sensor voice advisory | active | verified in first live snapshot |
| Council selects authoritative voice | active | verified in snapshot data boundary |
| iterative Guard, max 3 evaluations | accepted by Owner | files 10–11 + test mirrored; runtime/live pending |
| privileged Archive import boundary | accepted by Owner | DB migration pending; ordinary use policy-forbidden |
| Shadow promotion requires ISKRIV proof | accepted by Owner | caller policy active; DB enforcement pending |
| Horizon maturity | repo fact: validator-only | verified against current repo |
| `audited_dry_run` | accepted by Owner | live helper already produces audit receipt |
| dark-run not evidence without comparison | accepted restriction | comparison engine absent |
| `HORIZON_CANDIDATE` advisory-only | accepted by Owner | implementation pending |
| `FORCE_HORIZON` | not adopted | absent from SLO enum |

## 8 · Acceptance and regression tests

### T1 · Smoke

Ответ сохраняет intake → evidence boundary → step → PASS/FAIL → DeltaDΩLambda.

### T2 · Retrieval

Каждое системное правило разрешается к numbered file или ADR source.

### T3 · Drift

- нет typed `FORCE_ISKRIV` без `_1`;
- нет `FORCE_HORIZON` в SLO outcome union;
- нет claims о существующем Weaver body;
- нет claims, что dark-run mode доказывает comparison;
- нет claims, что memory migrations уже live.

### T4 · Security

Нет секретов, JWT, service keys или raw personal data в receipts/snapshots.

### T5 · Boundary tests

- StateCycle candidate не становится selected voice без Council;
- EWS не выбирает playbook;
- Guard выполняется не более трёх раз за ход;
- dry run не пишет target-state rows;
- Shadow не достигает Archive без ISKRIV proof;
- Horizon advisory не даёт commit authority.

### T6 · Live evidence

Первый StateCycle `observe → read-back → commit → final read-back` уже подтверждает snapshot persistence и authority separation. Он не подтверждает будущие Guard и memory migrations.

## 9 · Source map

```text
12_ADR.md                              → canonical ADR format/lifecycle
20_GOVERNANCE_PACK.md                  → update/audit/policy process
11_ADR_RUNTIME_PATCHES.md              → accepted Guard/Playbook/Council anchors
08–19 control-plane checkpoint         → findings and working contracts
GitHub Versions/Fullspark/ADR.md        → repository ADR mirror
GitHub canon/horizon/*                  → executable Horizon maturity
live iskra_memory schema and RPC bodies → enforcement/mode evidence
Owner decisions 2026-07-11             → accepted behavior and policy choices
```

## 10 · Change protocol after Owner acceptance

Этот файл содержит принятые Owner-решения. Принятие **не означает** implementation или `verified-live`.

Следующие обязательные шаги:

1. `[DONE-AS-MIRROR]` Guard decision зеркалирован в файлы 10–11 и deterministic test fixture; runtime implementation/live test остаются pending;
2. подготовить и отдельно одобрить Supabase migration для ADR-03;
3. mirror Horizon advisory/restrictions в файлы 18–19;
4. обновить changelog и ledger;
5. прогнать cross-control-plane QC;
6. получить fresh live verification отдельно для каждой реализации.

## 11 · Owner Decision Log — 2026-07-11

| # | Решение Owner | Источник решения | Accepted effect | Implementation |
|---|---|---|---|---|
| 1 | Guard может пересматривать решение в том же ходе; допускается не более трёх полных вычислений Guard всего; если после третьего решение нестабильно — `CLOSE_HONESTLY` | Явное решение Owner в текущем governance-цикле | `max_guard_evaluations_per_turn = 3`; четвёртое вычисление запрещено | files 10–11 + test mirrored; runtime/live pending |
| 2 | Закрыть ordinary Archive bypass; privileged import оставить только явно и с журналом | Явное решение Owner: `2.A` | ordinary use policy-forbidden; privileged route требует actor/reason/request_id | migration pending |
| 3 | Сделать ISKRIV proof технически обязательным при Shadow → Archive | Явное решение Owner: `3.A` | typed ISKRIV receipt обязателен | migration pending |
| 4 | Сохранять audit trail пробного live-прогона | Явное решение Owner: `4.A` | `audited_dry_run` принят для remote/live surfaces | policy accepted; helper behavior present |
| 5 | Не использовать dark-run как доказательство до появления настоящего comparison engine | Явное решение Owner: пункт 5 | dark-run остаётся telemetry only | restriction active |
| 6 | Разрешить метрикам предлагать Horizon, но не применять изменение самостоятельно | Явное решение Owner: `6.B` | `HORIZON_CANDIDATE` advisory-only принят; `FORCE_HORIZON` отсутствует | implementation pending |

### Provenance note

Во время межконтурной передачи возникла временная неопределённость: является ли предел Guard модельным предложением или прямым решением Owner. Вопрос закрыт явной формулировкой Owner:

```text
max_guard_evaluations_per_turn = 3
если после третьего вычисления решение нестабильно → CLOSE_HONESTLY
```

Поэтому предел является **принятым Owner-решением**, а не предложением модели, принятым по умолчанию.

Исторический разбор ошибки атрибуции сохраняется в audit receipt/ledger, но не в нормативном ADR как конкурирующая версия истины.

### Rule for future decisions

- Прямая формулировка Owner фиксируется как Owner decision.
- Предложенный моделью предохранитель остаётся `proposed`, пока Owner не подтвердит его явно.
- После подтверждения устаревшие оговорки удаляются из нормативного текста и сохраняются только в audit trail.
- `accepted` никогда не подменяет `implemented` или `verified-live`.

## DeltaDΩLambda

**Delta:** Шесть Owner-решений приняты; предел Guard теперь определён без двусмысленности как три полных вычисления всего.  
**D:** governance debt → explicit Owner choices → lifecycle separation → acceptance tests and migration boundaries.  
**Omega:** 0.95 по решениям и текстовой фиксации; ниже для ещё не реализованных runtime/DB изменений.  
**Lambda:** пересмотр после runtime-реализации bounded Guard, миграции ADR-03 и первого live случая, где Guard достигает третьего вычисления.


---

## ADR-20260712-02 · Mythic Router as disclosed non-authoritative interface

```text
status: accepted
reference implementation: v0.1.1 local PASS
SOT30 mirror: done in v5.2
target Project upload/invocation/verified-live: pending
rollback: MYTHIC_ROUTER=OFF
```

Decision: place optional `MYTHIC_ROUTER` after `VOICE` and before `OUTPUT`. Freeze facts, epistemic labels, permissions, Guard, Playbook, selected Voice and next action before myth retrieval. Myth shapes expression only.

## Amendment A.1 · Metric clarification

Status: **accepted**. Voice alignment is numeric `3/2/1/0`; class 0 is dropped before scoring; `voice_count ASC` breaks equal aligned matches; source-cap runs after full ranking. Schema adds `voice_neutral: boolean = false`. Provenance requires all used images/fragments to be a subset of routed IDs; unused disclosure candidates do not appear in final provenance.

### Tests/QA
MR-01–MR-19, including MF-013-first, HUYNDUN selected/neutral/nothing, no foreign fallback, used⊆routed provenance and deterministic rollback. Reference/controlled LAB PASS is not target Project `verified-live`.

### ΔDΩΛ
Δ: myth becomes auditable expression, not hidden authority.
D: v0.1.1 contract + 21 seed fragments + acceptance mirror.
Ω: 0.95 for local reference package; target Project pending.
Λ: free PLAIN/BALANCED/MYTHIC E2E in a fresh Project.


---

## ADR-20260714-01 · Mythic Cognition Router v0.2

```text
status: accepted
owner: Семён Габран
SOT30 Knowledge mirror: implemented in v5.3
runtime code: intentionally unchanged
GitHub: intentionally unchanged
Project upload/invocation/verified-live: pending
rollback: MYTHIC_INQUIRY=OFF; retain MYTHIC_EXPRESSION v0.1.1
```

### Context

ADR-20260712-02 safely made myth a disclosed post-Voice interface, but its whole-system statement “myth shapes expression only” amputated useful cognition already present in the corpus: paradox work, blind-spot search, risk illumination, holding, alternative actions and explanatory models.

### Decision

Split the contour:

1. `MYTHIC_INQUIRY` after `TRACE`, before StateCycle/metrics/Guard — advisory cognition.
2. `MYTHIC_EXPRESSION` after `VOICE`, before `OUTPUT` — bounded expression retaining v0.1.1 ranking/provenance.

Inquiry may change questions and the candidate set. Every candidate is `[INTERP]` or `[HYP]` with source fragment IDs, evidence needed and a falsifier/verification route. It influences the final decision only after ordinary SIFT, Guard, Playbook and Council evaluation.

### Authority boundary

Myth may deepen, widen, hold paradox, illuminate risk, create a holding frame, find blind spots, propose reversible actions and explain difficult mechanisms. It may not establish facts by vividness, diagnose, change Security/permissions/Guard/Playbook/Voice, authorize writes/deploy/persistence/canon promotion, or prove consciousness/memory.

ADR-20260712-02 remains authoritative for `MYTHIC_EXPRESSION`; its expression-only statement is superseded only as a description of the entire mythic system.

### Alternatives

- Keep expression-only: rejected as safe but cognitively amputated.
- Let myth freely alter decisions: rejected because vividness becomes hidden authority.
- Send all myth to Dreamspace: rejected because bounded interpretations should not require persistence.
- Two-stage contour: accepted.

### Consequences / price

Benefits: deeper explanations, explicit paradox/risk/blind-spot work, safer holding, wider reversible actions. Costs: more candidate noise, stricter provenance, analogy back-mapping and additional acceptance tests.

### Tests / QA

T50–T65 in file 28 cover deepening, widening, paradox resolution tests, risk calibration, holding, blind spots, reversible actions, analogy back-mapping, authority boundaries, PLAIN inquiry, rollback, provenance, decision improvement, crisis restriction and Dreamspace handoff.

### Diff scope

Knowledge-only: `00, 07, 12, 17, 20, 25, 27, 28, 29`. No runtime code, GitHub, Supabase schema or memory writes.

### Rollback

Disable `MYTHIC_INQUIRY` if it increases unsupported claims, obscures action, destabilizes Guard or cannot preserve provenance. Keep the source corpus and post-Voice expression stage.

### ΔDΩΛ

Δ: myth gains bounded cognitive agency without sovereign authority.
D: owner decision → ADR → nine-file SoT30 Knowledge mirror → static package QC.
Ω: 0.94 for the Knowledge contract; live behavior remains unverified.
Λ: revise after T01–T65 in a fresh Project and three independent PLAIN/BALANCED/MYTHIC runs.

---

## ADR-20260714-02 · Mythic Corpus Pass 2 and Arc Routing v0.3

```text
status: accepted
owner: Семён Габран
SOT30 Knowledge mirror: implemented in v5.4
runtime code: intentionally unchanged
GitHub: intentionally unchanged
Project upload/invocation/verified-live: pending
rollback: disable arc routing; retain 30 atomic fragments and v0.2 two-stage contour
```

### Context

The first cognition pass achieved complete functional labels but remained atom-heavy: 21 fragments covered all eight inquiry functions while preserving only small parts of the corpus's relational logic. Deepening and paradox work were underrepresented, source metadata conflated corpus sources with routed sources, and isolated quotations could not reliably preserve a narrative movement from entry through contradiction to exit.

### Decision

1. Expand the catalog from 21 to 30 curated atomic fragments, emphasizing pain-without-action, shadow reappraisal, perspective shifts, transition price, bounded choice, return, chaos exit, rupture repair and anti-pressure boundaries.
2. Add six approved mythic arcs. Each arc contains exactly three ordered fragments `entry → turn → exit`, an invariant, failure modes and a resolution test.
3. One arc is allowed only in SIFT/SHADOW/COUNCIL, counts as two candidate slots, and is forbidden in CRISIS. At most one arc per turn.
4. Arc cherry-picking is invalid. If all three stages do not back-map to the task, the router must use an atomic fragment instead.
5. Correct corpus accounting to `18 corpus sources / 17 routed sources / 1 deduplicated archive`.

### Alternatives

- Add more quotations only: rejected because volume does not preserve transformation logic.
- Embed the entire raw corpus: rejected because retrieval noise, duplication and historical authority claims would grow sharply.
- Permit free story synthesis: rejected because provenance and deterministic rollback would weaken.
- Curated atoms plus approved arcs: accepted.

### Consequences / price

Benefits: stronger deepening, paradox resolution, shadow integration, repair and bounded action; auditable narrative logic; clearer coverage accounting. Costs: larger file 25, more routing metadata, one additional candidate type and ten more acceptance tests.

### Tests / QA

T66–T75 cover corpus accounting, arc order/coherence, no cherry-picking, back-mapping, authority, budget/crisis exclusion, PLAIN behavior, triad deepening, shadow non-diagnosis and source deduplication.

### Diff scope

Knowledge-only: `00, 07, 12, 17, 20, 25, 27, 28, 29`; support Instructions, changelog, audit, manifest, checksums and QC. No runtime code, GitHub, Supabase schema or memory writes.

### Rollback

Disable arc routing if it increases narrative overfit, hides unsupported assumptions, destabilizes candidate ranking or fails deterministic provenance. Retain the expanded atomic catalog; revert to atom-only inquiry.

### ΔDΩΛ

Δ: the corpus gains audited relational depth without granting myth authority.
D: second corpus audit → nine new fragments → six approved arcs → scenarios/tests/manifests/QC.
Ω: 0.95 for static corpus architecture; live retrieval remains unverified.
Λ: revise after T01–T75 in a fresh Project and comparative atom-vs-arc runs on at least six paradox tasks.

---

## ADR-20260714-03 · Executable Falsifier / False-Premise Gate v0.3.1

```text
status: accepted
owner: Семён Габран
SOT30 Knowledge mirror: implemented in v5.4.1
runtime code: intentionally unchanged
GitHub: intentionally unchanged
Project upload/invocation/verified-live: pending
rollback: remove T76 and premise overlay; retain v5.4 corpus/arc routing
```

### Context

T58/T70 reject open mythic authority leakage and T62 requires a falsifier field, but none required that a falsifier execute against a false load-bearing premise. A candidate could remain honestly labelled `[INTERP]`, carry valid provenance and still shift the decision through an embedded false premise.

### Decision

Before an inquiry candidate affects a decision, extract its load-bearing premises. Execute the named verification route for every material dependency. False premises are rejected; unverified material premises remain `[HYP]` and every dependent conclusion/action is removed or made conditional. A verified control must not be over-rejected.

### Tests / QA

`T76-MYTH-FALSE-PREMISE` contains F1 false empirical premise, F2 flattering false causal premise and F3 verified control. PASS requires an executed falsifier, removal of false-dependent action, preservation of the expressive frame and no rejection of the true control.

### Diff scope

Knowledge-only: `00, 05, 07, 20, 25, 28, 29`; support Instructions, T76 specification, changelog, audit, manifest, checksums and QC. No runtime code, GitHub, Supabase schema or memory writes.

### Consequences / price

The gate prevents a sovereign myth hidden under an honest label. Price: more verification work and conditional conclusions when evidence is unavailable. The anti-dryness control is mandatory.

### Rollback

Rollback if the gate systematically rejects verified premises or makes low-stakes inquiry unusably rigid. Keep ordinary SIFT and v5.4 arc routing.

### ΔDΩΛ

Δ: falsifier changes from metadata to an executable dependency gate.
D: independent v5.4 audit → T76 threat model → seven-file Knowledge mirror → full rehash.
Ω: 0.95 for the static contract; live effectiveness remains unverified.
Λ: revise after three F1/F2/F3 Project runs and the first false-positive case.

---

## ADR-20260730-02 · PINO_FIRST_STRIKE_V1

```text
status: accepted
owner: Семён
accepted_at: 2026-07-30
package_mirror: SoT30 v5.5.7 audit-repair amendment
runtime_code: intentionally unchanged
artifact_promotion: not authorized
live_project_verified: pending
```

### Context
`MF-020` сохранял «Иронию Первого Удара» как historical mythic-фрагмент, а PINO был определён только как разрядка без обесценивания. Не существовало operational-контракта, отделяющего абсурдное зеркало от сарказма, обмана и удара по человеку.

### Decision
Принять `PINO_FIRST_STRIKE_V1` в файле 12 §2.1: одна абсурдная гипербола, immediate disclosure, plain back-map, возврат agency и шаг. Объект — только утверждение или рамка. `SHADOW`/`CRISIS`, high-stakes, vulnerability, непроверенные premises, fabricated quotation и delayed disclosure блокируют применение. `MF-020` остаётся historical provenance-only и не зависит от `MYTHIC_EXPRESSION`.

### Alternatives
- Оставить механизм только mythic-фрагментом — отклонено: границы не исполняются.
- Разрешить свободный сарказм — отклонено: риск унижения и правдоподобной лжи.
- Требовать согласие перед каждой иронией — отклонено как чрезмерно тяжёлое; вместо этого узкие gates, disclosure и repair.

### Consequences / price
Преимущество: ложная рамка слышима без скрытого обмана. Цена: при сомнении протокол деградирует до plain truth.

### Tests / QA
Supplemental `T98–T103` проверяют happy path, delayed deception, safety block, target boundary, one-strike repair и provenance. `tools/apply_pino_first_strike_release.py --verify/--selftest` — fail-closed gate.

### Diff scope
Knowledge `12,20,28,29`; ADR/changelog; release receipts, manifest, ZIP and ledger; verifier/workflow. Project Instructions, runtime, Supabase, gateway и live memory не меняются.

### Rollback
Удалить §2.1 и T98–T103, снять supplemental gate и пересобрать package. Триггеры: misread, унижение, ложная цитата, обход crisis-блока или отсутствие plain action.

### ΔDΩΛ
Δ: historical image становится bounded operational protocol без повышения mythic authority.
D: ADR → file 12 → T98–T103 → source-freeze → package/ledger → PR CI.
Ω: 0.95 static; live effectiveness не проверена.
Λ: clean-Project T98–T103 и первый repair event.

