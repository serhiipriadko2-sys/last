# CURRENT STATUS OVERLAY · 2026-07-30

```yaml
observed_at: 2026-07-30
observation_surface: supabase_connector_read_only (independent PR#321 audit read-back)
freshness: dated_observation      # becomes STALE/UNKNOWN without a fresh re-read; never quote as live
maturity: read_only_inspection    # not an end-to-end HTTP/Projects-Action test
project: typcvaszcfdpkzbjzuur
supersedes: 2026-07-20 overlay below (kept as history)
```

Facts below are **independent** (none inferred from another); each must be re-read before any live claim:

- **MIGRATION_PARITY** — **`HIGH-RISK DRIFT`, not parity.** Live `list_migrations` = **37** (was 35 on 2026-07-20) and the repo carries 37 files in `supabase/migrations/`, but the counts matching is *not* parity: a repo-wide search of `supabase/migrations/*.sql` finds **no** `memory_consent_registry` and **no** `iskra_memory` deny-all policy. Recreating the backend from committed migrations would therefore produce a **different** schema and a **weaker** security posture than the live plane described below. Equal counts with divergent content is the most misleading form of this drift. Required before treating live state as canonical: add the missing migration(s) to Git, or record an explicit emergency-remediation receipt naming what was applied out-of-band, by whom and when. `[FACT @2026-07-30: repo side verified by search in this session]`
- **LIVE_SCHEMA** — `iskra_memory` has **11 tables**, including `memory_consent_registry` (absent from the 2026-07-20 overlay **and from every committed migration** — see the drift note above). RLS enabled 11/11; client roles carry **explicit deny-all policies** — the earlier "zero policies" state is superseded. `anon`/`authenticated` have no schema USAGE and no table grants. `[FACT @2026-07-30 via connector read-back recorded in the PR#321 audit; not re-read by this session — treat as dated observation, re-read before any live claim]`
- **EDGE_FUNCTION_DEPLOYMENT** — `iskra-memory-gateway`: ACTIVE **version 4**, `verify_jwt=true`, handler runs in **`mode: probe_only`**: `auth/whoami` may answer after JWT validation, while every privileged route (`observe`, `commit`, `memory/write`, `memory/search`, `shadow/promote`, `dream/crystallize`, …) returns **HTTP 503 `gateway_security_hold`**. Correct maturity label: `DEPLOYED-PROBE-ONLY ≠ ENABLED ≠ INVOKED ≠ VERIFIED-2XX-PRIVILEGED`. `[FACT @2026-07-30]`
- **PROJECTS_ACTION_INVOCATION** — not tested; end-to-end HTTP enforcement from a Project surface remains unverified. `[HYP]`
- **LIVE_DATA_COUNTS** — not re-read on 2026-07-30; the 2026-07-20 counts below are historical. `UNKNOWN until re-read.`

---

# HISTORICAL OVERLAY · 2026-07-20 (superseded by the 2026-07-30 overlay above)

```yaml
observed_at: 2026-07-20T12:00Z
observation_surface: supabase_mcp_read_only
freshness: historical_snapshot   # superseded 2026-07-30
maturity: read_only_inspection   # not an end-to-end HTTP/Projects-Action test
project: typcvaszcfdpkzbjzuur
```

This overlay supersedes the dated 2026-07-10/2026-07-11 counts and the gateway-v1/v2
statements further below. The five status facts below are **independent** — none is
inferred from another:

