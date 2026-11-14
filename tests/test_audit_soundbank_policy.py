from __future__ import annotations

from pathlib import Path
import unittest


class SamplesAuditPolicyTests(unittest.TestCase):
    def test_audit_source_declares_unlimited_rev18_policy(self):
        script = Path(__file__).resolve().parents[1] / "tools" / "audit_soundbank.py"
        source = script.read_text(encoding="utf-8")
        self.assertIn("UNLIMITED (REV.18)", source)
        self.assertNotIn("--hard-limit-gb", source)
        self.assertNotIn("--preferred-gb", source)
        self.assertNotIn("hard_limit_gb", source)
        self.assertNotIn("preferred_gb", source)


if __name__ == "__main__":
    unittest.main()
