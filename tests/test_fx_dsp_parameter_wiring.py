from __future__ import annotations

from pathlib import Path
import unittest


class FxDspParameterWiringTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.source = (Path(__file__).resolve().parents[1] / "Source" / "PluginProcessor.cpp").read_text(encoding="utf-8")

    def test_amp_tone_and_eq_cutoffs_are_read_by_the_dsp(self):
        for parameter in ("tone", "eqLowFreq", "eqHighFreq"):
            with self.subTest(parameter=parameter):
                self.assertGreaterEqual(self.source.count(f'"{parameter}"'), 2,
                                        f"{parameter} must be declared and consumed by DSP")

    def test_compressor_amount_is_consumed_by_the_dsp(self):
        self.assertGreaterEqual(self.source.count('"compressorAmount"'), 2)
        self.assertIn("1.0f + (gain - 1.0f) * amount", self.source)

    def test_compressor_amount_is_snapshotted_outside_sample_loop(self):
        start = self.source.index('if (parameters.getRawParameterValue("compressorOn")')
        end = self.source.index("// Flanger uses a dedicated", start)
        block = self.source[start:end]
        amount_read = block.index('getRawParameterValue("compressorAmount")')
        sample_loop = block.index("for (int sample = 0; sample < samples; ++sample)")
        self.assertLess(amount_read, sample_loop,
                        "Do not load APVTS atomics from inside the per-sample compressor loop")
        self.assertEqual(block.count('getRawParameterValue("compressorAmount")'), 1)


if __name__ == "__main__":
    unittest.main()
