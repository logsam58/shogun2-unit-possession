# Source-control workflow

Clone the repository, create a branch from main, configure local dependencies and make a focused change. GitHub Desktop and the command line both operate on the same Git history; either can review changes, commit, pull and push.

```powershell
git switch -c describe-your-change
git status
git diff
# Stage reviewed source/doc files using GitHub Desktop or git add.
py tools/audit_repository.py
git diff --cached --stat
git commit -m "Describe the resulting behavior"
git push -u origin describe-your-change
```

Open a pull request with the behavior change and checks actually run. See [CONTRIBUTING.md](../CONTRIBUTING.md) for project-specific expectations. Generated outputs and local resources are ignored. Never force-add game files, recordings, dumps or credentials.

Git tracks source; changing a commit does not replace a game's loaded DLLs. Builds, closed-game installation and a fresh live session are separate steps. Product releases use reviewed tags, attached installers and checksum manifests; development commits are not automatically installed or published.
