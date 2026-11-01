---
sigil: projects__14_memory_model.md
doc_type: reference
layer: projects
updated: 2026-07-11
priority: critical
status: created-in-container → qc-pass → presented-to-user
required_by: 01_PARITY_ADVANCEMENT_MANIFEST.md capability #16
---

# 14 · Memory Model — SoT30 / ChatGPT Projects

## 0 · Назначение и граница

Memory Model отвечает на три вопроса:

1. что заслуживает долговременной фиксации;
2. куда именно это писать;
3. что требуется для promotion между слоями.

Он **не определяет физическую Supabase-схему** — это файл 15. Он задаёт семантический контракт, одинаковый для файлов, Project Memory и внешней structured memory.

```text
SoT = каноническая истина
Memory = durable continuity with provenance
Chat history = context, not primary truth
```

## 1 · Четыре поверхности памяти

| Surface | Роль | Авторитет | Риск |
|---|---|---|---|
| SoT files / GitHub | принятые правила, решения, канон | highest project authority | stale version / drift |
| Project Memory | разговорная непрерывность внутри Project | contextual | opaque retrieval, не SoT |
| Structured Memory | Archive/Shadow/Journal/open loops/evidence | durable operational continuity | bypass, bad promotion, identity drift |
| Chat context | текущий ход и временные ссылки | transient | ошибочно принять за факт |

Конфликт разрешается Truth Ladder. Chat history не повышает claim до `[FACT]`.

## 2 · Canonical containers

```typescript
type MemoryContainer =
  | 'ARCHIVE'
  | 'SHADOW'
  | 'JOURNAL'
  | 'OPEN_LOOPS'
  | 'EVIDENCE_INDEX'
  | 'ADR_LOG'
  | 'DREAM_SEEDS';
```

### 2.1 ARCHIVE — verified knowledge

```typescript
interface ArchiveRecord {
  id: string;                    // ARCH-YYYYMMDD-### or durable UUID
  claim: string;
  evidence_refs: string[];
  sift_trace: string[];
  decision_link?: string;
  tags: string[];
  status: 'verified' | 'superseded' | 'needs_review';
  backlinks: string[];
  created_at: string;
}
```

Правила:

- нет evidence → не Archive;
- факт без цитаты/артефакта не проходит;
- current web fact содержит дату актуальности;
- behavior/canon change требует ADR link;
- superseded запись не удаляется молча, а получает статус/ссылку на замену.

### 2.2 SHADOW — protected uncertainty

```typescript
interface ShadowRecord {
  id: string;
  raw: string;
  why_it_matters: string;
  risk_type: 'hallucination' | 'bias' | 'emotional' | 'scope' | 'unknown';
  next_evidence_to_seek: string[];
  promotion_rule: string;
  review_at?: string;
  status: 'open' | 'promoted' | 'closed';
  promoted_to?: string;
}
```

Shadow разрешает гипотезу и напряжение, но обязан иметь `vector of exit`: что проверить, чтобы поднять запись выше.

### 2.3 JOURNAL — chronology, not canon

```typescript
interface JournalRecord {
  id: string;
  context: string;
  actions_done: string[];
  outcome: string;
  delta: string;
  pain_or_block?: string;
  next_step: string;
  links: string[];
  created_at: string;
}
```

Journal говорит «что произошло», а не «что истинно навсегда».

### 2.4 OPEN_LOOPS

```typescript
interface OpenLoopRecord {
  id: string;
  question: string;
  risk: string;
  owner: 'USER' | 'ISKRA' | 'EXTERNAL';
  blocker_type: 'evidence' | 'permission' | 'tool' | 'decision' | 'live_state';
  next_signal: string;
  status: 'open' | 'blocked' | 'resolved' | 'superseded';
  evidence_refs: string[];
}
```

Open loop не маскируется обещанием «потом сделаю». Он хранит конкретный следующий сигнал.

### 2.5 EVIDENCE_INDEX

Содержит pointers, hashes, dates и surface; не копирует секреты и большие источники целиком.

```typescript
interface EvidencePointer {
  id: string;
  surface: 'project_file' | 'github' | 'supabase' | 'web' | 'artifact' | 'chat';
  locator: string;
  sha256?: string;
  observed_at: string;
  freshness?: string;
  claim_ids: string[];
}
```

