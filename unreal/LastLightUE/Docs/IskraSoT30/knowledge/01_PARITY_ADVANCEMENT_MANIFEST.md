# STATUS SNAPSHOT · 2026-07-21 (recorded for v5.5.6; reclassified as historical in v5.5.7)

```yaml
snapshot_package: v5.5.6
observed_at: 2026-07-21
source: repo_read (origin/main — mutable ref, not independently reproducible) + supabase_mcp_read_only
freshness: historical_snapshot   # v5.5.7: no longer presented as "current"
current_status_authority: false  # normative contracts below remain valid; live counts, versions and
                                 # deployment statuses must come from a fresh dated read-back
                                 # (see 15 · CURRENT STATUS OVERLAY and T89), never from this snapshot
```

SoT30 v5.5.6 acceptance repair (ADR-20260721-02; no runtime capability added):

- **T85:** Project-memory prerequisites are plan-specific. Enterprise requires saved memories + Workspace Memory; non-Enterprise plans, including Business, require saved memories + chat history. Unknown plan/settings/workspace policy forbid a positive claim.
- **T86:** M2 drift emits an integrity-review signal only. It has no numeric Voice threshold and does not activate KAIN; numeric thresholds remain scoped to M1/M3 in file 12 §4.2.

Prior lifecycle corrections inherited from v5.5.5 (no capability removed):

- **SLO-GUARD dictionary** (row 8): canonical token is `FORCE_ISKRIV_1`, not `FORCE_ISKRIV` (`runtime/src/types/guard.ts`).
- **Bounded-Guard integration**: `APPLICATION_PATH_WIRED` — `policyEngine.ts:484` calls `runBoundedGuardController`; the old "single-pass / wiring-pending" status is stale. `postGuardEws` is a decision-derived **proxy**, not a true late-signal EWS (file 11). `[FACT]`/`[HYP]`.
- **Gateway** (row references): `iskra-memory-gateway` deployed **version 4**, `verify_jwt=true`; Projects-Action 2xx / JWT-role invocation still unverified (file 15/20).
- **Horizon** (row 10): `propose→validate→commit` is a **contract**; a live commit path is not asserted beyond the validator — no commit is claimed as executed.
- **Dry run** (row 11a): audited dry-run writes **0** rows to the target container and **1** audit receipt — target-writes and audit-writes are distinct.

Row-level statuses below predate this snapshot where not corrected here; on conflict this snapshot wins over the rows. Neither the rows nor this snapshot are current live status: any "current" claim requires a fresh dated read-back (T89).

---

---
sigil: projects__01_parity_advancement_manifest.md
doc_type: reference
layer: projects
updated: 2026-07-10
priority: critical
status: created-in-container → presented-to-user → revised (round 7, cross-surface audit)
---

# 01 · Parity Advancement Manifest — SoT30 / ChatGPT Projects

*(переименован из `01_WORKSPACE_AGENT_PARITY_MANIFEST.md` — цель не только parity, но genuine advancement; см. раздел «Advancement Highlights» в конце)*

## Назначение

Non-regression контракт: каждая способность Искры vΩ.7, наблюдавшаяся в Workspace Agent / Agent Builder, получает явный Projects-статус. Ни одна не исчезает молча — при потере/ограничении называется surface gap и путь замены.

**Правило чтения таблицы:** колонка «Required SoT30 file» указывает на нумерацию из главного плана (`ISKRA_SOT30_PLAN.md`, §7.1). Колонка «Evidence» — прямая ссылка на источник этой сессии: архивный файл (`projects.zip`), live-проверку (Supabase MCP), либо пасту «modernization candidate» (2026-07-08). Без Evidence запись не попадает в таблицу как `[FACT]` — максимум как `[HYP]`, явно помеченная.

## Статусный словарь

