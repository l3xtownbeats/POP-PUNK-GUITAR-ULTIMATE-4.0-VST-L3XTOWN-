#!/usr/bin/env python3
"""Report REV.18 factory preset readiness without fabricating completion."""
from __future__ import annotations
import argparse, json
from pathlib import Path

READY_STATUSES = {"ready", "release-ready", "production-ready"}

def count_ready(items):
    ready = 0
    statuses = {}
    for item in items:
        status = str(item.get("status", "missing-status")).strip().lower()
        statuses[status] = statuses.get(status, 0) + 1
        mapped = bool(item.get("sampleMapping") or item.get("sampleProfile"))
        licensed = bool(item.get("licenseEvidence") or item.get("license"))
        if status in READY_STATUSES and item.get("audioValidated") is True and mapped and licensed:
            ready += 1
    return ready, statuses

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--catalog", type=Path, default=Path("presets/factory_bank_300.json"))
    parser.add_argument("--report", type=Path)
    parser.add_argument("--release-gate", action="store_true")
    args = parser.parse_args()
    data = json.loads(args.catalog.read_text(encoding="utf-8"))
    melody = data.get("melodyPresets", [])
    pattern = data.get("patternPresets", [])
    m_ready, m_status = count_ready(melody)
    p_ready, p_status = count_ready(pattern)
    blockers = []
    if m_ready < 500: blockers.append(f"MELODY READY count is {m_ready}; REV.18 requires 500")
    if p_ready < 500: blockers.append(f"PATTERN READY count is {p_ready}; REV.18 requires 500")
    lines = [
        "POP PUNK GUITAR ULTIMATE — REV.18 FACTORY PRESET READINESS",
        f"Catalog: {args.catalog}",
        f"MELODY definitions: {len(melody)}; verified READY: {m_ready}/500",
        f"PATTERN definitions: {len(pattern)}; verified READY: {p_ready}/500",
        f"Total verified READY: {m_ready+p_ready}/1000",
        f"MELODY statuses: {json.dumps(m_status, ensure_ascii=False, sort_keys=True)}",
        f"PATTERN statuses: {json.dumps(p_status, ensure_ascii=False, sort_keys=True)}",
        "READY requires an approved status, audioValidated=true, sample mapping/profile and license evidence.",
        "Catalogue-only records do not count as playable factory presets.",
        "Result: " + ("PASS" if not blockers else "NOT RELEASE READY"),
    ]
    if blockers: lines += ["", "Blockers:"] + [f"- {b}" for b in blockers]
    report = "\n".join(lines) + "\n"
    print(report, end="")
    if args.report:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(report, encoding="utf-8")
    return 1 if args.release_gate and blockers else 0

if __name__ == "__main__":
    raise SystemExit(main())
