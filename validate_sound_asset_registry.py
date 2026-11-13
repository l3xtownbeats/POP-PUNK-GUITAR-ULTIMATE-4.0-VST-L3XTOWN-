#!/usr/bin/env python3
"""G4: REV 17.0 sound asset registry policy (asset-level GREEN/YELLOW/RED/GOLD)."""
import json, sys
def main(path):
    reg = json.load(open(path, encoding="utf-8"))
    if not isinstance(reg.get("assets"), list): print("FAIL: assets list missing"); return 1
    seen = set()
    for a in reg["assets"]:
        aid = a.get("id")
        if not aid or aid in seen: print(f"FAIL: bad/duplicate id {aid}"); return 1
        seen.add(aid)
        v = a.get("verdict")
        if v not in ("GREEN","YELLOW","RED","GOLD"): print(f"FAIL: {aid} bad verdict"); return 1
        if v == "GREEN":
            req = ["licenseEvidence","legalReview","commercialUse","directDownload","sha256",
                   "snrDb","clipping","dcOffset","integrity","guitarConfirmed","profile"]
            miss = [k for k in req if k not in a]
            if miss: print(f"FAIL: GREEN {aid} missing {miss}"); return 1
            if a["legalReview"] != "APPROVED": print(f"FAIL: {aid} legalReview"); return 1
            if a["directDownload"] is not True: print(f"FAIL: {aid} directDownload must be true"); return 1
            if a["guitarConfirmed"] is not True: print(f"FAIL: {aid} guitar gate failed"); return 1
            if a["snrDb"] < 40: print(f"FAIL: {aid} SNR < 40 dB"); return 1
            if a["clipping"] or a["dcOffset"] > 0.0001: print(f"FAIL: {aid} quality gate"); return 1
        if v == "RED" and a.get("curatorOverride"):
            print(f"FAIL: RED {aid} cannot be curator-overridden"); return 1
        if v == "GOLD" and not a.get("humanListening"):
            print(f"FAIL: GOLD {aid} requires human listening"); return 1
    print(f"PASS: sound asset registry consistent ({len(seen)} assets, REV 17.0 policy)")
    return 0
if __name__ == "__main__": sys.exit(main(sys.argv[1]))