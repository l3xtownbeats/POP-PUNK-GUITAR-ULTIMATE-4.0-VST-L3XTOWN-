from __future__ import annotations

from pathlib import Path
import subprocess
import sys
import unittest


class SamplesAuditPolicyTests(unittest.TestCase):
    def test_audit_has_no_legacy_size_limit_arguments(self):
        script = Path(__file__).resolve().parents[1] / "tools" / "audit_soundbank.py"
        result = subprocess.run(
            [sys.executable, str(script), "--help"],
            text=True,
            capture_output=True,
            check=False,
        )
        self.assertEqual(result.returncode, 2)
        self.assertNotIn("--hard-limit-gb", result.stdout + result.stderr)
        self.assertNotIn("--preferred-gb", result.stdout + result.stderr)

    def test_audit_source_declares_unlimited_rev18_policy(self):
        script = Path(__file__).resolve().parents[1] / "tools" / "audit_soundbank.py"
        source = script.read_text(encoding="utf-8")
        self.assertIn("UNLIMITED (REV.18)", source)
        self.assertNotIn("hard_limit_gb", source)
        self.assertNotIn("preferred_gb", source)


if __name__ == "__main__":
    unittest.main()