- **MIGRATION_PARITY** — repo `supabase/migrations/*.sql` = 35 files; `list_migrations` live = 35; in parity. `[FACT]`
- **LIVE_SCHEMA** — schema `iskra_memory` present; its tables have RLS enabled with **zero policies** (confirmed by security advisors `rls_enabled_no_policy` for gateway_events / horizon_events / memory_archive / memory_dream_seeds / memory_edges / memory_journal / memory_open_loops / memory_sense_events / memory_shadow / statecycle_snapshots). RLS is not the enforcing layer for `postgres`/`service_role`. `[FACT]`
- **LIVE_DATA_COUNTS** — `iskra_memory` rows (read-only SELECT, 2026-07-20): journal 18, archive 2, shadow 2, open_loops 5, sense_events 2, dream_seeds 2, edges 5, statecycle_snapshots 4, gateway_events 7, horizon_events 0. (Changed vs 2026-07-11: journal 14→18, open_loops 4→5.) `[FACT]`
- **EDGE_FUNCTION_DEPLOYMENT** — `iskra-memory-gateway`: ACTIVE **version 4**, `verify_jwt=true` (was recorded as v1 in §7 / v2 in the prior overlay — both stale). Deployment ≠ invocation. `[FACT]` This overlay does not re-read the v4 gateway body; the §7 actor-source drift note below is from the v1 read and is `[HYP]` until a fresh body read confirms it on v4.
- **PROJECTS_ACTION_INVOCATION** — not tested this observation; no `invoke_edge_function`/Projects-Action tool was exercised, so end-to-end HTTP enforcement from a Project surface remains unverified. `[HYP]`

Direct Supabase MCP is a privileged path; it is **not equivalent** to gateway invocation
or end-user identity. Mandatory Projects runtime must work without this plane and degrade
to `memory write unavailable`.

---

---
sigil: projects__15_supabase_memory_plane.md
doc_type: reference
layer: projects
updated: 2026-07-10
priority: critical
status: created-in-container -> live-verified -> qc-pass
required_by: 01_PARITY_ADVANCEMENT_MANIFEST.md capabilities #17 and #18
---

# 15 · Supabase Memory Plane — SoT30 / ChatGPT Projects

## 0 · Назначение и граница

Этот файл описывает **физическую live-поверхность** structured memory. Семантика контейнеров задаётся файлом 14; Shadow и Dreamspace — файлами 16 и 17.

```text
SoT/GitHub = committed truth
Memory Model (14) = semantic contract
Supabase memory plane (15) = live persistence and transport
Project Memory = conversational continuity, not SoT
```

Supabase не переписывает канон автоматически. Наличие строки в БД доказывает persistence, но не превращает запись в `[FACT]`, ADR или SoT.

## 1 · `[FACT]` Live inventory — 2026-07-10

Project: `typcvaszcfdpkzbjzuur` · schema: `iskra_memory`.

| Table | Rows | Назначение |
|---|---:|---|
| `memory_journal` | 14 | chronology / audit history |
| `memory_archive` | 2 | evidence-backed claims |
| `memory_shadow` | 2 | protected uncertainty with exit vector |
| `memory_open_loops` | 4 | blockers, decisions, next signals |
| `memory_sense_events` | 2 | bounded process signals, never facts |
| `memory_dream_seeds` | 2 | pre-hypothesis quarantine |
| `memory_edges` | 5 | typed relations between memory records |
| `statecycle_snapshots` | 0 | Project runtime snapshots |
| `gateway_events` | 1 | gateway/DB boundary receipts |
| `horizon_events` | 0 | controlled future-option proposals |

All ten tables have RLS enabled. `[FACT]` `pg_policies` returned **zero policies** for schema `iskra_memory`. Table grants are limited to `postgres` and `service_role`; no grants were observed for `anon` or `authenticated`.

**Interpretation:** access is closed to ordinary client roles, but `postgres`/`service_role` remain privileged paths. RLS is not the enforcing layer for those privileged roles.

## 2 · Function boundary

Live schema contains sixteen functions. Seven form the Project-facing boundary:

```typescript
type MemoryBoundaryRpc =
  | 'iskra_memory_write'
  | 'iskra_memory_search'
  | 'iskra_memory_promote_shadow'
  | 'iskra_memory_crystallize_dream'
  | 'iskra_project_observe'
  | 'iskra_project_commit'
  | 'iskra_project_horizon_propose';
```

