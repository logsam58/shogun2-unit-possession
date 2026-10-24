"""Build an isolated native development payload; never install or launch."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from build_frontend import main

if __name__ == "__main__":
    main()
