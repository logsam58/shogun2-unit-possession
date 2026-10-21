# Prepare a release

The approved public format is a source/local-resource kit. Local setup/HUD/loader binaries embed game resources; do not upload them while redistribution permission is unresolved. Build tools are obtained separately.

1. Read current-release.json, choose a branch/candidate and preserve published tags.
2. Regenerate roles/tables/resources, then run current native/Python/diagnostic checks from a fresh source export with explicit paths.
3. Record source revision/content digest, tools, resources, results, explicit optional skips and live limits. Update acceptance.
4. Run tools/package_source.py --output-dir with a new directory. Curated selection excludes assets/binaries/private evidence; audit checks paths, secrets and binary signatures. A SHA-256 manifest and notices are included.
5. Extract and verify the kit, then build independently. Inspect its ZIP list for private/proprietary payloads.
6. Review the local commit and kit. Obtain owner approval before push/publication/visibility/release changes. The packager performs none of those operations.

    py -3.12 tools/package_source.py --output-dir 'D:\Shogun Source Release'

Git exports record revision and selected-source changes; an archive uses a content digest and must not invent a revision. Byte-identical binaries are not promised: compiler paths/timestamps and packing can differ. Validate meaningful resource geometry, source hashes and tests.

Keep v0.28 immutable. Test158 is development, not a replacement for accepted Test157. Live validation has a separate fresh loaded-hash/capture gate. Historical operator publication helpers remain in Git, outside this kit and procedure.
