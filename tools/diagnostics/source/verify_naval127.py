"""Offline byte fixtures for the naval reader; no game/process access."""
import unittest
import struct

from naval_roster127 import (
    decode_roster, decode_deck_surface, DECK_SURFACE_TABLE, DecodeError, ENGINE_HASHES, SHIP_TABLE, REGISTRY_TABLE,
    OFFICER_HOLDER_TABLE, COHORTS, MAX_CREW_PER_COHORT, _Reader,
)


class Fixture:
    engine = 0x10000000
    ship = 0x200000
    environment = 0x400000
    army = 0x410000

    def __init__(self):
        self.memory = {}
        self.reads = 0
        self.args = dict(engine_base=self.engine, engine_sha256=next(iter(ENGINE_HASHES)),
                         battle_generation=7, ship=self.ship,
                         expected_environment=self.environment, expected_army=self.army)
        self.u32(self.ship, self.engine + SHIP_TABLE)
        self.u32(self.ship + 0x1D8, 5)
        self.u32(self.ship + 0x1E8, self.environment)
        self.u32(self.ship + 0x4B0, self.army)
        self.actors = [0x600000, 0x610000, 0x620000, 0x630000]
        self.registries = [0x500000, 0x510000, 0x520000]
        for index, (_, offset) in enumerate(COHORTS):
            registry = self.registries[index]
            self.u32(self.ship + offset, registry)
            self.u32(registry, self.engine + REGISTRY_TABLE)
            self.u32(registry + 4, self.environment)
            self.u32(registry + 0x510, index)
            self.array(registry + 0x14, 0x700000 + index * 0x1000, [self.actors[index]])
            self.actor(self.actors[index], index, registry)
        self.actor(self.actors[3], 3, 0)
        self.u32(self.actors[3], self.engine + 0x15DCD60)
        holder = 0x710000
        self.u32(holder, self.engine + OFFICER_HOLDER_TABLE)
        self.u32(holder + 0xC, self.actors[3])
        self.u32(holder + 0x10, self.ship)
        self.array(self.ship + 0x694, 0x720000, [holder])

    def put(self, address, data):
        self.memory.update((address + i, value) for i, value in enumerate(data))

    def u32(self, address, value):
        self.put(address, struct.pack("<I", value))

    def array(self, header, address, entries):
        self.put(header, struct.pack("<III", len(entries), len(entries), address))
        if entries:
            self.put(address, struct.pack(f"<{len(entries)}I", *entries))

    def actor(self, address, serial, registry):
        for offset, value in {
            0: self.engine + 0x15DFEC8, 0x10: 0x800000, 0x18: 0, 0x28: self.environment,
            0x98: 0x810000, 0x1C8: serial, 0x1CC: 0x420048,
            0x20C: 0x900000 + serial * 0x1000, 0x214: 0, 0x218: serial,
            0x21C: self.ship, 0x3B8: registry, 0x600: 0,
        }.items():
            self.u32(address + offset, value)
        self.put(address + 0xF0, b"\x01")
        self.put(address + 0x384, b"\x00")
        self.put(address + 0x48, struct.pack("<fff", 2.0, 12.0, -4.0))

    def read(self, address, size):
        self.reads += 1
        return bytes(self.memory[address + i] for i in range(size))

    def decode(self, read=None, **changes):
        return decode_roster(read or self.read, **dict(self.args, **changes))