`[FACT]` They are `SECURITY INVOKER` (`prosecdef=false`) and executable only by `postgres` and `service_role`. The current MCP SQL surface runs as:

```text
current_user=postgres
session_user=postgres
jwt_role=null
can_assume_service_role=true
```

Therefore:

```text
LIVE-VIA-MCP = true
least-privilege user identity = false
identity derived from end-user JWT = false on direct MCP path
```

## 3 · Safe payload gate

Every boundary RPC routes payloads through `assert_safe_memory_payload` or equivalent helpers.

`[FACT]` The live function rejects common secret-like forms, including:

- OpenAI-style `sk-...` keys;
- PEM private keys;
- JWT-shaped triples;
- long values assigned to names such as `service_role`, `api_key`, `secret`, `token`, `password`, `private_key`.

This is a useful last-line guard, not a substitute for secret hygiene. Secrets must never be intentionally sent to the memory plane.

## 4 · Write protocol

Any write initiated from a Project follows:

```text
READ current target
-> PLAN container/action
-> VERIFY no secret / correct authority
-> CALL locked RPC
-> READ-BACK by returned id
-> VERIFY fields and status
-> RECEIPT
-> MEMORY EDGE / JOURNAL when required
```

A successful RPC response without read-back is `PARTIAL`, not persistence proof.

Direct DML or generic SQL is reserved for diagnostics and migrations. Routine memory writes use RPCs so that validation, audit journal and relation edges remain visible.

## 5 · Container routing

`iskra_memory_write(container, payload, actor)` supports:

```text
journal | archive | shadow | open_loop | sense_event | dream_seed | edge
```

The generic writer creates an audit entry in `memory_journal` for all containers except `journal`, where the created row is itself the audit record.

`iskra_memory_search` searches the six semantic containers:

```text
journal | archive | shadow | open_loop | sense_event | dream_seed
```

It is lexical/substring retrieval, not semantic vector search. `memory_edges` are typed relations, not similarity edges.

## 6 · Promotion enforcement matrix

| Path | Live-enforced | Missing / external contract |
|---|---|---|
| direct `archive` via `iskra_memory_write` | non-empty evidence, claim, source surface | explicit SIFT trace, ISKRIV receipt, ADR requirement |
| `iskra_memory_promote_shadow` | evidence, claim, source surface, source exists, edge + audit journal | explicit `iskriv_check` parameter |
| dream -> `archive` via `iskra_memory_crystallize_dream` | evidence refs, claim, source surface, `iskriv_check`, edge + audit journal | external validation that the supplied check is trustworthy |
| dream -> `shadow` | source exists, target transition, edge + journal | evidence not required because target remains non-factual |
| dream -> `adr_draft` | open-loop record + edge + journal | creates an ADR candidate, not an accepted ADR |

### HIGH-RISK DRIFT · Archive bypass

The generic writer can insert directly into `memory_archive` with evidence while defaulting `sift_status='verified'`. It does **not** require an ISKRIV verification receipt.

SoT30 rule:

```text
PROHIBITED for ordinary promotion:
iskra_memory_write('archive', ...)

REQUIRED:
shadow -> iskra_memory_promote_shadow + external ISKRIV receipt
or
dream -> iskra_memory_crystallize_dream(target='archive', iskriv_check=...)
```

Emergency/import use of direct Archive write requires a governance receipt and post-write audit.

### HIGH-RISK DRIFT · Shadow gate partial

`iskra_memory_promote_shadow` enforces evidence but has no `iskriv_check` argument. Until a migration adds it, the caller must store the ISKRIV verification pointer in `evidence`, `decision_link`, tags or metadata and prove it in the external receipt. File 16 owns this caller-side gate.

## 7 · Gateway surface

Live Edge Function:

```text
slug: iskra-memory-gateway
version: 1
status: ACTIVE
verify_jwt: true
routes: 9
```

Routes:

