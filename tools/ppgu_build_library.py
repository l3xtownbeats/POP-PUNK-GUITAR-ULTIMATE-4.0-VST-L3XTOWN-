#!/usr/bin/env python3
"""Fetch vetted public guitar SFZ libraries into the PPGU Sound Bank.

Default mode downloads only GitHub-hosted CC0 guitar libraries whose repository
metadata declares CC0-1.0. --github-only is an explicit alias for this mode.
Freesound token support is intentionally metadata/search-only in this first
version: original-file downloads require OAuth2 and are not guessed from an API
key. No Looperman or Splice material is redistributed by this tool.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import shutil
import sys
import urllib.error
import urllib.parse
import urllib.request
import zipfile
from datetime import datetime, timezone
from pathlib import Path

GITHUB_LIBRARIES = [
    {
        "repo": "sfzinstruments/karoryfer.black-and-green-guitars",
        "tag": "v1.000",
        "asset": "Karoryfer_Black_And_Green_Guitars_1000.zip",
        "folder": "03_Distortion_PowerChords/Black_And_Green_Guitars",
        "license": "CC0-1.0",
        "purpose": "Electric hollowbody: plucks, staccato, feedback; Green hammer-ons; Black behind-bridge plucks.",
    },
    {
        "repo": "sfzinstruments/karoryfer.emilyguitar",
        "tag": "v1.001",
        "asset": "Karoryfer.Emilyguitar.v1.001.zip",
        "folder": "02_Clean_Pickup/Emilyguitar",
        "license": "CC0-1.0",
        "purpose": "General-purpose electric guitar; 6 SFZ mappings including unison/barre chord layouts.",
    },
    {
        "repo": "sfzinstruments/karoryfer.shinyguitar",
        "tag": "v1.002",
        "asset": "Karoryfer.Shinyguitar.v1.002.zip",
        "folder": "02_Clean_Pickup/Shinyguitar",
        "license": "CC0-1.0",
        "purpose": "Pickup and microphone captures; bright/aggressive blend potential.",
    },
]

def request_json(url: str, headers: dict[str, str]) -> dict:
    req = urllib.request.Request(url, headers=headers)
    with urllib.request.urlopen(req, timeout=60) as response:
        return json.load(response)

def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()

def safe_extract(zip_path: Path, destination: Path) -> None:
    root = destination.resolve()
    with zipfile.ZipFile(zip_path) as archive:
        for member in archive.infolist():
            target = (destination / member.filename).resolve()
            if target != root and root not in target.parents:
                raise RuntimeError(f"Unsafe archive path: {member.filename}")
        archive.extractall(destination)

def fetch_github_bank(root: Path, token: str | None) -> list[dict]:
    headers = {"User-Agent": "PPGU-SoundBankBuilder/1.0", "Accept": "application/vnd.github+json"}
    if token:
        headers["Authorization"] = f"Bearer {token}"
    report = []
    for item in GITHUB_LIBRARIES:
        repo = item["repo"]
        print(f"[GitHub] Проверка лицензии: {repo}")
        metadata = request_json(f"https://api.github.com/repos/{repo}", headers)
        license_spdx = ((metadata.get("license") or {}).get("spdx_id") or "").upper()
        if license_spdx != item["license"]:
            raise RuntimeError(f"{repo}: expected {item['license']}, GitHub reports {license_spdx or 'unknown'}; refusing download")
        license_payload = request_json(f"https://api.github.com/repos/{repo}/license", headers)
        license_bytes = base64.b64decode(license_payload.get("content", ""))
        if b"CC0 1.0 Universal" not in license_bytes and b"Creative Commons Zero" not in license_bytes:
            raise RuntimeError(f"{repo}: official GitHub license file does not contain CC0 1.0; refusing download")
        release = request_json(f"https://api.github.com/repos/{repo}/releases/tags/{item['tag']}", headers)
        asset = next((a for a in release.get("assets", []) if a.get("name") == item["asset"]), None)
        if not asset:
            raise RuntimeError(f"{repo}@{item['tag']}: expected release asset {item['asset']} not found")
        target = root / item["folder"]
        marker = target / "SOURCE_PROVENANCE.json"
        if marker.exists():
            try:
                old = json.loads(marker.read_text(encoding="utf-8"))
                if old.get("asset_url") == asset["browser_download_url"] and old.get("license") == item["license"]:
                    report.append(old | {"status": "already-installed"})
                    print(f"  Уже установлена: {target}")
                    continue
            except Exception:
                pass
        target.parent.mkdir(parents=True, exist_ok=True)
        temp = target.with_name(target.name + ".download")
        if temp.exists():
            shutil.rmtree(temp)
        temp.mkdir(parents=True)
        archive_path = temp / item["asset"]
        print(f"  Загрузка {asset.get('size', 0) / (1024*1024):.1f} MiB ...")
        req = urllib.request.Request(asset["browser_download_url"], headers={"User-Agent": headers["User-Agent"]})
        with urllib.request.urlopen(req, timeout=180) as response, archive_path.open("wb") as out:
            shutil.copyfileobj(response, out)
        archive_hash = sha256(archive_path)
        extracted = temp / "extracted"
        extracted.mkdir()
        safe_extract(archive_path, extracted)
        sfz_files = list(extracted.rglob("*.sfz"))
        audio_files = [p for p in extracted.rglob("*") if p.is_file() and p.suffix.lower() in {".wav", ".flac", ".aif", ".aiff", ".ogg"}]
        if not sfz_files or not audio_files:
            raise RuntimeError(f"{repo}: archive must contain SFZ mapping and audio samples")
        license_files = [p for p in extracted.rglob("*") if p.is_file() and p.name.lower() in {"license", "license.txt", "copying", "copying.txt"}]
        license_text = "\n".join(p.read_text(encoding="utf-8", errors="ignore") for p in license_files).lower()
        if "cc0" not in license_text and "creative commons zero" not in license_text:
            raise RuntimeError(f"{repo}: release archive lacks an explicit CC0 license file; not installing")
        if target.exists():
            shutil.rmtree(target)
        shutil.copytree(extracted, target)
        provenance = {
            "source": repo, "tag": item["tag"], "asset": item["asset"],
            "asset_url": asset["browser_download_url"], "license": item["license"],
            "license_verified_from_github_spdx_and_archive": True,
            "archive_sha256": archive_hash, "audio_files": len(audio_files),
            "sfz_files": [str(p.relative_to(target)).replace("\\", "/") for p in target.rglob("*.sfz")],
            "purpose": item["purpose"], "downloaded_utc": datetime.now(timezone.utc).isoformat(),
            "status": "downloaded-license-gated",
        }
        (target / "SOURCE_PROVENANCE.json").write_text(json.dumps(provenance, indent=2), encoding="utf-8")
        shutil.rmtree(temp, ignore_errors=True)
        report.append(provenance)
        print(f"  Готово: {len(audio_files)} audio, {len(sfz_files)} SFZ -> {target}")
    return report

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", default="Samples/SoundBank", help="Sound Bank destination")
    parser.add_argument("--token", default=os.getenv("GITHUB_TOKEN"), help="Optional GitHub API token (or GITHUB_TOKEN env var)")
    parser.add_argument("--github-only", action="store_true", help="Download only vetted CC0 GitHub SFZ libraries")
    parser.add_argument("--freesound-token", default=os.getenv("FREESOUND_API_KEY"), help="Optional Freesound API key for future metadata search")
    args = parser.parse_args()
    root = Path(args.root).resolve()
    root.mkdir(parents=True, exist_ok=True)
    try:
        report = fetch_github_bank(root, args.token)
        summary = {
            "schemaVersion": 1, "generated_utc": datetime.now(timezone.utc).isoformat(),
            "mode": "github-only", "libraries": report,
            "freesound": {
                "configured": bool(args.freesound_token),
                "status": "API key detected; original sound download is not attempted by this script. Freesound original downloads require OAuth2.",
            },
            "excluded_sources": {
                "Looperman": "Not bundled: terms prohibit redistributing loops as sample packs or standalone files.",
                "Splice Mike Naran Pop Punk Riffs": "Not bundled: license prohibits sublicensing/redistribution as isolated samples or competing sample packs.",
                "Freesound Ax_Grinder pack 14939": "Not bundled automatically until each sound's individual license and original-download authorization are verified.",
                "Selekt Audio guitar MIDI": "Review per-file license certificate and redistribution rights before inclusion in a distributed preset bank.",
            },
        }
        (root / "SOURCE_LIBRARY_REPORT.json").write_text(json.dumps(summary, indent=2), encoding="utf-8")
        print(f"Sound Bank source report: {root / 'SOURCE_LIBRARY_REPORT.json'}")
        return 0
    except (urllib.error.URLError, urllib.error.HTTPError, OSError, RuntimeError, zipfile.BadZipFile) as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 2

if __name__ == "__main__":
    raise SystemExit(main())
