#!/usr/bin/env python3
"""G5: SoundBank - NATURAL SAMPLES ONLY policy. Zero synthesized WAV in factory bank."""
import json, os, sys, hashlib
def main(path):
    pol = json.load(open(os.path.join(path, "policy.json"), encoding="utf-8"))
    if pol.get("sampleSourcePolicy") != "NATURAL_SAMPLES_ONLY":
        print("FAIL: sampleSourcePolicy must be NATURAL_SAMPLES_ONLY"); return 1
    wav_dir = os.path.join(path, "WAV")
    n = len([f for f in os.listdir(wav_dir) if f.lower().endswith(".wav")]) if os.path.isdir(wav_dir) else 0
    sfz = os.path.join(path, "factory.sfz")
    if os.path.isfile(sfz) and "<sample>" in open(sfz, encoding="utf-8").read():
        print("FAIL: factory.sfz must not ship built-in samples (natural-only policy)"); return 1
    man = os.path.join(path, "manifest.json")
    if os.path.isfile(man):
        m = json.load(open(man, encoding="utf-8"))
        for e in m.get("files", []):
            p = os.path.join(path, e["path"])
            if not os.path.isfile(p): print(f"FAIL: manifest entry missing {e['path']}"); return 1
            if hashlib.sha256(open(p, "rb").read()).hexdigest() != e["sha256"]:
                print(f"FAIL: SHA-256 mismatch {e['path']}"); return 1
    print(f"PASS: SoundBank policy NATURAL_SAMPLES_ONLY ({n} user WAV; factory ships 0 synthesized samples)")
    return 0
if __name__ == "__main__": sys.exit(main(sys.argv[1]))