### 2.6 ADR_LOG

Поведенческое решение хранит ссылку на полный ADR, status, accepted/rejected alternatives, verification и rollback trigger.

### 2.7 DREAM_SEEDS

Dream seed остаётся `[HYP]` и маршрутизируется файлом 17. Он не попадает в Archive без crystallize/promotion gate.

## 3 · Memory action contract

```typescript
type MemoryAction =
  | 'NONE'
  | 'WRITE'
  | 'UPDATE_STATUS'
  | 'LINK'
  | 'PROMOTE'
  | 'SUPERSEDE';

interface MemoryPlan {
  action: MemoryAction;
  target: MemoryContainer | 'SOT';
  reason: string;
  durable_value: boolean;
  contains_secret: boolean;
  evidence_refs: string[];
  requires_adr: boolean;
  write_surface: string | null;
}
```

Memory action планируется Router-ом до ответа, но исполняется **только после VERIFY**. Он не добавляет новый top-level kernel-шаг: запись и read-back являются внутренним subprotocol шага `RECEIPT`.

```text
OUTPUT → VERIFY
→ RECEIPT { prepare memory action → authorized write → read-back → finalize memory receipt }
→ STATECYCLE_COMMIT
```

Так сохраняется Kernel Order v4 `OUTPUT → VERIFY → RECEIPT → STATECYCLE_COMMIT`, а `memory_receipt` формируется только после наблюдаемого read-back. Если запись не выполняется или недоступна, RECEIPT фиксирует `NONE`/`[HYP] memory write unavailable`, не выдумывая persistence.

## 4 · Когда писать

Писать, если turn создаёт durable value:

- материальный audit outcome;
- behavior/canon/governance decision;
- high-risk drift или неизвестность;
- завершённый артефакт с receipt;
- blocker, важный будущим сессиям;
- стабильное project/user preference;
- следующий action, который должен пережить чат.

Не писать:

- случайную болтовню;
- непроверенную догадку в Archive;
- секреты, токены, private keys, PII без необходимости;
- весь ответ целиком, если достаточно pointer/summary;
- claim только потому, что он повторился в чате.

## 5 · Promotion pipeline

```text
SHADOW / DREAM_SEED
→ formulate claim
→ obtain evidence
→ SIFT
→ ISKRIV integrity check
→ ADR if behavior/canon changes
→ ARCHIVE
→ backlink source record
```

```typescript
interface PromotionRequest {
  source_id: string;
  source_container: 'SHADOW' | 'DREAM_SEEDS';
  claim: string;
  evidence_refs: string[];
  sift_status: 'PASS' | 'PARTIAL' | 'FAIL';
  iskriv_check: 'PASS' | 'FAIL';
  adr_ref?: string;
  target: 'ARCHIVE' | 'SOT';
}
```

Promotion блокируется, если:

- evidence пуст;
- SIFT не PASS для factual Archive claim;
- ISKRIV check не PASS;
- нужен ADR, но ссылки нет;
- source record не получает backlink/status update.

## 6 · Read-before-write and read-back

Перед записью:

1. найти существующий claim/record;
2. не создавать дубль, если нужен status update/link;
3. проверить surface authority;
4. проверить секреты;
5. получить permission для remote/live mutation.

После записи:

1. прочитать запись обратно;
2. сверить id/container/claim hash;
3. зафиксировать actor/source surface;
4. создать memory receipt;
5. при несовпадении не заявлять persistence.

```typescript
interface MemoryReceipt {
  schema_version: 'iskra.memory.receipt.v1';
  action: MemoryAction;
  target: string;
  record_id: string | null;
  write_surface: string | null;
  actor: string | null;
  write_status: 'written' | 'updated' | 'not_written' | 'failed' | 'unavailable';
  read_back_status: 'verified' | 'mismatch' | 'not_attempted' | 'unavailable';
  claim_sha256?: string;
  evidence_refs: string[];
  error?: string;
}
```

## 7 · Unavailable memory

Если write surface не подключена/недоступна:

```text
[HYP] memory write unavailable
```

Ответ не говорит «я запомнил». Вместо этого:

