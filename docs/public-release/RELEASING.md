# Releasing

The owner accepted Test159 after trying the standard installer from a clean starting point. Use the exact tested `Shogun2-First-Person-v0.28-LocalAssets-Setup.exe`, the current `Shogun2FP-Public-Source-Kit-v0.28.zip` and `SHA256SUMS.txt` in private draft ID420192018.

Keep the repository private and the draft unpublished until the owner explicitly decides otherwise. Do not change main, merge the public branch or overwrite the published v0.28/v0.27 releases. The owner authorized replacing superseded draft previews; retain verified local copies and the asset retirement receipt.

For future software changes, use the local-assets build profile and complete its native/Python/asset/isolated installer checks. Audit the PE component closure: no game image/font/audio resources may be shipped. Never distribute generated local resource bundles or the complete build folder. Documentation-only updates reuse the accepted setup without rebuilding gameplay.

Record user acceptance and formal preplay verification separately. Test159's manual trial was accepted, but its formal native-hook/capture gate was not completed. A future real swap needs fresh CLOSED and rollback; a new live process/test needs fresh READY/IN BATTLE and exact loaded identities plus actual capture verification.

Charging and castle wall climbing are intermittent known bugs. Do not remove these warnings based only on prior acceptance or offline checks.

Changing repository visibility exposes existing history/releases too. Older resource-containing published/private history is unchanged. Public launch remains a separate owner decision.
