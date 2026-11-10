# Naval crew possession: Revision 127 private test

This is a PC keyboard/mouse, single-player naval walking and return-to-duty test.
Select one friendly ship, press F9 or Caps Lock, use WASD and Shift, look with the
mouse, and press F10 to return. Start the battle first. Normal marines and seamen
on their own ship are eligible; gunners, officers, traversal and priority tasks
are excluded. Personal weapon combat, boarding, naval QTE and Deck controls are
not implemented in this candidate. Existing land possession is retained.

## Native evidence

The owner supplied a selected-ship PC naval battle. Two bounded read-only captures
confirmed 134 living crew, separate marine/seaman/gunner/officer registries, current
duty tasks and reserved stations. No new live reads were performed for this build.
The underlying naval card view contained 150 entries, so it is not used as a crew
array. The selected-ship table, environment and single-human army are validated.

Focused Ghidra exports and exact bytes were compared against the owned stock and
reviewed Bugfix engines. Addresses below are RVAs, not a portable engine API:

- 3efc70 dispatches an ordinary task context. Only the chosen sailor's actor+2f0
  context is held; all other crew and priority contexts take the original path.
- 3dea90 uses ordinary started=0 after native priority work. The candidate keeps
  the same restart flag armed while it temporarily installs lower navigation.
- 350d10 constructs the 65-byte ordinary walk request; 3b91c0 converts it to the
  distinct 61-byte lower request; 3d9320 installs it in the chosen sailor only.
- 37c090 projects a short destination through the native navigation manager. Its
  thiscall ABI and four stack arguments are confirmed by the caller at 3e97e8.
  It runs on the actor update thread, never on the input worker or renderer.
- 3df250 reads the cached current Location at actor+524. Writer 3c6dc0 obtains it
  from 3c8230, which builds from the actor's world X/Z and current support surface
  when not in traversal. 399f80 / 36e710 resolve its moving ship frame.
- Native ordinary walk start 382670 / 391900 restores the retained task's station
  destination after release. No station release, roster removal, ship movement
  command, AI objective or manually written actor position is used.
- Ship transport 3a8900 remains native. The overhead camera's numeric pose is
  temporarily attached to the actor and restored; no land soldier lease is forged.

The runtime validates actor identity, registry membership, duty generation,
station owner, environment and camera lifetime. Death, task replacement, priority
work, traversal, focus loss and shutdown release control. It does not resurrect
old duty bytes or restore a cached station pointer over a new native owner.

## Verification and limits

verify_naval_tasks127.cpp executes native task/navigation routines in private
mapped stock/Bugfix images. verify_naval_control127.cpp also executes the production
control header against synthetic ship/world/station fixtures and the native task
routines. Point projection and downstream path submission have explicit fixture
providers. These tests prove ABI and ownership transitions, not real deck
pathfinding, collisions, ship motion, camera appearance or battle stability.

Private Ghidra output, snapshots, engine images and extracted resources are not
part of the mod package. The first live naval test is still required. This build
must not be described as fully tested naval combat or published as such.

## Follow-up boundaries

The movement build does not admit naval actors into the land detach/weapon/QTE
paths. Naval physical support and roster ownership stay with their original ship;
priority, traversal and a changed duty release the player lease. Next capability
work must decode naval personal equipment/projectile ownership and prove weapon
origins in the moving ship frame before enabling combat. Crew switching then
needs a transaction that finishes the outgoing release before a new actor takes
control. Both remain unimplemented, not silently mapped to land methods.

The minimal next experiment is the staged PC test: select a friendly ship, enter,
walk, return, and repeat while the vessel is already moving. Check the sailor's
return and the other crew's continued work before expanding combat or boarding.
The one-sailor prototype is not described as completed naval possession.
