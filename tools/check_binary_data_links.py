#!/usr/bin/env python3
"""Fail-fast JUCE BinaryData target/include/link/order contract check."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
CMAKE = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
ERRORS = []


def fail(message):
    ERRORS.append(message)


def block(pattern, text=CMAKE):
    match = re.search(pattern, text, re.DOTALL)
    return match.group(1) if match else None


binary_blocks = list(re.finditer(
    r"juce_add_binary_data\s*\(\s*(\w+)(.*?)\)",
    CMAKE, re.DOTALL
))
binary_targets = {}
header_owners = {}
for match in binary_blocks:
    target, body = match.group(1), match.group(2)
    header = re.search(r'\bHEADER_NAME\s+(?:"([^"]+)"|([A-Za-z0-9_.-]+))', body)
    if not header:
        fail(f"{target}: juce_add_binary_data must declare a unique HEADER_NAME")
        continue
    name = header.group(1) or header.group(2)
    if name in header_owners:
        fail(f"Generated header {name} is shared by {header_owners[name]} and {target}")
    header_owners[name] = target
    binary_targets[target] = (name, match.start())

if any(re.search(r"\bHEADER_NAMESPACE\b", match.group(2)) for match in binary_blocks):
    fail("JUCE 7.0.12 does not support HEADER_NAMESPACE; use NAMESPACE")

# BinaryData input paths must exist in the checked-out source tree.
for match in binary_blocks:
    target, body = match.group(1), match.group(2)
    sources = re.search(r"\bSOURCES\b(.*)", body, re.DOTALL)
    if sources:
        for token in re.findall(r"(?<![\w/])([A-Za-z0-9_./-]+\.[A-Za-z0-9]+)", sources.group(1)):
            if not (ROOT / token).is_file():
                fail(f"{target}: BinaryData source does not exist: {token}")

source_targets = {}
for match in re.finditer(r"target_sources\s*\(\s*(\w+)(.*?)\)", CMAKE, re.DOTALL):
    target, body = match.group(1), match.group(2)
    for token in re.findall(r"(?<![\w/])((?:Source|tests)/[A-Za-z0-9_./-]+\.(?:cpp|cc|cxx))", body):
        source = (ROOT / token).resolve()
        if not source.is_file():
            fail(f"CMake source file does not exist: {token}")
            continue
        source_targets.setdefault(source, set()).add(target)

for source, targets in source_targets.items():
    content = source.read_text(encoding="utf-8")
    for header, binary_target in ((name, target) for target, (name, _) in binary_targets.items()):
        if re.search(r'#\s*include\s*[<"]' + re.escape(header) + r'[>"]', content):
            for target in targets:
                link = block(r"target_link_libraries\s*\(\s*" + re.escape(target) + r"\b(.*?)\)", CMAKE)
                if link is None or not re.search(r"(?<![\w:])" + re.escape(binary_target) + r"(?![\w])", link):
                    fail(f"{source.relative_to(ROOT)} includes {header}, but target {target} does not link {binary_target}")
                declaration = next((m.start() for m in binary_blocks if m.group(1) == binary_target), None)
                target_decl = re.search(
                    r"(?:juce_add_plugin|juce_add_console_app|add_executable)\s*\(\s*" + re.escape(target) + r"\b",
                    CMAKE
                )
                if declaration is None or target_decl is None or declaration > target_decl.start():
                    fail(f"{binary_target} must be declared before dependent target {target}")

expected = {
    "PPGUData": ("PPGUData.h", "PPGU", "Source/PluginEditor.cpp"),
    "PPGUTestData": ("PPGUTestData.h", "ppgu_processor_pattern_tests", "tests/ProcessorPatternIntegrationTests.cpp"),
}
for binary_target, (header, consumer, source_rel) in expected.items():
    if binary_target not in binary_targets:
        fail(f"Missing required binary-data target {binary_target}")
        continue
    if binary_targets[binary_target][0] != header:
        fail(f"{binary_target} must generate {header}")
    source = ROOT / source_rel
    if not source.is_file() or not re.search(r'#\s*include\s*[<"]' + re.escape(header) + r'[>"]', source.read_text(encoding="utf-8")):
        fail(f"{source_rel} must include {header}")
    link = block(r"target_link_libraries\s*\(\s*" + re.escape(consumer) + r"\b(.*?)\)", CMAKE)
    if link is None or not re.search(r"(?<![\w:])" + re.escape(binary_target) + r"(?![\w])", link):
        fail(f"{consumer} must link {binary_target}")

if ERRORS:
    print("JUCE BINARY DATA CONTRACT: FAIL")
    for error in ERRORS:
        print(f" - {error}")
    sys.exit(1)

print("JUCE BINARY DATA CONTRACT: PASS")
for target, (header, _) in sorted(binary_targets.items()):
    print(f" - {target} -> {header}")
