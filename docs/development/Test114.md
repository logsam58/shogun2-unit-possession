# Test114: reduce redundant checks during crowded combat

This v0.19 development candidate addresses redundant work in the QTE crowd-reservation hooks. The player reported severe QTE slowdown when many units clash. Every unrelated participant was causing full validation of each reserved soldier before comparing its address. A nonmatching human participant is now rejected by address first. All possible matches still receive the existing current identity proof; horse/rider handling is unchanged.

The focused regression fails against the previous header at 93 memory reads for a five-participant encounter and passes with 23 reads after the correction. The fixture also checks all participant positions, dead/reused identities, unit/side/kind mismatches, read failure, native contact targets and 10,000 unrelated encounters. This demonstrates reduced redundant work, not a measured gameplay FPS improvement.

All 38 QTE fixtures pass, including the existing reservation, mounted, native ABI, input, timing and rendering checks. Exact accepted Test109 adapter and HUD binaries are retained. Test113's latest acknowledged-image input correction remains included. The loader and transactional installer are checked separately, including the exact installed Test113 predecessor, seven interrupted upgrades and repeat-install backup retention.

The additional 60-second PresentMon trace did not overlap a QTE and cannot establish the reported live cause. Software Present rates and native simulation rates alone do not measure physical display smoothness. Registry capacity, memory and VRAM samples did not demonstrate exhaustion in the examined intervals; other native lifetime or CPU problems remain possible.

Preparation flags in Test114-validation.json describe the package before deployment. Private installation and readiness receipts establish actual installed/loaded state. Native DLL replacement requires a closed game and a fresh user Steam launch. Automatic recording continues independently of chat replies. This candidate has no live acceptance or native crash-fix claim and is not a published release.
