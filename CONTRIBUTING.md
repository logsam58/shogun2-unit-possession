# Development workflow

Keep each change focused and build/test the affected native components. Preserve the independent unit simulation and all current ownership/lifetime guards. Report live evidence separately from offline fixture coverage.

Use a branch for experiments, then review the diff and commit a short description of the resulting behavior. Push commits to GitHub after checking the staged file list. Avoid force pushing or rewriting history without an explicit reason and authorization.

Before a push, run `py tools/audit_repository.py`. Game files, assets, raw recordings/dumps, generated binaries, local config and credentials stay outside version control. Keep source-import provenance as the historical record of the initial Test99 import rather than updating it to hide later code changes.

No project-wide license has been selected. Existing third-party notices remain in place. Source publication and downloadable binary releases are distinct; this repository currently stores development source, not a new player release.
