import importlib.util
import tempfile
import wave
from pathlib import Path

module_path = Path(__file__).resolve().parents[1] / "tools" / "soundforge" / "ppgu_soundforge.py"
spec = importlib.util.spec_from_file_location("ppgu_soundforge", module_path)
sf = importlib.util.module_from_spec(spec)
spec.loader.exec_module(sf)

with tempfile.TemporaryDirectory() as temp:
    root = Path(temp)
    wav_path = root / "note.wav"
    with wave.open(str(wav_path), "wb") as wav:
        wav.setnchannels(1); wav.setsampwidth(2); wav.setframerate(48000)
        wav.writeframes(b"\x00\x00" * 480)
    sfz = root / "guitar.sfz"
    sfz.write_text(
        "<global> ampeg_release=0.2\n"
        "<group> sample=note.wav seq_length=2\n"
        "<region> sample=note.wav key=40 lovel=1 hivel=63 seq_position=1\n"
        "<region> sample=note.wav key=40 lovel=64 hivel=127 seq_position=2\n",
        encoding="utf-8"
    )
    manifest = sf.build_manifest(sfz)
    assert manifest["ready"]
    assert manifest["available_modes"]["rhythm"] and manifest["available_modes"]["melody"]
    assert manifest["capabilities"]["velocity_layers"] == 2
    assert manifest["capabilities"]["round_robin_present"]
    assert len(manifest["assets"]) == 1
    out = root / "package"
    sf.package(sfz, out)
    assert (out / "guitar.sfz").is_file()
    assert (out / "note.wav").is_file()
    assert (out / "instrument-package.json").is_file()

    escape = root / "escape.sfz"
    escape.write_text("<region> sample=../outside.wav key=60\n", encoding="utf-8")
    try:
        sf.build_manifest(escape)
        raise AssertionError("path traversal was accepted")
    except ValueError as exc:
        assert "escapes" in str(exc)
print("PPGU Sound Forge contract tests passed")
