# Publication handoff — private GitHub preview

After the initial local preparation, the owner explicitly authorized uploading
the prepared page and installer to GitHub while keeping the repository private.
The page is on the [public-release branch](https://github.com/logsam58/shogun2-first-person/tree/public-release).
The installer, frozen source kit and checksums are in the [private draft release](https://github.com/logsam58/shogun2-first-person/releases/tag/untagged-ccbcd8419ff6f2951702).
There is no public release, repository rename or visibility change. The original
main branch and published v0.28/v0.27 releases remain intact.

## Prepared presentation

The [README](../../README.md) is the landing page, with the full player controls
and a gateway to technical documentation. [GitHub presentation](GITHUB_PRESENTATION.md)
contains the exact About description and recommended topics. The
[release body](../releases/v0.28-public-preview.md) uses the actual version and
artifact names, with a clear private-preview status.

The existing recordings examined did not provide a suitable verified showcase
of the finished native mod. The page therefore contains no substituted gameplay,
generated imagery or placeholder screenshots. Actual native-mod footage can be
added later without changing the text's feature claims.

## Prepared software

- **`Shogun2-First-Person-v0.28-Setup.exe`**: new Inno Setup wrapper, carrying the
  exact accepted Test158 payload. Private preview; unsigned; no new gameplay build.
  Local output: `artifacts/standard-setup/release/output/`. Its checksum is beside
  it in `SHA256SUMS.txt`; the [validation receipt](STANDARD_SETUP_VALIDATION.json)
  identifies the exact package and accepted payload.
- **`Shogun2FP-Public-Source-Kit.zip`**: existing frozen source/local-resource kit
  at source commit `3eab195ff5fd0c063e1d9150358e12a813b624b1`. Its SHA-256 remains
  `72807f6703fe714e6705053b08209a0f33f360f0551829d519f9d720d2852162`.
  It predates these presentation/packaging files and is not silently replaced.

The original custom setup, finished build, installed Test158, accepted Test157
rollback and previous release assets remain separate. The new setup is not
installed into the real game by this task.

## Before a public download

Game-derived resources embedded in the player binaries are outside MIT. The
owner previously approved the source/local-resource kit while redistribution
permission remains unresolved. Preparing a private EXE does not change that
decision. A public player-binary download needs that question resolved; the
source/local-resource kit remains the prepared distribution route. See
[provenance](PROVENANCE.md).

The current version is **v0.28**. Existing tags and published assets are
immutable. Do not upload the new same-named setup over the old v0.28 asset or
retag the release. The owner must choose how to publish the new packaging while
preserving that history; this preparation assigns no new version.

The release body uses absolute documentation links to the uploaded
`public-release` branch. The README links to the actual private draft. Keep the
private-preview notice while downloads remain private and redistribution status
is unresolved. A future public launch remains the owner's decision.

The optional diagnostic recorder is separate from the player installer. No new
recorder deployment or live-test readiness is implied. Any future installation
or game test still needs its own authorization and the project's fresh loaded-
identity/recording gate.
