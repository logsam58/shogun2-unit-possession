# Your Git workflow

The online repository belongs to **logsam58**: https://github.com/logsam58/shogun2-first-person . It starts private.

- A **commit** is a named saved checkpoint of source changes.
- A **push** uploads those commits to GitHub.
- A **pull** downloads changes made elsewhere.
- A **branch** lets an experiment develop without replacing the main branch immediately.

GitHub Desktop is optional. To use it, sign in to your account, choose File > Add local repository and select the clean checkout folder. You can review Changes, write a short summary, Commit to main (or your branch), then Push origin. Once pushed, your source checkpoint is backed up on GitHub.

Terminal equivalents, from this repository:

```powershell
git status
git diff
git add native tools docs README.md BUILD_AND_TEST.md SOURCE_MAP.md VALIDATION.md
git diff --cached --stat
git commit -m "Describe the completed change"
git push origin main
```

Git keeps source history, not the running game installation. Rolling back a source commit does not replace a loaded game DLL. Build, closed-game installation and a fresh user launch/readiness check remain separate operations. The accepted checkpoint79 release stays preserved locally; this repository begins from the newer Test99 development source.
