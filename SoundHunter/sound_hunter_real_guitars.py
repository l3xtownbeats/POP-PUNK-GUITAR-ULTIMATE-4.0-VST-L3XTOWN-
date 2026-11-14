#!/usr/bin/env python3
"""Additive Sound Hunter for explicitly verified real-recorded guitar sources.

Nothing is written into an existing source directory. Each run gets a unique
Samples/SoundHunterImports/<run-id>/<source-id> destination. Existing content
is never deleted, replaced, or normalized.
"""
from __future__ import annotations
import argparse, hashlib, json, os, re, shutil, sys, urllib.request, urllib.error, zipfile
from datetime import datetime, timezone
from pathlib import Path, PurePosixPath

ROOT = Path(__file__).resolve().parents[1]
CATALOG = Path(__file__).with_name("real_guitar_sources.json")
AUDIO = {".wav",".wave",".flac",".ogg",".oga",".mp3",".aif",".aiff",".caf",".opus",".m4a",".aac",".wma",".sf2",".sf3",".sfz",".dspreset"}
UA = "PPGU-SoundHunter/1.0 (+real-recorded-guitar-only)"

def request(url: str, timeout: int = 90) -> bytes:
    req = urllib.request.Request(url, headers={"User-Agent": UA, "Accept": "application/vnd.github+json"})
    with urllib.request.urlopen(req, timeout=timeout) as response:
        return response.read()

def json_url(url: str):
    return json.loads(request(url).decode("utf-8"))

def sha256(path: Path) -> str:
    h=hashlib.sha256()
    with path.open("rb") as f:
        for block in iter(lambda:f.read(1024*1024), b""): h.update(block)
    return h.hexdigest()

def license_ok(text: str, expected: str) -> bool:
    t=text.lower()
    if expected == "CC0-1.0":
        return ("cc0" in t or "creative commons zero" in t or "public domain dedication" in t) and ("noncommercial" not in t and "non-commercial" not in t)
    if expected == "MIT":
        return "permission is hereby granted" in t and "the software" in t
    return False

def fetch_lfs_object(repo: str, pointer: bytes) -> bytes:
    oid_match=re.search(rb"oid sha256:([0-9a-f]{64})",pointer)
    size_match=re.search(rb"size ([0-9]+)",pointer)
    if not oid_match or not size_match: raise RuntimeError("Malformed Git LFS pointer")
    oid=oid_match.group(1).decode("ascii"); size=int(size_match.group(1))
    payload=json.dumps({"operation":"download","transfers":["basic"],"objects":[{"oid":oid,"size":size}]}).encode()
    req=urllib.request.Request(f"https://github.com/{repo}.git/info/lfs/objects/batch",data=payload,headers={"User-Agent":UA,"Accept":"application/vnd.git-lfs+json","Content-Type":"application/vnd.git-lfs+json"})
    with urllib.request.urlopen(req,timeout=90) as response: batch=json.loads(response.read().decode())
    obj=batch.get("objects",[{}])[0]
    if obj.get("error"): raise RuntimeError(f"Git LFS object unavailable: {obj['error']}")
    action=obj.get("actions",{}).get("download")
    if not action or not action.get("href"): raise RuntimeError("Git LFS download action missing")
    req=urllib.request.Request(action["href"],headers={"User-Agent":UA,**action.get("header",{})})
    with urllib.request.urlopen(req,timeout=300) as response: data=response.read()
    if len(data)!=size or hashlib.sha256(data).hexdigest()!=oid: raise RuntimeError("Git LFS object failed size/SHA-256 verification")
    return data

def safe_member(name: str) -> bool:
    p=PurePosixPath(name)
    return not p.is_absolute() and ".." not in p.parts and not re.match(r"^[A-Za-z]:", name)

def fetch_repo_archive(repo: str, dest: Path):
    meta=json_url(f"https://api.github.com/repos/{repo}")
    branch=meta.get("default_branch") or "main"
    data=request(f"https://api.github.com/repos/{repo}/zipball/{branch}", timeout=180)
    dest.write_bytes(data)
    return meta

def fetch_release_zip(repo: str, tag: str, match: str, dest: Path):
    rel=json_url(f"https://api.github.com/repos/{repo}/releases/tags/{tag}")
    assets=[a for a in rel.get("assets",[]) if match.lower() in a.get("name","").lower()]
    if len(assets)!=1:
        raise RuntimeError(f"Expected exactly one release asset matching {match!r}; found {len(assets)}")
    data=request(assets[0]["browser_download_url"], timeout=300)
    dest.write_bytes(data)
    return assets[0]["name"]

