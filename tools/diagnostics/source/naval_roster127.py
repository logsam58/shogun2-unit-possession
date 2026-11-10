"""Bounded, read-only naval roster decoder for captured Shogun 2 memory.

Research support, NOT a possession implementation or a live-process collector.
The caller supplies exact reads, a separately verified engine identity, and a
battle generation. No native calls, hooks, writes, process handles or searches
are made here. See docs/research/NAVAL127.md for the decoded evidence/limits.
"""
from dataclasses import dataclass
import math
import struct
from typing import Callable

from actor_roles import HUMAN_TABLES

ENGINE_HASHES = {
    "022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5": "stock",
    "c4d91d74ce9ff8338cf61d5b021b03a2776830177a590fcac205509a42239545": "reviewed_bugfix",
}
SHIP_TABLE = 0x15DDB60
REGISTRY_TABLE = 0x15BB22C
OFFICER_HOLDER_TABLE = 0x15DDB2C
DECK_SURFACE_TABLE = 0x15DE30C
COHORTS = (("marines", 0x578), ("seamen", 0x5CC), ("gunners", 0x620))
MAX_CREW_PER_COHORT = 2048  # diagnostic resource bounds, not engine roster limits
MAX_OFFICERS = 128
MAX_UNIQUE_ACTORS = 4096
MAX_BYTES = 4 * 1024 * 1024
MAX_READS = 100000


class DecodeError(ValueError):
    """The supplied snapshot does not satisfy the reviewed read contract."""


@dataclass(frozen=True)
class ActorIdentity:
    battle_generation: int
    address: int
    table_rva: int
    serial: int
    actor_registry: int
    configuration: int
    original_ship: int
    owner_index: int


@dataclass(frozen=True)
class CrewActor:
    identity: ActorIdentity
    sources: tuple[str, ...]
    alive: bool
    land_owner: int
    current_crew_registry: int
    surface: int
    traversal: int
    transport: int
    station_handle: int
    position: tuple[float, float, float]
    task_present: bool
    task_table_rva: int | None
    task_finished: bool | None
    notes: tuple[str, ...]


@dataclass(frozen=True)
class RosterSnapshot:
    engine_profile: str
    battle_generation: int
    ship: int
    ship_serial: int
    environment: int
    army: int
    actors: tuple[CrewActor, ...]
    cohort_counts: tuple[int, ...]
    bytes_read: int
    reads: int


class _Reader:
    def __init__(self, read: Callable[[int, int], bytes]):
        self.read = read
        self.bytes_read = 0
        self.reads = 0
        self.guards: dict[tuple[int, int], bytes] = {}

    def get(self, address: int, size: int, *, stable: bool = False) -> bytes:
        if (not isinstance(address, int) or address < 0x10000 or size < 1
                or address + size > 0x100000000):
            raise DecodeError("Invalid 32-bit memory range")
        self.bytes_read += size
        self.reads += 1
        if self.bytes_read > MAX_BYTES or self.reads > MAX_READS:
            raise DecodeError("Snapshot read budget exceeded")
        try:
            data = self.read(address, size)
        except (OSError, ValueError, KeyError) as exc:
            raise DecodeError(f"Unreadable memory at {address:#x}") from exc
        if not isinstance(data, bytes) or len(data) != size:
            raise DecodeError(f"Short/non-byte read at {address:#x}")
        if stable:
            key = address, size
            if key in self.guards and self.guards[key] != data:
                raise DecodeError("Identity or roster changed during capture")
            self.guards[key] = data
        return data

    def u32(self, address: int, *, stable: bool = True) -> int:
        return struct.unpack("<I", self.get(address, 4, stable=stable))[0]

    def pointer(self, address: int, *, nullable: bool = False) -> int:
        value = self.u32(address)
        if value == 0 and nullable:
            return value
        if value < 0x10000 or value & 3:
            raise DecodeError(f"Invalid pointer at {address:#x}")
        return value

    def flag(self, address: int) -> bool:
        value = self.get(address, 1, stable=True)[0]
        if value not in (0, 1):
            raise DecodeError(f"Invalid boolean at {address:#x}")
        return bool(value)

    def array(self, address: int, limit: int) -> tuple[int, ...]:
        # CA dynamic-array capacity/count/data at +0/+4/+8.
        capacity, count, data = struct.unpack("<III", self.get(address, 12, stable=True))
        if count > capacity or capacity > limit:
            raise DecodeError("Roster capacity/count exceeds diagnostic bounds")
        if count == 0:
            return ()
        if data < 0x10000 or data & 3:
            raise DecodeError("Invalid roster array pointer")
        result = struct.unpack(f"<{count}I", self.get(data, count * 4, stable=True))
        if any(value < 0x10000 or value & 3 for value in result):
            raise DecodeError("Null/invalid actor or holder in roster")
        if len(set(result)) != len(result):
            raise DecodeError("Duplicate entry within one native roster")
        return result

    def verify(self) -> None:
        # Change detection, not an atomic snapshot or a future possession lease.
        for (address, size), expected in tuple(self.guards.items()):
            if self.get(address, size) != expected:
                raise DecodeError("Identity or roster changed during capture")