```text
observe | dry-run | dark-run | commit | horizon/propose
memory/write | memory/search | shadow/promote | dream/crystallize
```

### LIVE-REPO DRIFT · actor identity

`[FACT]` Live v1 derives actor from `body.actor` via `asActor(body)`. The verified GitHub `main` version derives actor from the Authorization JWT and removes client-supplied actor. The fix exists in repository state but is not deployed live.

Status:

```text
GitHub main: actor-from-JWT
Supabase live v1: actor-from-body
verdict: deploy lag / identity spoof risk on live v1
```

No deployment is performed by this document.

### Audit coverage

- `project.observe` and `project.commit` call `iskra_gateway_event` inside the database and can create `gateway_events` receipts.
- `memory/write`, `shadow/promote` and `dream/crystallize` create `memory_journal` audit entries, not `gateway_events` rows.
- The existing single `gateway_events` row is a DB dry-run receipt. It proves the database audit path, not an end-to-end HTTP invocation from ChatGPT.
- The current connector inventory exposes deploy/read operations but no `invoke_edge_function` tool, so HTTP gateway enforcement remains unverified from this Project surface.

## 8 · Authority and actor contract

```typescript
interface MemoryWriteAuthority {
  user_requested: boolean;
  action: 'read' | 'write' | 'promote' | 'crystallize' | 'commit';
  actor_source: 'jwt' | 'connector_role' | 'explicit_untrusted_label';
  target_container: string;
  requires_adr: boolean;
  evidence_refs: string[];
  read_back_required: true;
}
```

Rules:

- direct MCP actor is a connector/runtime identity, not a verified human identity;
- `body.actor` from live gateway v1 is untrusted metadata;
- no memory write authorizes canon mutation;
- destructive or bulk operations require explicit approval, scope and rollback;
- do not print connection strings, JWTs, keys or secret values in receipts.

## 9 · Fallback

If Supabase is unavailable:

```text
[HYP] memory write unavailable
```

Return the intended container, payload summary and read-back plan, but do not claim persistence. Chat context may carry temporary continuity only.

## 10 · Acceptance

PASS if:

- live table/function status is dated and source-labelled;
- write path includes read-back and receipt;
- ordinary Shadow/Dream promotion cannot use generic direct Archive write;
- privileged MCP bypass is named;
- RLS-enabled-with-zero-policies is not misrepresented as end-user policy enforcement;
- gateway deployment is not confused with gateway invocation;
- actor-from-body live drift is named;
- no secret value is printed or persisted.

FAIL if:

- a database row is treated as SoT merely because it exists;
- `service_role`/`postgres` access is described as least privilege;
- direct Archive write silently bypasses SIFT/ISKRIV;
- a successful RPC without read-back is called persisted;
- deployed gateway is called enforcing while real writes bypass it.

## Source map

- live `iskra_memory` schema/table/function/ACL inspection — 2026-07-10;
- live `iskra-memory-gateway` v1 source and config — 2026-07-10;
- `14_MEMORY_MODEL.md` — semantic containers and promotion pipeline;
- `24_MEMORY_STACK.md` + `10_ADR_MEMORY_STACK.md` — Evidence-only Archive, no secrets;
- `31_SECURITY.md` — least privilege and secret boundary;
- frozen `ISKRA_SOT30_PLAN_v8_FINAL.md` §3.2/§7.8.

## DeltaDΩΛ

**Delta:** Supabase memory is specified as a typed, privileged live plane rather than a magical shared memory. Two previously hidden bypasses are explicit: generic Archive write and Shadow promotion without an enforced ISKRIV argument.
**D:** live schema/functions/ACL/RLS/gateway source -> authority matrix -> safe write/read-back contract.
**Omega:** 0.95 — live structure and function bodies were read directly; HTTP invocation remains untested from this surface.
**Lambda:** revise after gateway v2 deployment, a migration that closes Archive/Shadow gate gaps, or the first verified end-to-end HTTP receipt.
