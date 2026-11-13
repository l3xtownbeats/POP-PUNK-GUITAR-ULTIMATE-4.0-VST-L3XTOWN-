#!/usr/bin/env python3
"""G3.5: usage.json validator. Values must reference the 21 canonical palette
tokens only; hex/rgb literals inside usage are forbidden (fail-closed)."""
import json, re, sys

CANON = ["window_bg","panel","panel_elevated","control_bg","control_face","control_border",
 "text_primary","text_secondary","text_disabled","accent","accent_glow","danger","warning",
 "success","lcd_bg","lcd_text","led_on","led_off","shadow","highlight","lowlight"]

def validate(path):
    errors = []
    with open(path, encoding="utf-8") as f:
        doc = json.load(f)
    if not isinstance(doc, dict) or "usage" not in doc:
        return ["FAIL: missing top-level 'usage'"]
    for key, val in doc["usage"].items():
        if not isinstance(val, str):
            errors.append(f"FAIL: {key}: value must be string")
            continue
        if val not in CANON:
            errors.append(f"FAIL: {key}: '{val}' is not a canonical token")
        if re.search(r"#(?:[0-9a-fA-F]{3}|[0-9a-fA-F]{6})\b", val):
            errors.append(f"FAIL: {key}: hex literal forbidden in usage")
        if re.search(r"rgba?\s*\(", val):
            errors.append(f"FAIL: {key}: rgb()/rgba() forbidden in usage")
    return errors

def main():
    if len(sys.argv) < 2:
        print("usage: validate_skin_usage.py <usage.json>"); return 2
    errs = validate(sys.argv[1])
    if errs:
        print("\n".join(errs)); return 1
    print("PASS: usage.json references canonical tokens only")
    return 0

if __name__ == "__main__":
    sys.exit(main())
