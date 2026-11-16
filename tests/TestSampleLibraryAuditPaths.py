#!/usr/bin/env python3
"""Contract tests for safe SFZ path suffix normalization."""
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from audit_sample_library import normalize_suffix, ASSIGNMENT


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
    # SFZ generator opcodes are not sample-file paths.
    import re
    opcodes = [next((v for v in m.groups()[1:] if v is not None), "")
               for m in ASSIGNMENT.finditer(
                   'sample=*sine\\n sample="Samples/guitar/note.wav"\\n'
               ) if m.group(1).lower() == "sample"]
    if opcodes != ["*sine", "Samples/guitar/note.wav"]:
        raise AssertionError(f"Unexpected SFZ sample opcodes: {opcodes!r}")
    print(f"SFZ path/opcode contract: {len(cases)} path cases and {len(opcodes)} opcode cases passed")


if __name__ == "__main__":
    main()
