#!/usr/bin/env python3
"""G0: 16.1 BinaryData contract gate. Scope-corrected: includes are searched
in Source/** and tests/** (NOT in CMakeLists text) - fixes false FAIL of run #839 class."""
import os, re, sys
REQUIRED = [
    ("PPGUData", "PPGUData.h", "BinaryData"),
    ("PPGUTestData", "PPGUTestData.h", "PPGUTestData"),
]
def main(root="."):
    cm = open(os.path.join(root, "CMakeLists.txt"), encoding="utf-8").read()
    if "HEADER_NAMESPACE" in cm:
        print("FAIL: forbidden HEADER_NAMESPACE present"); return 1
    srcs = []
    for base in ("Source", "tests"):
        full = os.path.join(root, base)
        for dp, _, fns in os.walk(full):
            srcs += [os.path.join(dp, f) for f in fns if f.endswith((".cpp", ".h"))]
    allsrc = ""
    for s in srcs: allsrc += open(s, encoding="utf-8", errors="ignore").read()
    targets = []
    for tname, header, ns in REQUIRED:
        m = re.search(r"juce_add_binary_data\(" + re.escape(tname) + r"\b(.*?)\)", cm, re.S)
        if not m: print(f"FAIL: target {tname} missing"); return 1
        block = m.group(1)
        if f"HEADER_NAME {header}" not in block: print(f"FAIL: {tname}: HEADER_NAME {header} missing"); return 1
        if f"NAMESPACE {ns}" not in block: print(f"FAIL: {tname}: NAMESPACE {ns} missing"); return 1
        targets.append((tname, m.start()))
    if targets[0][1] >= targets[1][1]:
        print("FAIL: PPGUData must be declared before PPGUTestData"); return 1
    for tname, header, ns in REQUIRED:
        if f"include <{header}>" not in allsrc and f'include "{header}"' not in allsrc:
            print(f"FAIL: header {header} never included in Source/** or tests/**"); return 1
    print("PASS: binary data contract (unique headers, pre-declared, includes in Source/tests scope)")
    return 0
if __name__ == "__main__":
    sys.exit(main(sys.argv[1] if len(sys.argv) > 1 else "."))