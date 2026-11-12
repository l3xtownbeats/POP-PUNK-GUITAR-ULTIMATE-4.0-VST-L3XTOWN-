#!/usr/bin/env python3
"""Validate PPGU artist/song tone-profile metadata links.

This validates catalogue integrity only. It does NOT prove that any preset is
playable, that its DSP route exists, or that its sound is mix-ready.
"""
from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BANK_PATH = ROOT / "presets" / "factory_bank_300.json"
REGISTRY_PATH = ROOT / "presets" / "artist_tone_profiles_v1.json"


def fail(message: str) -> None:
    raise SystemExit(f"FAIL: {message}")


def main() -> int:
    try:
        bank = json.loads(BANK_PATH.read_text(encoding="utf-8"))
        registry = json.loads(REGISTRY_PATH.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        fail(f"cannot read/parse preset metadata: {exc}")

    profiles = registry.get("profiles", [])
    songs = registry.get("songReferences", [])
    profile_ids = [item.get("id") for item in profiles]
    if len(profiles) != 21 or len(set(profile_ids)) != len(profile_ids):
        fail(f"expected 21 unique style profiles; found {len(profiles)}")
    if len(songs) != 50 or len({item.get("id") for item in songs}) != 50:
        fail(f"expected 50 unique song references; found {len(songs)}")

    for profile in profiles:
        controls = profile.get("ampControls", {})
        for key in ("gain", "bass", "mid", "treble", "presence"):
            value = controls.get(key)
            if not isinstance(value, (int, float)) or not 0.0 <= value <= 10.0:
                fail(f"profile {profile.get('id')} has invalid {key}: {value!r}")
        for key in ("sourceProfileHint", "ampFamilyTarget",
                    "cabinetVoiceTarget", "micPerspectiveTarget",
                    "fxIntent", "tuningHint", "playingAndMixNotes"):
            if not profile.get(key):
                fail(f"profile {profile.get('id')} missing {key}")

    all_presets = bank.get("melodyPresets", []) + bank.get("patternPresets", [])
    if len(bank.get("melodyPresets", [])) != 150 or len(bank.get("patternPresets", [])) != 150:
        fail("factory bank must contain 150 Melody and 150 Pattern definitions")
    if len(all_presets) != 300:
        fail(f"expected 300 factory definitions; found {len(all_presets)}")

    missing = []
    for preset in all_presets:
        style_id = preset.get("styleProfileId")
        tone_id = preset.get("toneProfileId")
        if not style_id or style_id not in profile_ids:
            missing.append(f"{preset.get('id')}: unknown styleProfileId={style_id!r}")
        if tone_id != style_id:
            missing.append(f"{preset.get('id')}: toneProfileId={tone_id!r} != {style_id!r}")
    if missing:
        fail("profile links invalid: " + "; ".join(missing[:12]))

    ref = bank.get("toneProfileRegistry", {})
    if ref.get("path") != "presets/artist_tone_profiles_v1.json" or ref.get("version") != registry.get("schemaVersion"):
        fail("factory bank registry pointer/version mismatch")

    print("PASS: 21 unique style profiles; 50 unique song references; 300/300 factory entries linked.")
    print("LIMIT: metadata integrity only; runtime routing, DSP availability, audio quality and host playback remain unverified.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
