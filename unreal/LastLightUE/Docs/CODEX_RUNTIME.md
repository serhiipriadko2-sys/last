# Codex and the full Искра SoT30 contract

Use VS Code opened at `E:\LastLightUnreal\LastLightUE`; Codex uses root `AGENTS.md` automatically on **new sessions**. The project instructions file is only an entrypoint: the complete canonical source set is `Docs/IskraSoT30/knowledge/00–29`, plus `Docs/IskraSoT30/support/PROJECT_INSTRUCTIONS_SOT30.md` and `MANIFEST.json`. Read the local full files when needed, don't invent their contents from a short AGENTS summary.

## Exact copy chain
Canonical public source: https://github.com/serhiipriadko2-sys/iskra/tree/e38ecb00c07176c7a1b0dcbee24ccac3cbe5f786/governance/releases/2026-07-30-sot30-v5-5-7-audit-repair
Local copy script: `Scripts/sync_sot30.ps1`, frozen to the exact Git SHA above. It downloads the 30 Knowledge files and two support files, fails closed if any source file SHA or byte length mismatches `support/MANIFEST.json`.
Local read-back: `Scripts/verify_sot30.ps1`. Do not blindly use the manifest's `generated_from_ref` as the copy's source ref; its historical build commit predates later amendments to the release folder. The frozen folder commit above is the actual source of these bytes.

## Codex checklist
- Restart or start new Codex thread to reload AGENTS.
- Ask Codex: "Read AGENTS.md, verify SoT30 hashes, list your instruction sources, report Kernel Order and build gates without changing files."
- Review a test change and require source trace, PASS/FAIL, paths and SHA.
- Codex or VS Code does not become a separate Iskra runtime merely from opening a folder; SoT30 governs agent *working behavior*, not additional tool capabilities.
- Keep owner permissions on actual tools. No secrets, no unauthorized network writes, no canon promotion.