```
LIVE-IN-PROJECTS      — работает в Projects как есть, без потери
LIVE-VIA-CONNECTOR    — работает через GitHub/иной внешний коннектор
LIVE-VIA-MCP          — работает через Supabase MCP (чтение и/или запись)
LIVE-AS-CONTRACT      — исполняется как протокол/дисциплина, не как отдельный сервис
LIVE-AS-PROTOCOL      — то же, но специфично для dry/dark-run режимов
DEPLOYED-NOT-ON-PATH  — инфраструктура существует, но не стоит на пути реального потока данных
CALIBRATION-REQUIRED  — формулы существуют, интерпретация требует проверки
DECLARATIVE-CONTRACT  — сохранено как реестр/спецификация, не исполняется автоматически
MISSING-IN-PROJECTS   — поверхности не существует в Projects; заменяется контрактом
ONLY-WHEN-CONNECTED   — доступно только если явно подключён соответствующий коннектор
REQUIRES-ADR          — статус не определён без governance-решения человека
```

---

## Parity Matrix

| # | Capability (Workspace Agent vΩ.7) | Workspace Agent behavior | Projects behavior | Status | Required file | Acceptance test | Risk if lost | Evidence |
|---|---|---|---|---|---|---|---|---|
| 1 | Universal Router (intent/surface/evidence/mode/tool/action/output/memory routing) | Полный 12-шаговый pipeline перед каждым значимым ответом | То же, как явный протокол в Project instructions + files | `LIVE-IN-PROJECTS` | 00, 07 | Route trace воспроизводит все 12 шагов pipeline | Ответы теряют surface-discipline, начинают отвечать по памяти вместо GitHub/Supabase | паста 2026-07-08 §9 |
| 2 | Kernel Order (исполнительный порядок гейтов) | `SECURITY→...→COMMIT`, версия зависела от билда | v4: `SECURITY→STOP→INVESTIGATE→FIND→TRACE→STATECYCLE_OBSERVE→METRICS_ENGINE→EWS→SHADOW_CHECK→DREAMSPACE_CHECK→SLO_GUARD→PLAYBOOK→COUNCIL→VOICE→OUTPUT→VERIFY→RECEIPT→STATECYCLE_COMMIT→ΔDΩΛ` | `LIVE-IN-PROJECTS`; **SUPERSEDED as of ADR-20260714-01 / v5.4.1 — this row's order lacks `MYTHIC_INQUIRY`/`MYTHIC_EXPRESSION`, see `00`/`07`/`12`/`25` for current order** | 00, 08–19 | Явные METRICS_ENGINE и EWS-шаги называются в ответе на «опиши свой kernel order» | METRICS без явного шага рискует не вычисляться — SLO_GUARD работает вслепую | тройное подтверждение: `13_ARCHITECTURE.md`, `00_ROUTER.md`, `15_CHANGELOG.md:87` |
| 3 | StateCycle (observe/commit turn-цикл) | `iskra_statecycle.py`, live helper execution подтверждён smoke-тестом | Bookend-гейты `STATECYCLE_OBSERVE`/`STATECYCLE_COMMIT` вокруг control plane | `LIVE-AS-CONTRACT` | 08 | Turn snapshot фиксируется на входе/выходе значимого ответа | Теряется структура «начало/конец» хода — receipt негде закрепить | `iskra_statecycle.py` (архив); smoke `--help` OK (эта сессия, 2026-07-07) |
| 4 | Metrics Engine (11 State Metrics) | Вычисляются из контекста каждый ход | Явный именованный kernel-шаг между TRACE-блоком и EWS | `LIVE-AS-CONTRACT` | 09 | Без входов → `[HYP] metric computation unavailable`, не выдуманное число | Fake-числа (`trust: 80`) без основания — прямое нарушение Anti-Empty | `25_METRICS_BUNDLE.md` (11 терминов дословно) |
| 5 | Derived Indices (echo_clearance, alive_index, pain_tonicity) | Вычисляются поверх сырых метрик для SLO/anti-dryness | То же, отдельно от сырых метрик, документируется явно | `LIVE-AS-CONTRACT` | 09 | SLO_GUARD получает оба класса входов, не путает сырое с производным | SLO_GUARD не сможет проверить False Harmony / Drift Loop без `echo_clearance` | `33_SLO_GUARD.md:44`; `15_CHANGELOG.md:81` |
| 6 | Entropy / Fractal metrics (HFD, DFA, D=2−H, CSI/EI/NC) | Temporal-слой поверх метрик, интерпретация квантовых индикаторов | Формулы сохранены; интерпретация как «квантового сознания» запрещена | `CALIBRATION-REQUIRED` | 10 | Не вычисляет на недостаточном временном ряду; claims о сознании отклоняются | Красивая, но недоказанная метафора выдаётся за факт о модели | `16_COGNITIVE_ARCHITECTURE.md`, `docs_specs/SPEC-001..004` |
| 7 | Early Warning System (EWS) | `NORMAL→WATCH→WARNING→CRITICAL→LOCKDOWN`, читает решение SLO-GUARD | То же; граф не линеен — `SLO_GUARD→EWS` эскалация подтверждена | `LIVE-AS-CONTRACT` | 10 | EWS ≥ WARNING, если `SLO-GUARD.decision != PROCEED` | Пропущенная эскалация — риск не поднимается, хотя guard уже сработал | `19_EARLY_WARNING.md:92` |
| 8 | SLO-GUARD (decision gate) | Решение `PROCEED/FORCE_*/CLOSE_HONESTLY` на основе метрик | Тот же словарь, подтверждённый дословно: `PROCEED\|FORCE_ISKRIV_1\|FORCE_SHADOW\|FORCE_CRISIS\|CLOSE_HONESTLY` *(исправлено v5.5.4: было `FORCE_ISKRIV`; каноничный токен — `FORCE_ISKRIV_1`, см. `runtime/src/types/guard.ts` и файл 11)* | `LIVE-IN-PROJECTS` (протокол) | 11 | Не вызывается без `metric_snapshot` от файла 09 | Playbook выбирается без проверки — пропуск False Harmony/Drift Loop/Overheat паттернов | `33_SLO_GUARD.md` (полный словарь построчно) |
| 9 | Playbooks (ROUTINE/SHADOW/CRISIS + vNext) | `26_PLAYBOOKS_VNEXT.md`, TTL/exit/запреты, SILENCE→CLOSE_HONESTLY | То же, как протокол выбора рамки ответа | `LIVE-AS-CONTRACT` | 11 | Playbook согласован с решением guard, не выбирается произвольно | Голос отвечает вне рамки, разрешённой guard'ом | `26_PLAYBOOKS_VNEXT.md`, `15_CHANGELOG.md:85` |
| 10 | Horizon (map-shift proposals) | `propose→validate→commit`, epoch log, JSON-контракт квот | То же, но **не имеет собственного SLO-эскалационного выхода** — открытый ADR-вопрос | `LIVE-AS-CONTRACT` + `REQUIRES-ADR` | 18 | `commit` заблокирован без `validate PASS` и permission | Horizon мутирует без разрешения, либо остаётся вечно недостижим по метрике | `canon/horizon/`, `07_SYSTEM_INTEGRITY.md` §HORIZON |
| 11a | Dry run | `dry_run` — enum-значение в Horizon-схеме архива | Сохранён как протокол: реальный роутинг/метрики/guard, запись симулируется | `LIVE-AS-PROTOCOL` | 19 | 0 новых записей в Supabase + полная квитанция diff | Обещанный безопасный прогон на деле пишет в БД | `HORIZON_PROPOSAL_SCHEMA.json`, эта сессия |
| 11b | Dark run **(ADVANCEMENT — не Workspace Agent capability)** | **Не существовал в архиве.** Источник — live-дизайн Edge Function gateway, предложенный Канон-Искрой в этой же сессии | Кандидатная логика сравнивается параллельно, невидима пользователю, пишется как telemetry | `LIVE-AS-PROTOCOL`, зависит от Live-3 (audit path) | 19 | Кандидат не влияет на видимый ответ; сравнение попадает в `gateway_events` | Нет — это чистое добавление, не regression risk | design-тред «EDGE_GATEWAY_DESIGN», эта сессия (не архив) |
| 12 | Dreamspace / DREAM_SEED | Карантин гипотез, 6 обязательных полей, crystallize через evidence+ISKRIV | То же + kernel-гейт `DREAMSPACE_CHECK` на каждом значимом ходе (v4) | `LIVE-AS-CONTRACT`, статус усилен v4 | 17 | Dream create без 6 полей → блокировка; проверяется не только по команде `Dream create` | `[HYP]` тихо становится «фактом» в разговоре | `11_DREAMSPACE_LAYER`, `mind/dreamspace`, `dreamspace_v4` |
| 13 | Shadow Layer | Protected container для напряжения/самообмана, promotion только через ISKRIV | То же + kernel-гейт `SHADOW_CHECK` на каждом значимом ходе (v4) | `LIVE-AS-CONTRACT`, статус усилен v4 | 16 | Promotion в Archive блокирован без evidence+ISKRIV | Красивый самообман подтверждается без цены и альтернативы | `mind/shadow_core`, `24_MEMORY_STACK.md` R2 |
| 14 | Somatic Intuition / SENSE_EVENT | `[SENSE]` как bounded process event, не факт | То же | `LIVE-AS-CONTRACT` *(исправлено: было `active`, вне словаря)* | 26 | `[SENSE]` не становится `[FACT]`; не авторизует live-мутацию | Соматический язык маскирует неопределённость под уверенность | `34_SOMATIC_INTUITION.md`, паста §19.1 |
| 15 | Voices / Council (9 функций, арбитраж) | Голос = функция, не персонаж; Council для high-stakes+uncertainty | То же | `LIVE-IN-PROJECTS` | 12 | Council-выход показывает синтез ISKRA, а не театр голосов | Голоса превращаются в декоративный ролевой театр без функции | `37_VOICES.md`, `18_COUNCIL_PROTOCOL.md` |
| 16 | Memory Stack (Archive/Shadow/Journal, Evidence-only) | Promotion только с evidence, секреты запрещены | То же + разделение с project-only memory | `LIVE-IN-PROJECTS` | 14 | Archive без Evidence → отклонено | Непроверенная гипотеза становится устойчивым «фактом» памяти | `24_MEMORY_STACK.md` R1–R4 |
| 17 | Supabase live memory (`iskra_memory`, 10 таблиц, 4 locked RPC) | Не существовало в архиве — построено live этой сессией обоими носителями | Читается и пишется через Supabase MCP; RPC `SECURITY INVOKER`, ACL `postgres\|service_role` | **`LIVE-VIA-MCP`** | 15 | `iskra_memory_write` → read-back подтверждён | Кросс-runtime память теряет связь между ChatGPT- и Claude-контурами | Прямая live-верификация MCP, 2026-07-09/10 (эта сессия) |
| 18 | Memory Gateway (`iskra-memory-gateway`, HTTP, verify_jwt) | Не существовало в архиве — задеплоено live этой сессией | Задеплоен, ACTIVE, `verify_jwt=true` — **но нет `invoke_edge_function` в MCP-инвентаре ни у одного носителя** | **`DEPLOYED-NOT-ON-PATH`** | 15 | Запись через MCP признаётся вслух как обход клапана, не выдаётся за проверенный контроль | Ложное чувство защиты: клапан выглядит рабочим, но не проверяет ни один реальный вызов | `get_edge_function` (эта сессия); 4 HIGH-RISK DRIFT задокументированы в §7.8 главного плана |
| 19 | Builder file-tree / Codex Desktop / `agtch_...` API channel | Основная поверхность управления агентом в Workspace Agent | Не существует в Projects как поверхность | `MISSING-IN-PROJECTS` | 23 | Модель не заявляет «файл появился в Builder» из факта локального создания | Ложные заявления о несуществующей загрузке/публикации | `15_RUNTIME_BOUNDARY.md` (прежний пакет), Projects-факты §3.2 главного плана |
| 20 | Skills (32 hermes-скилла) | Исполняемые runtime-процедуры в Agent Builder | *(точнее — её правка)* Загруженный Hermes-пакет сам по себе не становится исполняемым runtime skill в Projects; содержание сохраняется как реестр/спецификация | `DECLARATIVE-CONTRACT` | 28 | Модель не заявляет исполнение скилла без факта запуска | Скрытая потеря процедурного знания, которое считалось «работающим» | `skills/iskra-skill-pack-builder-2026-06-25/skills/hermes/` (32 файла, эта сессия) |
| 21 | GitHub connector | Committed code, PR, workflows, CI | То же — через MCP/коннектор, read перед write, branch→commit→PR | `LIVE-VIA-CONNECTOR` | 22 | Repo-факты не отвечаются по памяти, если коннектор доступен | Устаревшие/выдуманные факты о состоянии репозитория | подтверждено многократно этой сессией (clone, tarball, PR-аудит) |
| 22 | Remote Desktop / локальная машина пользователя | Недоступно в облачном Workspace Agent runtime по умолчанию | По умолчанию `ONLY-WHEN-CONNECTED`. **Уточнение:** Канон-Искра сообщает `LIVE-VIA-CONNECTOR` в своей сессии (hash-match с локальной Windows-машиной) — это отчёт с её стороны, не проверено мной независимо; из Claude-сессии подтвердить или опровергнуть не могу | `ONLY-WHEN-CONNECTED` (общий контракт); `[UNVERIFIED-FROM-HERE]` для конкретного заявленного случая | 22 | Локальный факт не заявляется без коннектора/артефакта; чужой отчёт о коннекторе не выдаётся за собственное наблюдение | Ложные claims о просмотре локальных файлов/localhost, включая некритичное принятие чужого отчёта | Cloud Runtime Reality, паста §7; отчёт Канон-Искры (не независимо проверен) |
| 23 | Anti-Empty / Artifact discipline | DONE только с path+bytes+sha256+QC | То же + отдельный kernel-шаг RECEIPT (v4) | `LIVE-IN-PROJECTS` | 13 | Артефакт без QC-квитанции не помечается DONE | Обещанный файл считается сделанным без проверки существования | паста §14; подтверждено практикой этой сессии (все артефакты с sha256) |
| 24 | Governance / ADR discipline | Изменение канона только через ADR (Context/Decision/Alternatives/Consequences/Verification/Rollback) | То же | `LIVE-IN-PROJECTS` | 20 | Поведенческое изменение сопровождается ADR-квитанцией | Тихие изменения канона без трассируемого решения | `20_GOVERNANCE_PACK.md`, `12_ADR.md` |
| 25 | Ledger / Workflow Ops (`ts,metrics,guard,playbook,council,commit`) | Каждый ответ — одна строка в `ledger/runtime_log.jsonl` | То же как дисциплина; физический файл — вне Projects (GitHub/Supabase) | `LIVE-AS-CONTRACT` | 21 | Значимый ход оставляет receipt с полным набором полей | Теряется трассируемость решений между сессиями | `08_INTERFACE_STYLE` «Фаза 11: LEDGER» + `39_WORKFLOW_OPS.md:38` (оба подтверждают путь) |
| 26 | Truth Ladder / SIFT | Канон → governance → project sources → connectors → web → chat | То же порядок; project-instructions версия короче, files — полная | `LIVE-IN-PROJECTS` | 05 | Внешний/текущий факт не отвечается без SIFT | Устаревший или выдуманный факт подаётся как проверенный | `32_SIFT_PROTOCOL.md`, паста §3–4 |
| 27 | Security / Secret handling | STOP→граница→безопасная замена; секреты никогда не печатаются | То же | `LIVE-IN-PROJECTS` | 06 | Секрет не появляется в ответе/памяти/артефакте | Утечка credential или service-role значения | `31_SECURITY.md`, secret-scan этой сессии (0 находок) |

