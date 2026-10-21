"""Bounded read-only PFH3 access for the supported original SHOGUN 2 packs."""
import hashlib
from pathlib import Path, PurePosixPath
import struct

ENGINE_SHA256 = '022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5'

def digest(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()

def require_game(game):
    engine = Path(game) / 'Empire.Retail.dll'
    if not engine.is_file() or digest(engine) != ENGINE_SHA256:
        raise ValueError('Unsupported original-game engine. Supply the legitimate Windows x86 installation whose Empire.Retail.dll SHA-256 is ' + ENGINE_SHA256 + '. Remasters and other builds are not supported.')
    return engine

class Pack:
    def __init__(self, path):
        self.path = Path(path)
        self.entries = {}
        total = self.path.stat().st_size
        with self.path.open('rb') as stream:
            header = stream.read(32)
            if len(header) != 32:
                raise ValueError('Truncated pack header: ' + self.path.name)
            magic, flags, deps, dep_size, count, index_size, stamp = struct.unpack('<4s5IQ', header)
            if magic != b'PFH3' or flags not in (0, 1, 3) or deps or dep_size or count > 100000 or index_size > 16 * 1024 * 1024 or 32 + index_size > total:
                raise ValueError('Unsupported pack variant: ' + self.path.name)
            index = stream.read(index_size)
        cursor, offset = 0, 32 + index_size
        names = set()
        for _ in range(count):
            if cursor + 4 > len(index):
                raise ValueError('Truncated pack index')
            size = struct.unpack_from('<I', index, cursor)[0]
            cursor += 4
            end = index.find(b'\0', cursor)
            if end < 0:
                raise ValueError('Unterminated pack entry')
            name = index[cursor:end].decode('utf-8').replace('\\', '/')
            cursor = end + 1
            parts = PurePosixPath(name)
            if not name or parts.is_absolute() or ':' in name or any(p in ('..', '.') for p in name.split('/')) or name.casefold() in names or offset + size > total:
                raise ValueError('Unsafe/duplicate/out-of-bounds pack entry: ' + name)
            names.add(name.casefold())
            self.entries[name] = (offset, size)
            offset += size
        if cursor != index_size:
            raise ValueError('Pack index not fully consumed')

    def read(self, name, expected_size, expected_sha256):
        if name not in self.entries:
            raise ValueError('Missing game resource: ' + self.path.name + ':' + name)
        offset, size = self.entries[name]
        if size != expected_size or size > 32 * 1024 * 1024:
            raise ValueError('Game resource size differs: ' + name)
        with self.path.open('rb') as stream:
            stream.seek(offset)
            data = stream.read(size)
        if len(data) != size or hashlib.sha256(data).hexdigest() != expected_sha256:
            raise ValueError('Game resource fingerprint differs: ' + name)
        return data
