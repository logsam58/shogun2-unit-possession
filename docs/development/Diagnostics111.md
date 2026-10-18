# Diagnostics111: isolated debugger hardening

This is offline diagnostic work. The running installation remains the capture-only Diagnostics110 recovery executable, with accepted Test109 gameplay and the original watcher. The user confirmed startup works. Menu and battle checks verified loaded backing hashes, actual debugger attachment, CPU/memory/GPU/VRAM, OS input, lifecycle sampling and initialized native input. The user subsequently reported finishing the battle. Diagnostics111 binaries are uninstalled; no live debugger replacement, additional attachment, watcher restart, gameplay/settings change or publication occurred.

## Reproduced defect

Both earlier startup captures had a single-step trap with DR6 bit3 set, actual DR3 equal to the initial observed audio interface, and stored programmed DR3 already zero. The old handler knew only the latest register assignment. It forwarded its own restored trap as an unknown exception; the application did not handle it.

A new toy reproduces this deterministically without writing its own debug registers. A worker enters a native exception filter while the interface is watched. Another thread clears the wrapper pointer, causing the debugger to retire that interface watch. Returning EXCEPTION_CONTINUE_EXECUTION restores the worker's saved exception context, including the retired DR3 binding. The next interface write traps. The withdrawn debugger reproduces the actual DR6/DR7/DR3-versus-programmed disagreement and an unhandled0x80000004 termination. Concurrent writes alone did not reproduce it in the initial twelve trials.

This establishes a concrete context-restoration failure mechanism and the ownership defect. The exact Windows callback/continuation path that restored state inside the real audio subsystem is still untraced; the toy does not prove that the game used this particular exception-filter path. [Microsoft exception dispatching](https://learn.microsoft.com/en-us/windows/win32/debug/exception-dispatching) documents saved context and continuation; [GetExceptionInformation](https://learn.microsoft.com/en-us/windows/win32/Debug/getexceptioninformation) exposes that saved state. [Processor-breakpoint guidance](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/processor-breakpoints---ba-breakpoints-) discourages manually editing debug registers. These tools remain separate from investigation of the longstanding native battle crashes.

## Containment and correction

The default recorder compiles CaptureOnly111 and does not include the hardware tracer. Its PE imports are checked for absence of thread-context writes, remote memory writes, remote allocation/protection and explicit thread suspension/resumption. A compiled capture-only capability is required before the deployment/installation helpers mutate anything. Experimental tracing has a different executable name and build define; it refuses real-game mode and requires the exact adjacent toy path and a compile-time toy SHA256, before attaching. Supplying the fixture flag to an arbitrary process is insufficient.

Experimental ownership now retains up to64 proven bindings per processor slot for each tracked thread, until thread exit. A retired binding is accepted only when the actual slot address, local enable/type/width and execution address where applicable match that history. Foreign/combined trap conditions remain native exceptions. History is never evicted to admit another binding: reaching the bound stops hardware observations while preserving ownership needed for restored contexts. The current event thread is forcibly reprogrammed after an owned trap; cached desired-register equality cannot prove the actual restored context.

These changes remove hardware instrumentation from the normal build and constrain the experimental build. They do not prove every possible OS, debugger, hardware or logging failure impossible. Experimental tracing remains toy-only and is not enabled on the game by this work.

## Build and verification

Run tools/diagnostics/build111.py with explicit MSVC vcvars and a new output directory below artifacts/. The default builds capture-only tools. The optional experimental flag additionally builds the toy and its fingerprint-locked debugger. Building never installs or launches the game.

Run source/verify_hardening111.py with the build directory, a new evidence directory, and a separately configured support root supplying the existing private session/runtime inputs. An optional baseline-debugger argument exercises the preserved withdrawn debugger against the deterministic toy. It deliberately reproduces a toy crash and retains that evidence.

Coverage includes84 ownership assertions, twelve restored-context cases, twelve concurrent retirement cases, bounded-history exhaustion, six existing native hardware cases, four full core-session cases, native handled single-step/breakpoint/custom exceptions, unhandled illegal instruction,128-thread churn, forced debugger exit, duplicate attachment, log/dump/health write failures, unavailable output, malformed binary rejection and pre-mutation deployment rejection. Separate path and fingerprint mismatches must be rejected before attachment. Read-only lifecycle coverage remains selected-unit/followed-actor coverage; live partial-read transitions are reported rather than silently labeled complete tracing.

The initial missing C++ include and the first history-bound fixture attempt are retained. That fixture reached the existing work budget before the ownership limit; pacing only the toy's writes separated those two limits without raising the debugger budget. Final source/build hashes, exact test outcomes and preservation checks are recorded separately. The frozen v0.19 packages and published v0.18 are unchanged.
