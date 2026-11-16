#!/usr/bin/env python3
"""Read-only inventory of canonical guitar audio and SFZ sample references.

Uses a precomputed suffix index so large libraries do not degrade into an
O(SFZ references × audio files) scan. Quoted SFZ paths may contain spaces.
"""
import argparse, hashlib, json, re, sys
from collections import Counter, defaultdict
from pathlib import Path

AUDIO={".wav",".flac",".ogg",".aiff",".aif",".mp3"}
ASSIGNMENT=re.compile(
    r"""(?im)^\s*(sample|default_path)\s*=\s*(?:"([^"]*)"|'([^']*)'|([^\s;]+))"""
)

def digest(p):
    h=hashlib.sha256()
    with p.open("rb") as f:
        for block in iter(lambda:f.read(1024*1024),b""):
            h.update(block)
    return h.hexdigest()

def norm(value):
    return value.replace("\\\\","/").replace("\\","/").strip()

def inside(root, candidate):
    try:
        candidate.relative_to(root)
        return True
    except ValueError:
        return False

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--root",type=Path,required=True)
    ap.add_argument("--output",type=Path,required=True)
    a=ap.parse_args()
    root=a.root.resolve()
    if not root.is_dir():
        raise SystemExit("Sample Library root missing")

    audio=sorted(p for p in root.rglob("*") if p.is_file() and p.suffix.lower() in AUDIO)
    maps=sorted(p for p in root.rglob("*") if p.is_file() and p.suffix.lower()==".sfz")
    licenses=sorted(p for p in root.rglob("*") if p.is_file() and p.name.lower() in {"license","license.txt","copying","copying.txt"})
    if len(audio)<100:
        raise SystemExit(f"Expected >=100 audio files, found {len(audio)}")

    # Index every suffix once: e.g. "samples/guitar/a.wav", "guitar/a.wav",
    # and "a.wav". Matching later is O(path components), not O(all audio).
    suffix_index=defaultdict(list)
    relative_by_path={}
    for p in audio:
        rel=p.relative_to(root).as_posix()
        relative_by_path[p]=rel
        parts=rel.casefold().split("/")
        for i in range(len(parts)):
            suffix_index["/".join(parts[i:])].append(p)

    missing=[]
    resolved_via_fallback=0
    references=0
    for sfz in maps:
        source=sfz.read_text(encoding="utf-8",errors="replace")
        assignments=[]
        for m in ASSIGNMENT.finditer(source):
            key=m.group(1).lower()
            value=next((v for v in m.groups()[1:] if v is not None),"")
            assignments.append((key,value))
        defaults=[norm(v) for key,v in assignments if key=="default_path"]
        # SFZ default_path is scoped; the last declaration in this map is used
        # as the conservative map-level fallback for this inventory audit.
        default_path=defaults[-1] if defaults else ""
        for key,raw in assignments:
            if key!="sample":
                continue
            references+=1
            normalized=norm(raw)
            rel=Path(normalized)
            sfz_rel=sfz.relative_to(root).as_posix()
            # SFZ libraries commonly keep maps in nested folders and use safe
            # parent-relative references such as ../Samples/guitar/note.wav.
            # Do not reject '..' lexically: resolve each candidate first, then
            # enforce the canonical library-root boundary (also blocks symlink
            # escapes and absolute paths).
            if rel.is_absolute():
                missing.append({"sfz":sfz_rel,"sample":raw,"error":"unsafe_or_absolute_path"})
                continue

            candidates=[]
            if default_path:
                candidates.append((sfz.parent/Path(default_path)/rel).resolve())
            candidates.append((sfz.parent/rel).resolve())
            candidates.append((root/rel).resolve())
            safe_candidates=[p for p in candidates if inside(root,p)]
            found=next((p for p in safe_candidates if p.is_file()),None)
            if found is None and not safe_candidates:
                missing.append({"sfz":sfz_rel,"sample":raw,"default_path":default_path,"error":"unsafe_or_absolute_path"})
                continue

            if found is None:
                suffix=(Path(default_path)/rel).as_posix() if default_path else rel.as_posix()
                matches=suffix_index.get(suffix.casefold(),[])
                if len(matches)==1:
                    found=matches[0]
                    resolved_via_fallback+=1
                elif len(matches)>1:
                    missing.append({"sfz":sfz_rel,"sample":raw,"default_path":default_path,"error":"ambiguous_suffix","matches":[relative_by_path[p] for p in matches[:10]]})
                    continue
            if found is None:
                missing.append({"sfz":sfz_rel,"sample":raw,"default_path":default_path,"error":"missing_file"})

    dirs=sorted({p.parent for p in audio})
    report={
        "schemaVersion":"1.2.0",
        "audit":"READ_ONLY_CONTENT_INVENTORY",
        "root":"Sample Library/",
        "audioFileCount":len(audio),
        "audioBytes":sum(p.stat().st_size for p in audio),
        "audioExtensions":dict(Counter(p.suffix.lower() for p in audio)),
        "sfzFileCount":len(maps),
        "sfzSampleReferenceCount":references,
        "licenseFileCount":len(licenses),
        "resolvedByUniqueSuffixFallback":resolved_via_fallback,
        "banks":[{"directory":d.relative_to(root).as_posix(),"audioFilesRecursive":sum(1 for p in audio if d in p.parents or p.parent==d)} for d in dirs],
        "audio":[{"path":relative_by_path[p],"bytes":p.stat().st_size,"sha256":digest(p)} for p in audio],
        "sfzMappings":[p.relative_to(root).as_posix() for p in maps],
        "unresolvedSampleReferences":missing,
        "limitations":["Inventory does not prove license compatibility, guitar identity, audible quality, SNR, clipping, DC offset, or runtime playback correctness.","SFZ control-flow opcodes and nested #include semantics require runtime validation."]
    }
    a.output.parent.mkdir(parents=True,exist_ok=True)
    a.output.write_text(json.dumps(report,indent=2,ensure_ascii=False)+"\n",encoding="utf-8")
    print(f"Sample audit: {len(audio)} audio files, {len(maps)} SFZ maps, {references} references, {licenses.__len__()} license files, {resolved_via_fallback} suffix-resolved, {len(missing)} unresolved.")
    for item in missing[:30]:
        print("UNRESOLVED:",json.dumps(item,ensure_ascii=False))
    return 2 if missing else 0

if __name__=="__main__":
    sys.exit(main())