def decode_roster(read: Callable[[int, int], bytes], *, engine_base: int,
                  engine_sha256: str, battle_generation: int, ship: int,
                  expected_environment: int, expected_army: int) -> RosterSnapshot:
    """Decode a caller-proven selected ship; never discover or possess one.

    The digest is an admission check on the caller's verified module metadata,
    not a hash of captured memory. All three registries must be initialized.
    Alive humans are reported for inspection, NOT certified combat-capable.
    Physical support pointers remain opaque here. decode_deck_surface can
    separately inspect the reviewed direct ship-deck surface class.
    """
    profile = ENGINE_HASHES.get(engine_sha256)
    if profile is None:
        raise DecodeError("Unreviewed engine image")
    if (engine_base < 0x10000 or engine_base & 0xFFF
            or engine_base + 0x1D88000 > 0x100000000):
        raise DecodeError("Invalid engine mapping")
    if not isinstance(battle_generation, int) or battle_generation < 1:
        raise DecodeError("A current battle generation is required")
    for value in (ship, expected_environment, expected_army):
        if value < 0x10000 or value & 3:
            raise DecodeError("Invalid supplied ship/environment/army")
    memory = _Reader(read)
    if memory.u32(ship) != engine_base + SHIP_TABLE:
        raise DecodeError("Selected object is not the reviewed ship class")
    serial = memory.u32(ship + 0x1D8)
    if serial == 0:
        raise DecodeError("Ship is not registered")
    if memory.pointer(ship + 0x1E8) != expected_environment:
        raise DecodeError("Ship belongs to another environment")
    if memory.pointer(ship + 0x4B0) != expected_army:
        raise DecodeError("Ship belongs to another army")
    sources: dict[int, list[str]] = {}
    counts = []
    registry_for_source = {}
    for index, (name, offset) in enumerate(COHORTS):
        registry = memory.pointer(ship + offset)
        if memory.u32(registry) != engine_base + REGISTRY_TABLE:
            raise DecodeError("Unexpected crew registry class")
        if memory.pointer(registry + 4) != expected_environment:
            raise DecodeError("Crew registry belongs to another environment")
        if memory.u32(registry + 0x510) != index:
            raise DecodeError("Crew cohort index disagrees with ship slot")
        entries = memory.array(registry + 0x14, MAX_CREW_PER_COHORT)
        counts.append(len(entries))
        registry_for_source[name] = registry
        for actor in entries:
            sources.setdefault(actor, []).append(name)
    for holder in memory.array(ship + 0x694, MAX_OFFICERS):
        if memory.u32(holder) != engine_base + OFFICER_HOLDER_TABLE:
            raise DecodeError("Unexpected naval officer holder")
        if memory.pointer(holder + 0x10) != ship:
            raise DecodeError("Officer holder belongs to another ship")
        sources.setdefault(memory.pointer(holder + 0xC), []).append("officer")
    if len(sources) > MAX_UNIQUE_ACTORS:
        raise DecodeError("Too many unique crew actors")
    actors = []
    seen_serials = set()
    for actor, memberships in sources.items():
        table = memory.u32(actor) - engine_base
        if table not in HUMAN_TABLES:
            raise DecodeError("Crew entry is not a reviewed human actor")
        if memory.pointer(actor + 0x28) != expected_environment:
            raise DecodeError("Actor belongs to another environment")
        actor_serial = memory.u32(actor + 0x1C8)  # zero is valid
        actor_registry = memory.pointer(actor + 0x1CC)
        serial_key = actor_registry, actor_serial
        if serial_key in seen_serials:
            raise DecodeError("Two actors claim the same native identity")
        seen_serials.add(serial_key)
        configuration = memory.pointer(actor + 0x20C)
        land_owner = memory.pointer(actor + 0x214, nullable=True)
        original_ship = memory.pointer(actor + 0x21C)
        owner_index = memory.u32(actor + 0x218)
        crew_registry = memory.pointer(actor + 0x3B8, nullable=True)
        notes = []
        if land_owner:
            notes.append("land owner present: naval control not established")
        if original_ship != ship:
            notes.append("original owner differs: boarding/ownership needs inspection")
        if any(name in registry_for_source and registry_for_source[name] != crew_registry
               for name in memberships):
            notes.append("current registry differs from one or more roster sources")
        surface = memory.pointer(actor + 0x10, nullable=True)
        traversal = memory.pointer(actor + 0x18, nullable=True)
        transport = memory.pointer(actor + 0x98, nullable=True)
        # Native station reservation handle; not a crew owner or supporting ship.
        # Kept opaque: never dereference, release or restore it in this reader.
        station_handle = memory.pointer(actor + 0x600, nullable=True)
        if not surface:
            notes.append("no current walking surface")
        if traversal:
            notes.append("native traversal active")
        alive = memory.flag(actor + 0xF0)
        if not alive:
            notes.append("not alive")
        position = struct.unpack("<fff", memory.get(actor + 0x48, 12))
        if not all(math.isfinite(value) for value in position):
            raise DecodeError("Nonfinite actor position")
        task_present = memory.flag(actor + 0x384)
        task_table = memory.u32(actor + 0x2F8) - engine_base if task_present else None
        task_finished = memory.flag(actor + 0x308) if task_present else None
        if task_table is not None and not 0 <= task_table < 0x1D88000:
            raise DecodeError("Task table lies outside the reviewed engine")
        identity = ActorIdentity(battle_generation, actor, table, actor_serial,
                                 actor_registry, configuration, original_ship, owner_index)
        actors.append(CrewActor(identity, tuple(memberships), alive, land_owner,
                                crew_registry, surface, traversal, transport, station_handle, position,
                                task_present, task_table, task_finished, tuple(notes)))
    memory.verify()
    return RosterSnapshot(profile, battle_generation, ship, serial,
                          expected_environment, expected_army, tuple(actors),
                          tuple(counts), memory.bytes_read, memory.reads)


