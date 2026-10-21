# Native integration contracts

This is an analysis-based x86 integration, not recovered original game source. Exact engine hash is pinned in identity.h and game_packs.py. An RVA is relative to loaded Empire.Retail.dll. Never reuse layouts/signatures for another image.

“Verified” means fingerprint/byte gates and finite fixtures, not exhaustive live safety. Source typedefs/signatures/bounds remain authoritative. [Native inventory](native-inventory.json) indexes hook/ABI/offset/lock sites; it is not semantic proof of every field.

| Boundary | Contract / evidence | Limits |
| --- | --- | --- |
| Roles | common/soldier_roles.h: ten final human vtables, separate horse kind. Specialists use own config/equipment. Adapter, production HUD and diagnostic negatives. | Naval class identity is not ship movement; animals/vehicles/unknowns excluded |
| Actor | Serial +1c8, life +f0, unit +214; unit serial +4; camera table RVA15d22e8, subject +188. Root RVA1bc8180 → env +110 → manager +281fc → camera +8e0. HUD rereads table/serial/unit/camera/definition/army. | Addresses alone are not identity; leases expire |
| Tasks/walls | Unit ba8 is count, task slots ba0/ba4 require bounded type/environment/unit identity. Portal getters397e40/397e80 preserve navigation surface/frame. Animation bearing and navigation location differ. | Real asynchronous solving/world scene stays a provider boundary |
| Camera lifetime | Hooks325ca0/321310/319dd0/2d6210/340d30/2fcb10/341560/321d70 scoped by lease. Routing tolerance cannot resurrect stale actors. Death retains numeric poses. | New candidate live transitions pending |
| ABI | __thiscall ECX objects, __fastcall shims explicitly accept EDX, cdecl/stdcall/WinAPI signatures preserved at call sites. | Do not change typedef/trampoline/native bytes independently |
| IPC | Magic/version/size/PID/lease/actor/unit plus bounded sequence/heartbeat checks. Odd sequence is unstable. QTE Publication91 is internal DLL storage. | Snapshots never confer ownership of game pointers |
| Foreign calls | Original behavior forwarded outside owned actor/context. Unit AI, morale/fatigue, cooldowns and pair registration remain native. | Scoped fixtures do not cover every native caller |

## Threads and reentrancy

Adapter hooks run on native calling threads; worker requests 8 ms service. WndProc and render callbacks have their own contexts. Entry/release/combat-pair/pose locks are explicit in source. Never dereference a retained actor address without validating the current lease.

QTE has worker/input polling (requested 1 ms), render worker, native callbacks, authorization/click/frame locks and callback accounting. Judgments use submitted geometry plus native deadlines. HUD is a separate read-only process.

The inventory exposes sites for review. Lock ordering and race freedom are not formally proved. Do not call unknown native functions from arbitrary threads or hold a new lock across an original callback without reentrancy analysis. World/render/animation allocation, input/display and asynchronous pathfinding remain external boundaries. Requested sleeps are not guaranteed latency.

## Shutdown

Adapter disables control before restoration, drops queued shots and may retain the projectile guard while a fire ticket is pending. It restores WndProc only if still its own, checks restoration bytes, and leaves DLL/mappings/trampolines resident until process exit. It does not hot-unload under callbacks.

QTE clears commitments, disables hooks, waits for active callbacks and requests render stop with a bounded renderer wait. DllMain creates the worker rather than doing native setup under loader lock. Error/shutdown paths are not a universal clean-unload guarantee. The external debugger must detach without terminating its target; toy tests cover it.

ABI/thread/lifetime/shutdown changes need native/negative tests and separately authorized live validation. This branch does not change gameplay hook scheduling.
