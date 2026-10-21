# Test151 — cursor selection, shared HUD visibility and battle transition QTEs

At the stock RTS camera minimum zoom, F9 now uses the actual native mouse cursor rather than screen center. The first physical hit still wins; eligibility remains within the selected unit, including reciprocal horse/rider links. High views and invalid hits retain prior selection.

K uses the game's native Hide UI state for both RTS and first-person HUD visibility. Leaving possession reapplies the current native state instead of an old widget snapshot. QTEs and their mouse-button prompt remain visible.

The saved Test150 session reused battle environment/phase addresses while resetting the native model tick from13669 to4386. Its QTE timeline retained the old identity and rejected subsequent encounters. Test151 tracks current native battle clock lifetimes, confirms live counter resets, and assigns new epochs. Native timing, pause, stale input, ownership, charge priority, outcomes and encounter guards remain intact.

Offline regression reproduces the old admission failure, then opens QTEs across100 reused-address battles. Actual cursor unprojection, keyboard hook and UI exit fixtures cover the corrections. Render device/geometry/allocation and native UI tree providers are explicit; actual mouse choice, hide/show and multi-battle siege QTE behavior need fresh loaded verification and play.

Unpublished development0.27. Published v0.27/433815b remains unchanged. No settings, CPU/affinity or recorder configuration changes. Automatic recording stays armed onD:. Installation requires the game closed; a fresh READY and exact loaded component/capture checks precede testing.
