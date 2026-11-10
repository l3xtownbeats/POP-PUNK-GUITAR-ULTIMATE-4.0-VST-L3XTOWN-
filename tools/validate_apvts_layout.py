#!/usr/bin/env python3
"""Static regression guard for the JUCE APVTS registry and state persistence."""
from __future__ import annotations

import re
import sys
from pathlib import Path

SOURCE = Path(sys.argv[1] if len(sys.argv) > 1 else "Source/PluginProcessor.cpp")
source = SOURCE.read_text(encoding="utf-8")

# These IDs are part of the saved-session/automation contract. Keep them stable:
# adding parameters is allowed, silently renaming/removing an existing ID is not.
REQUIRED_PARAMETER_IDS = {
    "master", "voices", "velocity", "dynamics", "palmMuteThreshold",
    "tuning", "pan", "output", "playMode", "articulation", "pickStyle",
    "pitchBendTarget", "vibrato", "slideTime", "legato", "hammerPull",
    "scaleRoot", "scaleMode", "monoMode", "fretNoiseOnChordChange",
    "slideModifier", "slideVelocitySensitivity", "modWheelVibrato",
    "melodyPatternSlot", "endingLength", "palmHitOnEnding",
    "stringSlapOnEnding", "patternSlot", "patternLength", "syncMode",
    "swingBase", "globalPpqOffset", "chordSlashBass",
    "singleMidiNoteAsChordRoot", "preserveHumanTiming", "dnaChugDensity",
    "dnaHammerPullProbability", "dnaStringSpread", "dnaBeatAccent",
    "dnaFxTaste", "compressorThreshold", "compressorRatio",
    "compressorAttack", "compressorRelease", "eqLowFreq", "eqMidFreq",
    "eqMidQ", "eqHighFreq", "distortionMix", "chorusRate", "chorusDepth",
    "delayFeedback", "reverbRoomSize", "reverbDamping", "cabinetMix",
    "cabinetLowCut", "cabinetHighCut", "bpm", "swing", "humanize",
    "accent", "startOnFirstChord", "patternQuantize", "ampGain", "tone",
    "compressorOn", "compressorAmount", "eqOn", "eqLow", "eqMid",
    "eqHigh", "distortionOn", "distortionDrive", "distortionTone",
    "chorusOn", "flangerOn", "flangerMix", "flangerRate", "flangerDepth",
    "flangerFeedback", "chorusMix", "delayOn", "delayTime", "delayMix",
    "reverbOn", "reverbMix", "cabinetOn", "doubleTrackOn", "doubleTrackMix",
    "fxSlot1", "fxSlot2", "fxSlot3", "fxSlot4", "fxSlot5", "fxSlot6",
    "fxSlot7",
}

declared = set(re.findall(r'(?:floatParameter|boolParameter)\(\s*"([^"]+)"', source))
declared.update(re.findall(r'ParameterID\s*\{\s*"([^"]+)"', source))

# FX slots are generated in a bounded loop rather than written as seven literals.
if re.search(r'for\s*\(int\s+slot\s*=\s*0;\s*slot\s*<\s*7;\s*\+\+slot\)', source) and re.search(
    r'ParameterID\s*\{\s*"fxSlot"\s*\+\s*juce::String\(slot\s*\+\s*1\)', source
):
    declared.update(f"fxSlot{i}" for i in range(1, 8))

lookups = set(re.findall(r'getRawParameterValue\(\s*"([^"]+)"', source))
errors: list[str] = []

missing_required = sorted(REQUIRED_PARAMETER_IDS - declared)
if missing_required:
    errors.append("Required APVTS IDs missing: " + ", ".join(missing_required))

missing_lookups = sorted(lookups - declared)
if missing_lookups:
    errors.append("Audio parameter lookups without a declared APVTS ID: " + ", ".join(missing_lookups))

# Detect duplicate literal IDs across helper declarations and ParameterID choices.
literal_ids = re.findall(r'(?:floatParameter|boolParameter)\(\s*"([^"]+)"', source)
literal_ids += re.findall(r'ParameterID\s*\{\s*"([^"]+)"', source)
duplicates = sorted({item for item in literal_ids if literal_ids.count(item) > 1})
if duplicates:
    errors.append("Duplicate literal APVTS IDs: " + ", ".join(duplicates))

# The selected SFZ bank path is part of the saved session and must restore
# asynchronously without blocking the host's state-loading callback.
for token in (
    'state.setProperty("sfzFilePath"',
    'restoredState.getProperty("sfzFilePath")',
    'restoredSfzFile.existsAsFile()',
    'loadSfzFileAsync(restoredSfzFile, {})',
    'juce::ScopedLock lock(sampleStateLock)',
):
    if token not in source:
        errors.append("SFZ session-path persistence/restore path missing: " + token)

# APVTS state must continue to round-trip through JUCE's binary XML helpers.
for token in (
    "parameters.copyState()", "state.createXml()", "copyXmlToBinary",
    "getXmlFromBinary", "parameters.replaceState"
):
    if token not in source:
        errors.append("APVTS state persistence path missing: " + token)

if errors:
    print("APVTS layout validation FAILED:")
    for error in errors:
        print(" - " + error)
    raise SystemExit(1)

print(
    f"APVTS layout OK: {len(declared)} declared IDs, "
    f"{len(lookups)} audio-thread parameter lookups, "
    f"{len(REQUIRED_PARAMETER_IDS)} stable IDs preserved; state round-trip present."
)