class NavalRosterTests(unittest.TestCase):
    def setUp(self):
        self.f = Fixture()

    def test_three_cohorts_plus_officer(self):
        result = self.f.decode()
        self.assertEqual(result.cohort_counts, (1, 1, 1))
        self.assertEqual([a.sources for a in result.actors],
                         [("marines",), ("seamen",), ("gunners",), ("officer",)])
        self.assertEqual(result.actors[0].identity.serial, 0)
        self.assertEqual(result.actors[0].position, (2, 12, -4))
        self.assertTrue(all(not a.notes for a in result.actors))
        self.assertLess(result.bytes_read, 10000)

    def test_reviewed_bugfix_same_layout(self):
        result = self.f.decode(engine_sha256=list(ENGINE_HASHES)[1])
        self.assertEqual(result.engine_profile, "reviewed_bugfix")

    def test_unknown_engine_denied_before_reads(self):
        with self.assertRaises(DecodeError):
            self.f.decode(engine_sha256="0" * 64)
        self.assertEqual(self.f.reads, 0)

    def test_invalid_context_denied_before_reads(self):
        for changes in ({"battle_generation": 0}, {"ship": 0},
                        {"engine_base": 0xFFFFF000}, {"expected_army": 3}):
            with self.subTest(changes=changes), self.assertRaises(DecodeError):
                self.f.decode(**changes)
        self.assertEqual(self.f.reads, 0)

    def test_wrong_ship_or_army_or_environment(self):
        for offset, value in ((0, self.f.engine + 0x15DFEC8),
                              (0x4B0, 0xDEAD0000), (0x1E8, 0xDEAD0000), (0x1D8, 0)):
            f = Fixture()
            f.u32(f.ship + offset, value)
            with self.subTest(offset=offset), self.assertRaises(DecodeError):
                f.decode()

    def test_all_registries_required(self):
        self.f.u32(self.f.ship + 0x5CC, 0)
        with self.assertRaises(DecodeError):
            self.f.decode()

    def test_bad_registry_type_environment_index(self):
        for offset, value in ((0, 1), (4, 0xF0000), (0x510, 9)):
            f = Fixture()
            f.u32(f.registries[0] + offset, value)
            with self.subTest(offset=offset), self.assertRaises(DecodeError):
                f.decode()

    def test_count_capacity_and_diagnostic_bounds(self):
        for capacity, count in ((0, 1), (1, 0xFFFFFFFF),
                                (MAX_CREW_PER_COHORT + 1, 0)):
            f = Fixture()
            f.put(f.registries[0] + 0x14, struct.pack("<III", capacity, count, 0x700000))
            with self.subTest(count=count), self.assertRaises(DecodeError):
                f.decode()
            self.assertLess(f.reads, 20)

    def test_empty_rosters_are_valid(self):
        for registry in self.f.registries:
            self.f.array(registry + 0x14, 0, [])
        self.f.array(self.f.ship + 0x694, 0, [])
        self.assertEqual(self.f.decode().actors, ())

    def test_array_wraparound(self):
        self.f.put(self.f.registries[0] + 0x14, struct.pack("<III", 2, 2, 0xFFFFFFFC))
        with self.assertRaises(DecodeError):
            self.f.decode()

    def test_null_entry_not_silently_omitted(self):
        self.f.u32(0x700000, 0)
        with self.assertRaises(DecodeError):
            self.f.decode()

    def test_duplicate_in_one_roster_rejected(self):
        self.f.array(self.f.registries[0] + 0x14, 0x700000, [self.f.actors[0]] * 2)
        with self.assertRaises(DecodeError):
            self.f.decode()

    def test_shared_actor_between_sources_keeps_provenance(self):
        self.f.u32(0x710000 + 0xC, self.f.actors[0])
        result = self.f.decode()
        self.assertEqual(len(result.actors), 3)
        self.assertEqual(result.actors[0].sources, ("marines", "officer"))

    def test_wrong_officer_holder_rejected(self):
        for offset, value in ((0, 0x12345678), (0x10, 0x220000)):
            f = Fixture()
            f.u32(0x710000 + offset, value)
            with self.subTest(offset=offset), self.assertRaises(DecodeError):
                f.decode()

    def test_decorative_or_unknown_actor_rejected(self):
        self.f.u32(self.f.actors[0], self.f.engine + 0x15DDB2C)
        with self.assertRaises(DecodeError):
            self.f.decode()

    def test_dead_actor_is_reported_not_certified(self):
        self.f.put(self.f.actors[0] + 0xF0, b"\x00")
        actor = self.f.decode().actors[0]
        self.assertFalse(actor.alive)
        self.assertIn("not alive", actor.notes)

    def test_original_owner_and_support_not_conflated(self):
        a = self.f.actors[0]
        self.f.u32(a + 0x21C, 0x230000)
        self.f.u32(a + 0x10, 0x820000)
        self.f.u32(a + 0x18, 0x830000)
        actor = self.f.decode().actors[0]
        self.assertEqual(actor.identity.original_ship, 0x230000)
        self.assertEqual(actor.surface, 0x820000)
        self.assertEqual(actor.traversal, 0x830000)
        self.assertTrue(any("original owner differs" in n for n in actor.notes))

    def test_missing_surface_not_guessed(self):
        self.f.u32(self.f.actors[0] + 0x10, 0)
        actor = self.f.decode().actors[0]
        self.assertIn("no current walking surface", actor.notes)
        self.assertEqual(actor.surface, 0)

    def test_land_owner_is_not_cast_to_ship(self):
        self.f.u32(self.f.actors[0] + 0x214, 0xA00000)
        self.assertTrue(any("land owner" in n for n in self.f.decode().actors[0].notes))

    def test_registry_mismatch_reported(self):
        self.f.u32(self.f.actors[0] + 0x3B8, self.f.registries[1])
        self.assertTrue(any("current registry differs" in n for n in self.f.decode().actors[0].notes))

    def test_duplicate_serial_rejected(self):
        self.f.u32(self.f.actors[1] + 0x1C8, 0)
        with self.assertRaises(DecodeError):
            self.f.decode()

    def test_new_generation_or_reused_actor_has_different_identity(self):
        old = self.f.decode().actors[0].identity
        self.assertNotEqual(old, self.f.decode(battle_generation=8).actors[0].identity)
        self.f.u32(self.f.actors[0] + 0x1C8, 10)
        self.assertNotEqual(old, self.f.decode().actors[0].identity)

    def test_station_reservation_is_independent_and_opaque(self):
        actor = self.f.actors[0]
        self.f.u32(actor + 0x600, 0xA00000)
        before = dict(self.f.memory)
        result = self.f.decode().actors[0]
        self.assertEqual(result.station_handle, 0xA00000)
        self.assertEqual(result.identity.original_ship, self.f.ship)
        self.assertEqual(result.current_crew_registry, self.f.registries[0])
        self.assertEqual(result.surface, 0x800000)
        self.assertEqual(self.f.memory, before)

    def test_changing_station_reservation_refuses_inconsistent_capture(self):
        address = self.f.actors[0] + 0x600
        calls = 0
        def changing_read(at, size):
            nonlocal calls
            if at == address:
                calls += 1
                if calls == 2:
                    self.f.u32(address, 0xA00000)
            return self.f.read(at, size)
        with self.assertRaisesRegex(DecodeError, "changed during capture"):
            self.f.decode(read=changing_read)

    def test_active_task_observed_without_executing_it(self):
        a = self.f.actors[0]
        self.f.put(a + 0x384, b"\x01")
        self.f.u32(a + 0x2F8, self.f.engine + 0x15DD500)
        self.f.put(a + 0x308, b"\x00")
        actor = self.f.decode().actors[0]
        self.assertTrue(actor.task_present)
        self.assertFalse(actor.task_finished)
        self.assertEqual(actor.task_table_rva, 0x15DD500)

    def test_malformed_flags_or_position_rejected(self):
        for offset, data in ((0xF0, b"\x02"), (0x384, b"\xFF"),
                             (0x48, struct.pack("<fff", float("nan"), 0, 0))):
            f = Fixture()
            f.put(f.actors[0] + offset, data)
            with self.subTest(offset=offset), self.assertRaises(DecodeError):
                f.decode()

    def test_unreadable_or_short_read_rejected(self):
        with self.assertRaises(DecodeError):
            self.f.decode(read=lambda address, size: b"")
        del self.f.memory[self.f.actors[0] + 0x1C8]
        with self.assertRaises(DecodeError):
            self.f.decode()

    def test_actor_changes_during_capture(self):
        address = self.f.actors[0] + 0x1C8
        calls = 0
        def read(where, size):
            nonlocal calls
            if where == address:
                calls += 1
                if calls == 2:
                    self.f.u32(address, 45)
            return self.f.read(where, size)
        with self.assertRaisesRegex(DecodeError, "changed during capture"):
            self.f.decode(read=read)

    def test_array_changes_during_capture(self):
        calls = 0
        def read(where, size):
            nonlocal calls
            if where == 0x700000:
                calls += 1
                if calls == 2:
                    self.f.u32(where, self.f.actors[1])
            return self.f.read(where, size)
        with self.assertRaisesRegex(DecodeError, "changed during capture"):
            self.f.decode(read=read)

    def test_bounded_reader_budget(self):
        memory = _Reader(lambda address, size: bytes(size))
        with self.assertRaisesRegex(DecodeError, "budget"):
            memory.get(0x10000, 4 * 1024 * 1024 + 1)



