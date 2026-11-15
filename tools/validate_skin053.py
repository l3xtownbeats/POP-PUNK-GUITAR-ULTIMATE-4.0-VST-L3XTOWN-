#!/usr/bin/env python3
"""Static contract checks for the canonical NEON STAGE SKIN-1 manifest."""
from __future__ import annotations
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SKIN = ROOT / "SKIN" / "Skin_053"
MANIFEST = SKIN / "theme.json"

EXPECTED = {
    "preset_browser", "mode_selector",
    *(f"knob{i:02d}" for i in range(1, 10)),
    *(f"fx_switch{i:02d}" for i in range(1, 8)),
    "midi_keyboard", "LCD_display", "LED_indicator", "SKIN_button",
    "Import_Guitar", "Auto_Discover", "Curator", "Settings", "About",
    "sync_selector",
}

def main() -> None:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    assert data["schemaVersion"] == "1.0.0"
    assert (data["id"], data["index"], data["name"]) == ("skin-053", 53, "NEON STAGE")
    assert data["family"] == "neon_basement"
    assert len(data["palette"]) == 21
    assert all(isinstance(v, str) and len(v) == 7 and v.startswith("#")
               for v in data["palette"].values())
    controls = data["controls"]
    assert set(controls) == EXPECTED, (EXPECTED - set(controls), set(controls) - EXPECTED)
    for control_id, spec in controls.items():
        grid = spec["grid"]
        assert grid["x"] >= 0 and grid["y"] >= 0
        assert grid["w"] > 0 and grid["h"] > 0
        assert grid["x"] + grid["w"] <= 12, control_id
        assert grid["y"] + grid["h"] <= 16, control_id
        assert spec["presentation"] == "native-control"
        assert spec["runtimeBinding"] == "control_schema.json"
    assert data["reduceInEco"] is True
    assert data["hotSwapFadeMs"] == 150
    compatibility = data["compatibility"]
    assert compatibility["apvtsChanges"] is False
    assert compatibility["layoutChanges"] is False
    assert compatibility["audioChanges"] is False
    for relative in ("svg/texture.svg", "svg/bezel.svg", "svg/crest.svg"):
        asset = SKIN / relative
        assert asset.is_file() and "<svg" in asset.read_text(encoding="utf-8")
    print("PASS: NEON STAGE SKIN-1 static manifest and SVG contract")

if __name__ == "__main__":
    main()
