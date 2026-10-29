# Test173 — remove F12 session disable

F12 no longer shuts down possession or QTE, releases the possessed soldier, or
temporarily yields camera ownership. F12 is forwarded normally; F10 and Escape
retain the existing return-to-RTS behavior. Internal shutdown/failure handling
remains intact.

Only the adapter and playable QTE input code changed. The generated Test172
engine contract, hook checks, layout/ABI assumptions, role and lifetime rules,
movement, weapons, native commands and other gameplay mechanics are unchanged.
The source ledger was refreshed after reviewing those two local input changes.
Two removed numeric candidates were 0x8000 key-state masks, not engine addresses.
No decoder/Ghidra rerun or new native binding was needed.

Current README/help text and future generated launcher metadata omit the F12
disable instruction. Versioned release notes and historical build guides retain
their original descriptions. The Workshop pack carries PLAYING173.md.

The updated pack uses the existing main shogun2_unit_possession_workshop165
entry. Installation replaces only its verified pack and preserves exact Test172
bytes for rollback. No Workshop upload, GitHub publication, game launch,
recording or debugger startup is part of this change.

See TEST173_VALIDATION.json for final offline checks, installation and the
separate pending live-acceptance state.
