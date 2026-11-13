#!/usr/bin/env python3
"""G3.5: validate usage.json against the 21 canonical semantic palette tokens."""
from __future__ import annotations

import json
import re
import sys
from pathlib import Path

CANONICAL_TOKENS = {
    "window_bg", "panel", "panel_elevated", "control_bg", "control_face",
    "control_border", "text_primary", "text_secondary", "text_disabled",
    "accent", "accent_glow", "danger", "warning", "success", "lcd_bg",
    "lcd_text", "led_on", "led_off", "shadow", "highlight", "lowlight",
}
HEX_LITERAL = re.compile(r"#(?:[0-9a-fA-F]{3}|[0-9a-fA-F]{6}|[0-9a-fA-F]{8})\b")
RGB_LITERAL = re.compile(r"\brgba?\s*\(", re.IGNORECASE)


def validate(path: Path) -> list[str]:
    try:
        document = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        return [f"FAIL: cannot read usage JSON: {exc}"]

    if not isinstance(document, dict) or not isinstance(document.get("usage"), dict):
        return ["FAIL: top-level 'usage' must be an object mapping controls to tokens"]

    usage = document["usage"]
    if not usage:
        return ["FAIL: usage object is empty"]

    errors: list[str] = []
    for key, value in usage.items():
        if not isinstance(key, str) or not key.strip():
            errors.append("FAIL: usage contains an empty/non-string key")
            continue
        if not isinstance(value, str):
            errors.append(f"FAIL: {key}: value must be a token string")
            continue
        if HEX_LITERAL.search(value):
            errors.append(f"FAIL: {key}: hex literal forbidden in usage")
        if RGB_LITERAL.search(value):
            errors.append(f"FAIL: {key}: rgb()/rgba() forbidden in usage")
        if value not in CANONICAL_TOKENS:
            errors.append(f"FAIL: {key}: '{value}' is not a canonical token")

    return errors


def main(argv: list[str]) -> int:
    if len(argv) != 2:
        print("usage: validate_skin_usage.py <usage.json>", file=sys.stderr)
        return 2
    errors = validate(Path(argv[1]))
    if errors:
        print("\n".join(errors), file=sys.stderr)
        return 1
    print("PASS: usage.json references canonical tokens only")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
