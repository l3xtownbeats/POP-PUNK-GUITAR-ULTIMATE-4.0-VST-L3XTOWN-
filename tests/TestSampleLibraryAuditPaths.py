#!/usr/bin/env python3
"""Contract tests for safe SFZ path suffix normalization."""
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from audit_sample_library import normalize_suffix


def main():
    cases = {
        "../Samples/black/btb/note.wav": "Samples/black/btb/note.wav",
        "..\\Samples\\black\\btb\\note.wav": "Samples/black/btb/note.wav",
        "Programs/../Samples/guitar/note.wav": "Samples/guitar/note.wav",
        "./Samples//guitar/note.wav": "Samples/guitar/note.wav",
        "../../Samples/guitar/note.wav": "Samples/guitar/note.wav",
    }
    for source, expected in cases.items():
        actual = normalize_suffix(source)
        if actual != expected:
            raise AssertionError(f"{source!r}: expected {expected!r}, got {actual!r}")
    print(f"SFZ path normalization contract: {len(cases)} cases passed")


if __name__ == "__main__":
    main()
