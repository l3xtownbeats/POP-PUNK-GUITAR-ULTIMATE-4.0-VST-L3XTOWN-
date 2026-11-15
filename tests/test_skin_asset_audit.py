#!/usr/bin/env python3
"""Security regression tests for the read-only REV18 SVG asset audit."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from tools.audit_skin_rev18_assets import svg_safety_errors  # noqa: E402

SVG_NS = 'xmlns="http://www.w3.org/2000/svg"'


class SvgSafetyTests(unittest.TestCase):
    def test_standard_namespace_and_internal_gradient_are_allowed(self):
        source = f'<svg {SVG_NS}><defs><linearGradient id="g"/></defs><rect fill="url(#g)"/></svg>'
        self.assertEqual(svg_safety_errors(source, "safe.svg"), [])

    def test_embedded_data_image_is_allowed(self):
        source = f'<svg {SVG_NS}><image href="data:image/png;base64,AAAA"/></svg>'
        self.assertEqual(svg_safety_errors(source, "data.svg"), [])

    def test_script_element_is_rejected(self):
        source = f'<svg {SVG_NS}><script>alert(1)</script></svg>'
        self.assertTrue(any("script" in error for error in svg_safety_errors(source, "script.svg")))

    def test_foreign_object_is_rejected(self):
        source = f'<svg {SVG_NS}><foreignObject><div/></foreignObject></svg>'
        self.assertTrue(any("foreignObject" in error for error in svg_safety_errors(source, "foreign.svg")))

    def test_event_handler_attribute_is_rejected(self):
        source = f'<svg {SVG_NS}><rect onload="alert(1)"/></svg>'
        self.assertTrue(any("event-handler" in error for error in svg_safety_errors(source, "event.svg")))

    def test_external_href_is_rejected(self):
        source = f'<svg {SVG_NS}><image href="https://example.invalid/a.png"/></svg>'
        self.assertTrue(any("external href" in error for error in svg_safety_errors(source, "href.svg")))

    def test_external_css_url_is_rejected(self):
        source = f'<svg {SVG_NS}><rect style="fill:url(https://example.invalid/a.svg#x)"/></svg>'
        self.assertTrue(any("external url()" in error for error in svg_safety_errors(source, "css.svg")))

    def test_javascript_url_is_rejected(self):
        source = f'<svg {SVG_NS}><a href="javascript:alert(1)"/></svg>'
        self.assertTrue(any("javascript" in error for error in svg_safety_errors(source, "js.svg")))

    def test_dtd_and_entity_declarations_are_rejected(self):
        source = f'<!DOCTYPE svg [<!ENTITY x "bad">]><svg {SVG_NS}><text>&x;</text></svg>'
        self.assertTrue(any("DTD/entity" in error for error in svg_safety_errors(source, "entity.svg")))


if __name__ == "__main__":
    unittest.main(verbosity=2)