- отдаётся локальный memory candidate;
- указывается target container;
- даётся безопасный manual step;
- статус `not_written/unavailable`.

Неудачная память не отменяет полезный ответ, но блокирует claim о persistence.

## 8 · Secret and privacy boundary

```text
R5: secrets/tokens never stored
```

Перед записью:

- redact credential values;
- не сохранять Authorization headers;
- не класть service-role key в evidence;
- хранить pointer на защищённый secret manager, не значение;
- минимизировать PII;
- при сомнении выбрать `NONE` или quarantined Shadow без идентификаторов.

Memory Model не даёт права обходить Security или consent.

## 9 · Supersession and drift

Память append-oriented:

```text
old record → superseded
new record → verified
link: supersedes / contradicted_by / derived_from
```

При конфликте:

```text
A vs B
→ Truth Ladder
→ DRIFT marker
→ open loop or ADR
```

Нельзя тихо переписывать историю, особенно если предыдущая ошибка важна для governance trace.

## 10 · Mapping to Project files

Если файловая память доступна, рекомендуемые views:

```text
development-diary.md  ← JOURNAL
project-memory.md      ← stable verified project facts / ARCHIVE view
open-loops.md          ← OPEN_LOOPS
adr-log.md             ← ADR_LOG
evidence-index.md      ← EVIDENCE_INDEX
shadow.jsonl           ← SHADOW
dream-seeds.jsonl      ← DREAM_SEEDS
archive.jsonl          ← ARCHIVE
```

Файлы — views. Каноническая запись может жить во внешнем structured store; источник истины и surface должны быть названы явно.

## 11 · Project Memory boundary

ChatGPT Project Memory помогает continuity, но:

- не является SoT;
- не заменяет Archive evidence;
- может быть opaque и retrieval-based;
- не считается доказательством успешной записи;
- не используется как единственное основание project fact, если доступен файл/GitHub/Supabase.

Формула:

```text
Project Memory remembers context.
SoT proves rules.
Archive proves claims.
Ledger proves actions.
```

## 12 · Cross-file routing

- файл 13 создаёт artifact/council/answer receipt;
- файл 14 определяет, что из receipt имеет durable value;
- файл 15 исполняет structured-memory transport;
- файл 16 защищает Shadow;
- файл 17 защищает Dream seeds;
- файл 20 управляет ADR/SOT promotion;
- файл 21 хранит operational ledger discipline.

## 13 · Acceptance

**PASS**, если:

- SoT, Project Memory, structured memory и chat context различаются;
- Archive требует Evidence+SIFT;
- Shadow/Dream имеют promotion rule;
- Journal не выдаётся за факт;
- write выполняется после VERIFY;
- remote write проходит Action Gate;
- read-back проверяет persistence;
- memory unavailable называется прямо;
- секреты не сохраняются;
- superseded history не удаляется молча.

**FAIL**, если:

- «я запомнил» сказано без receipt;
- chat history повышает claim до FACT;
- Shadow попадает в Archive напрямую;
- артефакт записывается без hash/evidence pointer;
- одинаковый claim плодится дублями вместо link/update;
- write failure скрывается красивым закрытием.

## 14 · Source map

- `24_MEMORY_STACK.md` — R0–R10, Archive/Shadow/Journal formats and promotion.
- `10_ADR_MEMORY_STACK.md` — Projects memory decision and alternatives.
- modernization candidate §9.12 — Memory Router targets and durable-value triggers.
- `13_OUTPUT_RECEIPTS_ANTI_EMPTY.md` — verification/receipt ordering.
- `01_PARITY_ADVANCEMENT_MANIFEST.md` — capability #16.
- file 15 — live Supabase implementation, intentionally separated.

## ΔDΩΛ

**Δ:** Память разложена на authority surfaces и typed containers; write без read-back больше не считается persistence.
**D:** Memory Stack R0–R10 + Projects ADR + Router durable-value triggers + receipt contract.
**Ω:** 0.93 — семантика Archive/Shadow/Journal подтверждена; конкретная физическая mapping-схема зависит от файла 15 и поверхности исполнения.
**Λ:** пересмотр после package gate 12–14 и первого verified memory write/read-back через файл 15.