def existing_hashes(samples: Path):
    found=set()
    if samples.exists():
        for p in samples.rglob("*"):
            if p.is_file() and p.suffix.lower() in AUDIO:
                try: found.add(sha256(p))
                except OSError: pass
    return found

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--samples", type=Path, default=ROOT/"Samples")
    ap.add_argument("--discover-only", action="store_true", help="list approved source links without downloading")
    ap.add_argument("--max-gb", type=float, default=25.0)
    args=ap.parse_args()
    if args.max_gb <= 0 or args.max_gb > 25.0:
        raise SystemExit("The approved Samples capacity is capped at 25 GB.")
    catalog=json.loads(CATALOG.read_text(encoding="utf-8"))
    sources=catalog["sources"]
    print("SOUND HUNTER — REAL RECORDED GUITARS ONLY")
    for s in sources:
        print(f'{s["id"]}: https://github.com/{s["repository"]} [{s["license"]}]')
    if args.discover_only:
        return 0

    samples=args.samples.resolve()
    samples.mkdir(parents=True, exist_ok=True)
    base=samples/"SoundHunterImports"
    base.mkdir(parents=True, exist_ok=True)
    run_id=datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")+"-"+os.urandom(3).hex()
    run_root=base/run_id
    run_root.mkdir()  # unique; fail rather than reuse
    used=sum(p.stat().st_size for p in samples.rglob("*") if p.is_file())
    limit=int(args.max_gb*1024**3)
    seen=existing_hashes(samples)
    registry={"schema_version":"1.0","run_id":run_id,"classification":"REAL_RECORDED","assets":[],"sources":[]}
    tmp=run_root/"_download_tmp"; tmp.mkdir()
    try:
        for s in sources:
            sid=s["id"]; repo=s["repository"]
            print(f"Checking {sid} ...")
            archive=tmp/(sid+".zip")
            meta=fetch_repo_archive(repo, archive)
            # Read the exact repository license file from the downloaded archive.
            with zipfile.ZipFile(archive) as z:
                names=[n for n in z.namelist() if safe_member(n)]
                license_paths=[n for n in names if PurePosixPath(n).name.lower()==s["license_file"].lower()]
                if not license_paths:
                    raise RuntimeError(f"{sid}: required license file {s['license_file']} missing")
                license_text=z.read(license_paths[0]).decode("utf-8","replace")
                if not license_ok(license_text,s["license"]):
                    raise RuntimeError(f"{sid}: license text does not verify expected {s['license']}; skipped")
                source_dir=run_root/sid
                source_dir.mkdir()
                (source_dir/"LICENSE_SOURCE.txt").write_text(license_text,encoding="utf-8")
                (source_dir/"SOURCE.json").write_text(json.dumps({
                    "name":s["name"],"repository":f"https://github.com/{repo}",
                    "kind":"REAL_RECORDED","license":s["license"],
                    "notes":s.get("notes",""),"default_branch":meta.get("default_branch")
                },ensure_ascii=False,indent=2),encoding="utf-8")
                copied=0
                for name in names:
                    rel=PurePosixPath(name)
                    if rel.suffix.lower() not in AUDIO: continue
                    # Skip Git LFS pointer stubs and suspiciously tiny audio placeholders.
                    info=z.getinfo(name)
                    if info.file_size < 128: continue
                    pointer=z.read(name) if info.file_size <= 1024 else b""
                    if pointer.startswith(b"version https://git-lfs.github.com/spec/v1"):
                        data=fetch_lfs_object(repo,pointer); source_size=len(data)
                    else:
                        data=None; source_size=info.file_size
                    if used+source_size > limit:
                        print(f"CAP REACHED at {sid}; remaining source files not imported.")
                        break
                    if data is None: data=z.read(name)
                    digest=hashlib.sha256(data).hexdigest()
                    if digest in seen: continue
                    target=source_dir.joinpath(*rel.parts[1:])
                    target.parent.mkdir(parents=True,exist_ok=True)
                    if target.exists():
                        raise RuntimeError(f"Refusing path collision: {target}")
                    with target.open("xb") as dst: dst.write(data)
                    if target.stat().st_size != source_size:
                        raise RuntimeError(f"Size verification failed: {target}")
                    actual=sha256(target)
                    if actual != digest: raise RuntimeError(f"SHA-256 verification failed: {target}")
                    seen.add(digest); used+=source_size; copied+=1
                    registry["assets"].append({
                        "source_id":sid,"path":str(target.relative_to(samples)).replace("\\","/"),
                        "format":target.suffix.lower(),"bytes":target.stat().st_size,
                        "sha256":actual,"classification":"REAL_RECORDED",
                        "license":s["license"],"verification":"license-and-sha256-verified"
                    })
                if s.get("release_tag") and s.get("release_asset_contains"):
                    relzip=tmp/(sid+"-release.zip")
                    asset_name=fetch_release_zip(repo,s["release_tag"],s["release_asset_contains"],relzip)
                    with zipfile.ZipFile(relzip) as rz:
                        for name in rz.namelist():
                            if not safe_member(name) or PurePosixPath(name).suffix.lower() not in AUDIO: continue
                            info=rz.getinfo(name)
                            if info.file_size < 128 or used+info.file_size > limit: continue
                            data=rz.read(name); digest=hashlib.sha256(data).hexdigest()
                            if digest in seen: continue
                            target=source_dir/"release"/Path(*PurePosixPath(name).parts[1:])
                            target.parent.mkdir(parents=True,exist_ok=True)
                            if target.exists(): raise RuntimeError(f"Refusing path collision: {target}")
                            with target.open("xb") as f: f.write(data)
                            if sha256(target)!=digest: raise RuntimeError(f"SHA-256 verification failed: {target}")
                            seen.add(digest); used+=len(data)
                            registry["assets"].append({"source_id":sid,"path":str(target.relative_to(samples)).replace("\\","/"),"format":target.suffix.lower(),"bytes":len(data),"sha256":digest,"classification":"REAL_RECORDED","license":s["license"],"verification":"release-license-and-sha256-verified","release_asset":asset_name})
                registry["sources"].append({"id":sid,"repository":f"https://github.com/{repo}","license":s["license"],"kind":"REAL_RECORDED","files_added":copied})
                print(f"  imported {copied} repository audio files")
        out=run_root/"SOUND_HUNTER_REGISTRY.json"
        out.write_text(json.dumps(registry,ensure_ascii=False,indent=2),encoding="utf-8")
        print(f"Completed. {len(registry['assets'])} audio assets; library usage {used/1024**3:.2f}/{args.max_gb:.2f} GB.")
    finally:
        # Only remove this run's temporary download directory, never existing user content.
        if tmp.exists(): shutil.rmtree(tmp)
    return 0

if __name__=="__main__":
    try: raise SystemExit(main())
    except Exception as e:
        print(f"SOUND HUNTER ERROR: {e}",file=sys.stderr)
        raise SystemExit(2)