class DeckSurfaceTests(unittest.TestCase):
    def setUp(self):
        self.f = Fixture()
        self.surface = 0x800000
        self.provider = 0x810000
        # Independent physical ship: deliberately not the crew's original ship.
        self.support_ship = 0x220000
        self.f.u32(self.surface, self.f.engine + DECK_SURFACE_TABLE)
        self.f.u32(self.surface + 0x24, self.f.environment)
        self.f.u32(self.surface + 0x30, 0)
        self.f.u32(self.surface + 0x80, self.provider)
        self.f.u32(self.surface + 0xA4, self.support_ship)
        self.f.u32(self.support_ship, self.f.engine + SHIP_TABLE)
        self.f.u32(self.support_ship + 0x1E8, self.f.environment)
        self.f.u32(self.support_ship + 0x1D8, 10)
        self.f.u32(self.support_ship + 0x764, self.provider)
        self.args = dict(engine_base=self.f.engine,
                         engine_sha256=next(iter(ENGINE_HASHES)),
                         battle_generation=7, surface=self.surface,
                         expected_environment=self.f.environment)

    def decode(self, read=None, **changes):
        return decode_deck_surface(read or self.f.read, **dict(self.args, **changes))

    def test_physical_support_differs_from_original_crew_owner(self):
        before = dict(self.f.memory)
        actor = self.f.decode().actors[0]
        surface = self.decode()
        self.assertEqual(actor.identity.original_ship, self.f.ship)
        self.assertEqual(surface.supporting_ship, self.support_ship)
        self.assertEqual(surface.transport_provider, self.provider)
        self.assertEqual(surface.surface_serial, 0)
        self.assertLess(surface.bytes_read, 256)
        self.assertEqual(before, self.f.memory)

    def test_unknown_surface_not_cast_to_ship(self):
        # Generic movable surface uses the base class and is not a naval ship.
        self.f.u32(self.surface, self.f.engine + 0x15DE2CC)
        with self.assertRaises(DecodeError):
            self.decode()
        self.assertEqual(self.f.reads, 1)

    def test_support_identity_must_match_environment(self):
        for address in (self.surface + 0x24, self.support_ship + 0x1E8):
            with self.subTest(address=address):
                self.f.u32(address, 0x430000)
                with self.assertRaises(DecodeError):
                    self.decode()
                self.f.u32(address, self.f.environment)

    def test_provider_mismatch_rejected(self):
        self.f.u32(self.support_ship + 0x764, 0x820000)
        with self.assertRaises(DecodeError):
            self.decode()

    def test_unregistered_or_wrong_class_support_rejected(self):
        for address, replacement in ((self.support_ship + 0x1D8, 0),
                                      (self.support_ship, self.f.engine + 0x15DFEC8)):
            original = self.f.read(address, 4)
            self.f.u32(address, replacement)
            with self.subTest(address=address), self.assertRaises(DecodeError):
                self.decode()
            self.f.put(address, original)

    def test_support_replaced_during_capture_rejected(self):
        calls = 0
        def changing(address, size):
            nonlocal calls
            if address == self.surface + 0xA4:
                calls += 1
                if calls == 2:
                    return struct.pack('<I', self.f.ship)
            return self.f.read(address, size)
        with self.assertRaises(DecodeError):
            self.decode(changing)

    def test_surface_generation_or_serial_change_changes_identity(self):
        before = self.decode()
        self.assertNotEqual(before, self.decode(battle_generation=8))
        self.f.u32(self.surface + 0x30, 3)
        self.assertNotEqual(before.surface_serial, self.decode().surface_serial)

    def test_invalid_context_denied_before_reads(self):
        for change in ({'engine_sha256': '0' * 64}, {'battle_generation': 0},
                       {'surface': 0}, {'expected_environment': 7},
                       {'engine_base': 0xFFFFF000}):
            with self.subTest(change=change), self.assertRaises(DecodeError):
                self.decode(**change)
        self.assertEqual(self.f.reads, 0)

if __name__ == "__main__":
    unittest.main(verbosity=2)
