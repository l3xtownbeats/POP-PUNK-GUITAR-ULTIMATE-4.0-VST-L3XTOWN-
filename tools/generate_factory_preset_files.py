#!/usr/bin/env python3
"""Expand the authored 432-entry catalog into portable factory .ppgm/.ppgp files.

Pattern files contain original MIDI-style event recipes derived from the catalog's
groove, accent and mute metadata. They do not claim to be recorded guitar audio.
"""
from __future__ import annotations

import json
import re
import shutil
import sys
from pathlib import Path

SOURCE = Path(sys.argv[1] if len(sys.argv) > 1 else "presets/factory_bank_300.json")
DEST = Path(sys.argv[2] if len(sys.argv) > 2 else "presets/Factory")


def slug(value: str) -> str:
    value = value.lower().replace("&", "and")
    return re.sub(r"[^a-z0-9]+", "-", value).strip("-") or "misc"


def groove_steps(preset: dict) -> list[dict]:
    groove = str(preset.get("groove") or preset.get("playingStyle") or "straight eighth-note drive").lower()
    performance = preset.get("performance") or {}
    mute_map = performance.get("palmMuteMap") or []
    accent_map = performance.get("accentMap") or []
    if "ska" in groove or "offbeat" in groove:
        beats = [0.5, 1.5, 2.5, 3.5]
        direction = "up"
    elif "sixteenth" in groove or "tremolo" in groove:
        beats = [i * 0.25 for i in range(16)]
        direction = "alternate"
    elif "quarter" in groove:
        beats = [0.0, 1.0, 2.0, 3.0]
        direction = "down"
    elif "arpeggio" in groove or "picked" in groove:
        beats = [i * 0.5 for i in range(8)]
        direction = "alternate"
    else:
        beats = [i * 0.5 for i in range(8)]
        direction = "down" if "downstroke" in groove or "down-stroke" in groove else "alternate"

    steps = []
    for index, beat in enumerate(beats):
        if direction == "alternate":
            stroke = "down" if index % 2 == 0 else "up"
        else:
            stroke = direction
        mute = bool(mute_map[index % len(mute_map)]) if mute_map else ("palm" in groove or "muted" in groove)
        accent = float(accent_map[index % len(accent_map)]) if accent_map else (1.0 if index % 2 == 0 else 0.68)
        steps.append({
            "beat": beat,
            "direction": stroke,
            "gate": 0.78 if not mute else 0.52,
            "velocity": max(1, min(127, round(96 * accent))),
            "articulation": "palm-mute" if mute else "open",
            "accent": accent,
        })
    return steps


def main() -> int:
    data = json.loads(SOURCE.read_text(encoding="utf-8"))
    melody = data.get("melodyPresets", [])
    pattern = data.get("patternPresets", [])
    if len(melody) < 216 or len(pattern) < 216:
        raise SystemExit("Factory catalog must contain at least 216 Melody and 216 Pattern definitions.")

    if DEST.exists():
        shutil.rmtree(DEST)
    counts = {"MELODY": 0, "PATTERN": 0}
    index = []
    for mode, entries in (("MELODY", melody), ("PATTERN", pattern)):
        extension = ".ppgm" if mode == "MELODY" else ".ppgp"
        for source in entries:
            artist = source.get("referenceArtist") or "Unsorted"
            folder = DEST / mode.title() / slug(artist)
            folder.mkdir(parents=True, exist_ok=True)
            params = dict(source.get("parameters") or {})
            params.setdefault("voices", 8)
            common = {
                "format": "l3xtown-pop-punk-preset",
                "formatVersion": "1.0.0",
                "id": source["id"],
                "name": source["name"],
                "mode": mode,
                "author": "L3XTOWN",
                "description": source.get("tonalTarget", ""),
                "tags": list(dict.fromkeys([
                    mode.lower(), slug(artist), slug(source.get("soundCharacter", "guitar")),
                    *source.get("tags", []),
                ])),
                "sampleProfile": source.get("sampleProfile", "electric"),
                "parameters": params,
                "reference": {
                    "artist": artist,
                    "tracks": source.get("referenceTracks", []),
                    "styleProfileId": source.get("styleProfileId"),
                    "isStyleReferenceOnly": True,
                },
                "factory": True,
            }
            if mode == "MELODY":
                common["melody"] = {
                    "midiNoteMapping": "chromatic",
                    "velocityLayers": "use-library-mapping",
                    "roundRobin": True,
                    "articulation": str(source.get("soundCharacter", "open")).lower().replace(" ", "-"),
                    "palmMute": params.get("palmMute", "optional"),
                    "releaseMs": 180,
                    "noteBehavior": source.get("noteBehavior", "one MIDI key triggers one chromatic guitar note"),
                }
                common["contentStatus"] = "portable-preset-definition; sample-map-and-runtime-validation-required"
            else:
                common["pattern"] = {
                    "grooveId": slug(source.get("groove") or source.get("playingStyle") or "straight-eighths"),
                    "tempoSync": True,
                    "steps": groove_steps(source),
                    "chordFollow": "midi-chord",
                    "humanizeMs": 4 if (source.get("performance") or {}).get("humanize", True) else 0,
                    "humanizeVelocity": 5 if (source.get("performance") or {}).get("velocityAccents", True) else 0,
                    "sourceNote": "original event recipe; not a transcription or recorded performance",
                }
                common["contentStatus"] = "portable-pattern-definition; runtime-and-guitar-performance-render-validation-required"

            path = folder / (source["id"] + extension)
            path.write_text(json.dumps(common, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            counts[mode] += 1
            index.append({
                "id": source["id"], "name": source["name"], "mode": mode,
                "artist": artist, "file": path.relative_to(DEST).as_posix(),
                "contentStatus": common["contentStatus"],
            })

    manifest = {
        "schemaVersion": "1.0.0",
        "product": "POP PUNK GUITAR ULTIMATE 5.0",
        "sourceCatalog": SOURCE.name,
        "counts": {**counts, "total": sum(counts.values())},
        "note": "Generated portable factory preset definitions. Catalog expansion does not by itself certify runtime integration or recorded PATTERN performance samples.",
        "presets": index,
    }
    (DEST / "factory_manifest.json").write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"Generated factory files: {counts['MELODY']} Melody + {counts['PATTERN']} Pattern = {sum(counts.values())}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
