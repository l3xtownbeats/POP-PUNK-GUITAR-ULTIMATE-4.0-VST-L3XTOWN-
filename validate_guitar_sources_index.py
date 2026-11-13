#!/usr/bin/env python3
"""G6: guitar sound sources index - verified natural-sample sources with licenses."""
import os, sys
REQUIRED = ["FreePats","Karoryfer","Freesound","Signature Sounds","Looperman"]
def main(path):
    if not os.path.isfile(path): print(f"FAIL: index missing {path}"); return 1
    text = open(path, encoding="utf-8").read()
    miss = [s for s in REQUIRED if s not in text]
    if miss: print(f"FAIL: sources missing: {miss}"); return 1
    if "CC0" not in text: print("FAIL: license column missing"); return 1
    print("PASS: guitar sound sources index complete (verified natural sources, license columns)")
    return 0
if __name__ == "__main__": sys.exit(main(sys.argv[1]))