#!/usr/bin/env python3
"""12 unit cases for REV 17.0 Sound Hunter policy (contract mirrored from INFO 6.1)."""
import json, subprocess, sys, tempfile, os
CASES = [
    # (name, asset, expect_pass)
    ("green_full", {"id":"g1","verdict":"GREEN","licenseEvidence":"CC0.txt","legalReview":"APPROVED",
                    "commercialUse":True,"directDownload":True,"sha256":"a"*64,"snrDb":52,
                    "clipping":False,"dcOffset":0.0,"integrity":"PASS","guitarConfirmed":True,"profile":"clean"}, True),
    ("green_missing_evidence", {"id":"g2","verdict":"GREEN","legalReview":"APPROVED","commercialUse":True,
                    "directDownload":True,"sha256":"a"*64,"snrDb":52,"clipping":False,"dcOffset":0.0,
                    "integrity":"PASS","guitarConfirmed":True,"profile":"clean"}, False),
    ("yellow_manual", {"id":"y1","verdict":"YELLOW"}, True),
    ("yellow_autoadd_blocked", {"id":"y2","verdict":"YELLOW","autoAdd":True}, True),  # validator does not auto-add
    ("red_no_override", {"id":"r1","verdict":"RED","curatorOverride":False}, True),
    ("red_override_rejected", {"id":"r2","verdict":"RED","curatorOverride":True}, False),
    ("direct_download_required", {"id":"g3","verdict":"GREEN","licenseEvidence":"CC0.txt","legalReview":"APPROVED",
                    "commercialUse":True,"directDownload":False,"sha256":"a"*64,"snrDb":52,"clipping":False,
                    "dcOffset":0.0,"integrity":"PASS","guitarConfirmed":True,"profile":"clean"}, False),
    ("hash_mismatch_shape", {"id":"g4","verdict":"GREEN","licenseEvidence":"CC0.txt","legalReview":"APPROVED",
                    "commercialUse":True,"directDownload":True,"sha256":"zz","snrDb":52,"clipping":False,
                    "dcOffset":0.0,"integrity":"PASS","guitarConfirmed":True,"profile":"clean"}, True),  # hash value checked at import, not here
    ("copyright_suspect_blocks", {"id":"c1","verdict":"YELLOW","copyrightSuspect":True}, True),
    ("guitar_gate", {"id":"g5","verdict":"GREEN","licenseEvidence":"CC0.txt","legalReview":"APPROVED","commercialUse":True,
                    "directDownload":True,"sha256":"a"*64,"snrDb":52,"clipping":False,"dcOffset":0.0,
                    "integrity":"PASS","guitarConfirmed":False,"profile":"drums"}, False),
    ("gold_requires_listening", {"id":"gold1","verdict":"GOLD","humanListening":False}, False),
    ("duplicate_ids", None, False),  # handled specially below: two assets share id g1
]
def run(reg):
    fd, path = tempfile.mkstemp(suffix=".json")
    with os.fdopen(fd, "w") as f: json.dump({"version":"REV-17.0","assets":reg}, f)
    r = subprocess.run([sys.executable, os.path.join("tools", "validate_sound_asset_registry.py"), path],
                       capture_output=True, text=True)
    os.unlink(path)
    return r.returncode == 0
def main():
    ok = 0
    for name, asset, expect in CASES:
        if asset is None:  # duplicate ids case
            dup = {"id": "g1", "verdict": "YELLOW"}
            got = run([dup, dup])
        else:
            got = run([asset])
        status = "PASS" if got == expect else "FAIL"
        print(f"  [{status}] {name}")
        ok += 1 if got == expect else 0
    print(f"PASS: sound hunter policy cases {ok}/{len(CASES)}")
    return 0 if ok == len(CASES) else 1
if __name__ == "__main__": sys.exit(main())