@dataclass(frozen=True)
class DeckSurfaceSnapshot:
    engine_profile: str
    battle_generation: int
    surface: int
    surface_serial: int
    environment: int
    supporting_ship: int
    ship_serial: int
    transport_provider: int
    bytes_read: int
    reads: int


def decode_deck_surface(read: Callable[[int, int], bytes], *, engine_base: int,
                        engine_sha256: str, battle_generation: int, surface: int,
                        expected_environment: int) -> DeckSurfaceSnapshot:
    """Inspect one direct deck surface without calling a native virtual.

    0x35ecf0 constructs this concrete class; its virtual +0x18 returns this.
    It retains its physical ship at +0xa4 and ship+0x764 provider at +0x80.
    This is NOT the actor's original crew owner, a traversal resolver, or proof
    that a surface is presently walkable. Unknown surface classes are refused.
    No frame/motion is applied and no current-world transform is cached here.
    """
    profile = ENGINE_HASHES.get(engine_sha256)
    if profile is None:
        raise DecodeError("Unreviewed engine image")
    if (engine_base < 0x10000 or engine_base & 0xFFF
            or engine_base + 0x1D88000 > 0x100000000):
        raise DecodeError("Invalid engine mapping")
    if not isinstance(battle_generation, int) or battle_generation < 1:
        raise DecodeError("A current battle generation is required")
    for value in (surface, expected_environment):
        if value < 0x10000 or value & 3:
            raise DecodeError("Invalid supplied surface/environment")
    memory = _Reader(read)
    if memory.u32(surface) != engine_base + DECK_SURFACE_TABLE:
        raise DecodeError("Surface is not the reviewed direct ship-deck class")
    if memory.pointer(surface + 0x24) != expected_environment:
        raise DecodeError("Surface belongs to another environment")
    surface_serial = memory.u32(surface + 0x30)  # native constructor allows zero
    ship = memory.pointer(surface + 0xA4)
    provider = memory.pointer(surface + 0x80)
    if memory.u32(ship) != engine_base + SHIP_TABLE:
        raise DecodeError("Surface owner is not the reviewed ship class")
    if memory.pointer(ship + 0x1E8) != expected_environment:
        raise DecodeError("Supporting ship belongs to another environment")
    ship_serial = memory.u32(ship + 0x1D8)
    if not ship_serial:
        raise DecodeError("Supporting ship is not registered")
    if memory.pointer(ship + 0x764) != provider:
        raise DecodeError("Surface and ship transport providers disagree")
    memory.verify()
    return DeckSurfaceSnapshot(profile, battle_generation, surface, surface_serial,
                               expected_environment, ship, ship_serial, provider,
                               memory.bytes_read, memory.reads)
