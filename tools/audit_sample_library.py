#!/usr/bin/env python3
"""Read-only inventory of canonical guitar audio and SFZ sample references."""
import argparse, hashlib, json, re, sys
from collections import Counter
from pathlib import Path
AUDIO={".wav",".flac",".ogg",".aiff",".aif",".mp3"}
RX=re.compile(r'\bsample\s*=\s*["\']?([^"\'\s]+)',re.I)
def digest(p):
 h=hashlib.sha256()
 with p.open("rb") as f:
  for b in iter(lambda:f.read(1024*1024),b""): h.update(b)
 return h.hexdigest()
def main():
 ap=argparse.ArgumentParser()
 ap.add_argument("--root",type=Path,required=True); ap.add_argument("--output",type=Path,required=True)
 a=ap.parse_args(); root=a.root.resolve()
 if not root.is_dir(): raise SystemExit("Sample Library root missing")
 audio=sorted(p for p in root.rglob("*") if p.is_file() and p.suffix.lower() in AUDIO)
 maps=sorted(root.rglob("*.sfz"))
 licenses=sorted(p for p in root.rglob("*") if p.is_file() and p.name.lower() in {"license","license.txt","copying","copying.txt"})
 if len(audio)<100: raise SystemExit(f"Expected >=100 audio files, found {len(audio)}")
 missing=[]
 for sfz in maps:
  for raw in RX.findall(sfz.read_text(encoding="utf-8",errors="replace")):
   target=(sfz.parent/raw.replace("\\\\","/")).resolve()
   try: target.relative_to(root)
   except ValueError:
    missing.append({"sfz":sfz.relative_to(root).as_posix(),"sample":raw,"error":"path_escapes_root"}); continue
   if not target.is_file(): missing.append({"sfz":sfz.relative_to(root).as_posix(),"sample":raw,"error":"missing_file"})
 dirs=sorted({p.parent for p in audio})
 report={"schemaVersion":"1.0.0","audit":"READ_ONLY_CONTENT_INVENTORY","root":"Sample Library/",
  "audioFileCount":len(audio),"audioBytes":sum(p.stat().st_size for p in audio),
  "audioExtensions":dict(Counter(p.suffix.lower() for p in audio)),"sfzFileCount":len(maps),
  "licenseFileCount":len(licenses),
  "banks":[{"directory":d.relative_to(root).as_posix(),"audioFilesRecursive":sum(1 for p in audio if d in p.parents or p.parent==d)} for d in dirs],
  "audio":[{"path":p.relative_to(root).as_posix(),"bytes":p.stat().st_size,"sha256":digest(p)} for p in audio],
  "sfzMappings":[p.relative_to(root).as_posix() for p in maps],"unresolvedSampleReferences":missing,
  "limitations":["Inventory does not prove license compatibility, guitar identity, audible quality, SNR, clipping, DC offset, or runtime playback correctness."]}
 a.output.parent.mkdir(parents=True,exist_ok=True); a.output.write_text(json.dumps(report,indent=2,ensure_ascii=False)+"\n",encoding="utf-8")
 print(f"Sample audit: {len(audio)} audio files, {len(maps)} SFZ maps, {len(licenses)} license files, {len(missing)} unresolved references.")
 return 2 if missing else 0
if __name__=="__main__": sys.exit(main())
