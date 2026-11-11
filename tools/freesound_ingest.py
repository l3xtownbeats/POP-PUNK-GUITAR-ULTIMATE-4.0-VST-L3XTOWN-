#!/usr/bin/env python3
"""Discover and optionally ingest Freesound CC0 guitar recordings for PPGU.

Credentials are read only from environment variables:
  FREESOUND_API_KEY       APIv2 key (required)
  FREESOUND_ACCESS_TOKEN  OAuth2 bearer token (required only for --download)

Search-only mode writes a reviewable JSON manifest. Download mode fetches original
files only when Freedsound reports the exact "Creative Commons 0" license.
No non-CC0 item is ever downloaded or marked distributable.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import sys
import time
from datetime import datetime, timezone
from pathlib import Path
from urllib.parse import urlencode
from urllib.request import Request, urlopen
from urllib.error import HTTPError, URLError

API = "https://freesound.org/apiv2"
FIELDS = ",".join([
    "id", "name", "url", "username", "license", "tags", "description",
    "type", "channels", "duration", "samplerate", "bitdepth", "filesize",
    "md5", "num_downloads", "avg_rating", "previews", "download"
])
QUERIES = [
    "electric guitar isolated note",
    "electric guitar palm mute",
    "electric guitar clean note",
    "electric guitar distorted note",
    "electric guitar power chord",
    "guitar harmonic note",
    "acoustic guitar isolated note",
    "acoustic guitar strum",
    "guitar slide note",
]

def api_get(url: str, headers: dict[str, str]) -> dict:
    req = Request(url, headers=headers)
    with urlopen(req, timeout=45) as response:
        return json.loads(response.read().decode("utf-8"))

def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()

def safe_name(value: str) -> str:
    value = re.sub(r"[^A-Za-z0-9._-]+", "_", value).strip("._")
    return value[:90] or "sound"

def search(api_key: str, max_per_query: int) -> list[dict]:
    headers = {"User-Agent": "PPGU-SoundBank/1.0", "Authorization": f"Token {api_key}"}
    found: dict[int, dict] = {}
    for query in QUERIES:
        params = urlencode({"query": query, "page_size": min(max_per_query, 150), "fields": FIELDS})
        url = f"{API}/search/?{params}"
        try:
            page = api_get(url, headers)
        except (HTTPError, URLError, TimeoutError) as exc:
            print(f"Search failed for {query!r}: {exc}", file=sys.stderr)
            continue
        for item in page.get("results", []):
            # Strict allowlist: only CC0 recordings are candidates for factory redistribution.
            if item.get("license") != "Creative Commons 0":
                continue
            item["ppgu_search_query"] = query
            found[int(item["id"])] = item
        time.sleep(1.05)  # remain comfortably below standard API request limits
    return list(found.values())

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", default="Samples/Freesound-CC0/manifest.json")
    parser.add_argument("--max-per-query", type=int, default=40)
    parser.add_argument("--download", action="store_true",
                        help="download original files (requires OAuth2 access token)")
    parser.add_argument("--min-duration", type=float, default=0.08)
    parser.add_argument("--max-duration", type=float, default=20.0)
    args = parser.parse_args()

    api_key = os.environ.get("FREESOUND_API_KEY")
    if not api_key:
        parser.error("Set FREESOUND_API_KEY in the environment; do not put it in source code.")
    access_token = os.environ.get("FREESOUND_ACCESS_TOKEN")
    if args.download and not access_token:
        parser.error("--download requires FREESOUND_ACCESS_TOKEN (OAuth2 bearer token).")

    items = search(api_key, args.max_per_query)
    items = [x for x in items
             if args.min_duration <= float(x.get("duration") or 0) <= args.max_duration
             and str(x.get("type", "")).lower() in {"wav", "aif", "aiff", "flac"}]
    items.sort(key=lambda x: (-(float(x.get("avg_rating") or 0)),
                              -int(x.get("num_downloads") or 0), int(x["id"])))

    output = Path(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)
    root = output.parent
    records = []
    for item in items:
        record = {
            "source": "Freesound APIv2",
            "soundId": item["id"],
            "title": item.get("name"),
            "creator": item.get("username"),
            "sourceUrl": item.get("url"),
            "license": item.get("license"),
            "licenseStatus": "CC0_CANDIDATE_REVIEW_REQUIRED",
            "tags": item.get("tags", []),
            "description": item.get("description", ""),
            "format": item.get("type"),
            "channels": item.get("channels"),
            "sampleRate": item.get("samplerate"),
            "bitDepth": item.get("bitdepth"),
            "durationSeconds": item.get("duration"),
            "sourceMd5": item.get("md5"),
            "retrievedAtUtc": datetime.now(timezone.utc).isoformat(),
            "downloaded": False,
        }
        if args.download:
            # Original download is an OAuth2-only API resource.
            headers = {"User-Agent": "PPGU-SoundBank/1.0",
                       "Authorization": f"Bearer {access_token}"}
            target_dir = root / "audio"
            target_dir.mkdir(parents=True, exist_ok=True)
            target = target_dir / f"{item['id']}_{safe_name(item.get('name', 'guitar'))}.{item['type'].lower()}"
            if not target.exists():
                req = Request(f"{API}/sounds/{item['id']}/download/", headers=headers)
                try:
                    with urlopen(req, timeout=180) as response, target.open("wb") as dest:
                        while True:
                            chunk = response.read(1024 * 1024)
                            if not chunk:
                                break
                            dest.write(chunk)
                except Exception:
                    target.unlink(missing_ok=True)
                    raise
            record["localPath"] = str(target).replace("\\", "/")
            record["sha256"] = sha256(target)
            record["downloaded"] = True
            record["licenseStatus"] = "CC0_DOWNLOADED_PENDING_AUDIO_AND_PROVENANCE_QA"
        records.append(record)

    payload = {
        "schemaVersion": 1,
        "source": "Freesound APIv2",
        "generatedAtUtc": datetime.now(timezone.utc).isoformat(),
        "policy": "Only exact Creative Commons 0 results are included; every item still requires provenance and audio QA before factory release.",
        "candidateCount": len(records),
        "downloadedCount": sum(1 for x in records if x["downloaded"]),
        "sounds": records,
    }
    output.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"Saved {len(records)} CC0 guitar candidates to {output}")
    if not args.download:
        print("Search-only: no audio was downloaded. Re-run with --download and OAuth2 token after review.")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
