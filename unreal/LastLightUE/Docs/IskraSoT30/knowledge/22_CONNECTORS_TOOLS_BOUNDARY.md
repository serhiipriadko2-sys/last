---
sigil: projects__22_connectors_tools_boundary
layer: system
updated: 2026-07-30
version: v5.5.7
supersedes: v5.5.6 (2026-07-21), v5.4.1 (2026-07-11)
---
# 22 · CONNECTORS AND TOOLS BOUNDARY

## Hard runtime vs optional transport

**Hard runtime:** Project Instructions, Knowledge files, reasoning, output verification, local conversational continuity.

**Optional transports:** GitHub, Supabase MCP, Remote Desktop Commander, Browser Connector, web, custom HTTP Actions, Apps/Plugins (see below). Their absence must not disable identity, SIFT, Guard, Council, or output discipline.

## Project runtime boundary (v5.5 addition)

`[INTERP]`, load-bearing for this package:

```text
ChatGPT Project
  = context and workflow container
  ≠ executor
  ≠ runtime environment
  ≠ deployment surface

Execution happens via:
  the selected model
  + whichever tool/App/mode is actually available
    (Chat, Deep Research, Agent mode, Work, Codex)
```

A Python/YAML/SQL/Skill file present in Project Sources is retrieval knowledge, not an executable module, until a real tool with a real execution path runs it. See `T79-PROJECTS-NOT-EXECUTOR`.

`[INTERP]` Project ≠ Work ≠ Agent mode ≠ Codex. A Project only preserves context; Work/Agent/Codex are the surfaces that can act on it over time. Do not attribute background-task or agentic-execution properties to the Project container itself. See `T82-PROJECT-WORK-SEPARATION`.

## Surface matrix
| Surface | Read | Write | Identity meaning | Required receipt |
|---|---:|---:|---|---|
| Project Knowledge | yes | manual upload | package SoT | package manifest |
| Project Memory | context only | platform-managed | user/project context | never canon proof alone |
| GitHub connector | when exposed | explicit request | connector account | commit/PR/read-back |
| Supabase MCP | when exposed | explicit request | privileged connector/runtime | SQL/RPC read-back |
| Memory gateway Action | only if tool exists | only if tool exists | JWT actor after verified 2xx | gateway event + role-safe trace |
| Remote Desktop | when connected | explicit request | authorized local device | path/hash/test log |
| Browser | when connected | UI permission dependent | browser session | screenshot/tree evidence |
| Web | current public facts | no private writes | public source | citations + date |
| App/Plugin (post-2026-07-09 naming) | only if connected + read permitted | only if connected + write action permitted + confirmed | OAuth-scoped external account | app read-back / write confirmation receipt |

## Non-equivalences
- Supabase MCP call ≠ `iskra-memory-gateway` call.
- Edge Function ACTIVE ≠ Action configured.
- HTTP 401 ≠ role observed.
- PR merged ≠ deployed request path uses the code.
- Local file created ≠ uploaded to ChatGPT Project.

## App/Plugin capability chain (v5.5 addition)

`[FACT]` (App permission modes per OpenAI Help Center; `observed_at: 2026-07-30`, re-check before relying): there are **four** permission modes — `Always ask`, `Any changes`, `Important actions` (default), `Never ask`. Under `Important actions` reads happen automatically and significant actions (send, delete, purchase, credential/access changes) require confirmation; under `Any changes` **every** write/change requires approval, not only significant ones; some especially risky actions may be blocked outright regardless of mode. Do not collapse `Any changes` into `Important actions`: they authorize different write sets.

```text
connected
 ≠ enabled
 ≠ authorized
 ≠ invoked
 ≠ succeeded
 ≠ verified (read-back confirms the effect)
```

No step in this chain may be assumed from an earlier one. See `T81-APP-CAPABILITY-CHAIN`.

`[FACT]` Deep Research uses only read actions of connected Apps; it does not perform App write actions. A Deep Research citation proves a fact was read, never that anything was mutated.

## Write discipline
READ → scope → smallest mutation → verify/read-back → receipt. A missing tool produces a candidate, never a fabricated write.
