# Known issues and evidence limits

## Captured Test99 effects crash

The user's extended siege session ended in C0000005. Both real full dumps agree: native effects object table+28 is null and count+2C is0, while active global record6 still references element1 of that object. Native14A3F50 returns0 for the empty table, but outer14A3FC0 still scans the active records and reads table element1. At14A4011 it evaluates0+(1*5)*8+4=2C and faults.

The captured owning-manager vtable matches the indexed native constructor naming Effect Instances and Parsed Datasets. Seven complete bounded fault/caller code ranges in runtime memory match relocated stock. A bounded caller chain was verified with actual Memory64 stack storage and native saved-register/RET/argument cleanup. Other stack words remain candidates, not a full unwind.

These facts establish the immediate failure, not the write/lifetime history. Stale ownership across effects reset/destruction/reuse is a focused hypothesis. Mod influence remains unresolved. No exact effect asset, native-only origin, universal crash fix or connection to historical E6748/E674A/C0C624 signatures is claimed. The older unit-experience hypothesis does not explain this actual dump.

Pre-fault samples show about1.25GiB private process memory,6.15GiB available physical RAM,11.22GiB commit headroom, a1.77GiB largest free region, and1750MiB used device VRAM out of12288MiB. This is not evidence of resource exhaustion; it does not rule out every allocation/hardware fault. A post-fault9.12s QTE input interval overlaps full-dump suspension and cannot establish a pre-fault gameplay hitch.

Full dumps, actual input/memory records and the detailed local findings remain outside Git because they include process memory and large session data. Local reports are under the original workspace's outputs/crash-investigation-Test99-20261017 and automatic-recordings/20261017-153311-pid23208.

## Gameplay limits

Mounted-bow presentation was rejected in Test87; the later acquisition candidate was never universally accepted. Specialty units, every campaign/map lifecycle, naval control and crew possession are not certified. The latest siege feedback covers the user's session. Preserve working branches and verify additional cases instead of extrapolating from finite fixtures.

The circular accuracy cone uses native effective unit accuracy with a fixed calibration conversion. It is not an exact implementation of stock rectangular scatter or a certified stock hit-probability match. Non-firearm descending-dash reticles retain their earlier uncalibrated presentation.

Timestamped QTE input is judged against recent software-submitted marker geometry. This is bounded software matching, not proof of physical display scanout or complete end-to-end hardware latency.
