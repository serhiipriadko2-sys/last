@echo off
setlocal
set "ROOT=E:\LastLightUnreal\LastLightUE"
set "CODEX=C:\Users\gabra\AppData\Local\Programs\OpenAI\Codex\bin\codex.exe"
cd /d "%ROOT%"
"%CODEX%" exec -C "%ROOT%" --sandbox read-only --ephemeral --skip-git-repo-check -- "Read AGENTS.md and Docs/IskraSoT30/support/PROJECT_INSTRUCTIONS_SOT30.md. Verify the 30 SoT files by SHA-256 using Scripts/verify_sot30.ps1. Output the exact canonical Kernel Order and PASS/FAIL plus game build test commands. Do not edit files. Keep under 350 words." <NUL
exit /b %errorlevel%