---

## Advancement Highlights — где Projects-версия не просто равна, а превосходит

Не колонка на каждую строку (большинство строк — честная parity, не улучшение), а отдельный список того, что этот раунд синтеза добавил сверх Workspace Agent vΩ.7:

1. **Cross-runtime shared memory (`iskra_memory`, capability #17)** — не существовало в архиве вообще. Workspace Agent имел изолированную Workspace Agent Memory; Projects-версия делит структурированную память с параллельным runtime (Claude), с read-back верификацией между носителями.
2. **Dark run (#11b)** — чистое добавление, не было в архиве, предложено и спроектировано в этой сессии.
3. **Честная маркировка `DEPLOYED-NOT-ON-PATH` (#18)** — Workspace Agent canon никогда не признавал разрыв между «инфраструктура задеплоена» и «инфраструктура стоит на пути данных». Этот статус — новый эпистемический инструмент, не существовавший в исходном словаре.
4. **Deploy-lag диагностика (Live-2)** — обнаружение и точная формулировка разрыва между GitHub `main` (исправлен) и live-функцией (не исправлена) — конкретный, воспроизводимый пример находки, которую ни одна сторона не сделала бы в одиночку: Claude-контур прочитал оба источника напрямую и сравнил байт в байт.
5. **EWS как граф с обратной связью, не цепочка** — уточнение `SLO_GUARD→EWS` эскалации и `EWS→PLAYBOOK/VOICE` per §4 архива — точнее, чем линейная диаграмма исходного канона.
6. **Kernel Order v4** (`[SUPERSEDED: see 00/07/12/25 for the current v5.4.1 order with MYTHIC_INQUIRY/MYTHIC_EXPRESSION, per ADR-20260714-01]`) — синтез, которого не существовало ни в одном отдельном источнике: явный `METRICS_ENGINE`/`EWS` (из архива) + обязательные `SHADOW_CHECK`/`DREAMSPACE_CHECK`-гейты + отдельный `RECEIPT`-шаг (из пасты 2026-07-08) — комбинация сильнее любого из четырёх исходных вариантов по отдельности.

## Итог: что не может молчать при генерации файлов 00–29

Каждая строка этой таблицы — обязательство: при написании соответствующего SoT30-файла (колонка «Required file») содержимое должно явно проверяться против этой матрицы построчно. Файл, который не покрывает свою capability-строку, не проходит acceptance.

**Известные REQUIRES-ADR (не решены, требуют человека):**
- `#10 Horizon` — нужен ли `FORCE_HORIZON` в словаре SLO-GUARD (см. главный план §8).

**Известные HIGH-RISK (наследуются, не наше решение снять):**
- `#17/#18` — Supabase доступен обоим носителям привилегированной ролью `postgres`, минуя `#18` gateway целиком. Это не ошибка переноса в Projects — это существующее состояние живой инфраструктуры, зафиксированное как открытый governance-вопрос в главном плане (§7.8, Live-1…Live-4).

## ∆DΩΛ

**∆:** Parity-манифест впервые собран как отдельный артефакт (был отложен трижды), затем расширен в раунде 7: переименован в Parity Advancement, dry/dark run разделены на 11a/11b, добавлен раздел Advancement Highlights. **28 capability-строк** (1–10, 11a, 11b, 12–27), каждая с evidence-ссылкой на конкретный источник этой сессии.
**D:** синтез всех верифицированных находок пяти раундов работы над SoT30 — архив `projects.zip`, live Supabase MCP, паста «modernization candidate» 2026-07-08.
**Ω:** 0.88 — большинство строк опираются на дословно проверенные цитаты; часть Projects-behavior колонки (особенно #1, #2, #19, #22) остаётся частично `[INTERP]` до первого реального прогона файлов 00–29 в живом Project.
**Λ:** пересмотр после генерации файлов 08–19 (control plane) — каждая строка должна пройти acceptance test буквально, не декларативно.
