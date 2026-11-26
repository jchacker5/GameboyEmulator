"""Exercise the shipped executable, not a mock CPU or snapshot parser."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

BINARY = str(Path(sys.argv.pop(1)).resolve())
ROOT = Path(__file__).resolve().parents[1]


class CliTests(unittest.TestCase):
    def run_cli(self, *args):
        return subprocess.run([BINARY, "--quiet", *args], cwd=ROOT,
                              env={**os.environ, "QT_QPA_PLATFORM": "offscreen"},
                              capture_output=True, text=True, timeout=10)

    def test_bundled_rom_and_export(self):
        with tempfile.TemporaryDirectory() as directory:
            png = Path(directory) / "frame.png"
            result = self.run_cli("--png", str(png))
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn("Executed 21 instructions; halted, A=21", result.stdout)
            self.assertEqual(png.read_bytes()[:8], b"\x89PNG\r\n\x1a\n")

    def test_invalid_snapshots(self):
        values = (ROOT / "assets/screendump.txt").read_text().split()
        self.assertEqual(len(values), 8208)
        invalids = [values[:-1], values + ["extra"]]
        for index, value in [(0, "-1"), (8191, "256"), (8192, "2"), (8193, "-1"),
                             (8194, "256"), (8195, "-1"), (8196, "4"), (8199, "x")]:
            invalid = values.copy()
            invalid[index] = value
            invalids.append(invalid)
        with tempfile.TemporaryDirectory() as directory:
            snapshot = Path(directory) / "bad.txt"
            png = Path(directory) / "frame.png"
            for values in invalids:
                snapshot.write_text(" ".join(values))
                result = self.run_cli("--vram", str(snapshot), "--png", str(png))
                self.assertEqual(result.returncode, 1, result.stdout)
                self.assertIn("Cannot read video snapshot", result.stderr)
                self.assertFalse(png.exists())

    def test_nonhalting_rom_fails(self):
        with tempfile.TemporaryDirectory() as directory:
            rom = Path(directory) / "loop.gb"
            png = Path(directory) / "frame.png"
            rom.write_bytes(bytes(256) + b"\xc3\x00\x01")  # JP 0x0100
            result = self.run_cli("--rom", str(rom), "--png", str(png))
            self.assertEqual(result.returncode, 1, result.stderr)
            self.assertIn("Executed 1000000 instructions; did not halt", result.stdout)
            self.assertFalse(png.exists())

    def test_file_and_option_errors(self):
        self.assertEqual(self.run_cli("--rom", "/no/such/rom").returncode, 1)
        self.assertEqual(self.run_cli("--vram", "/no/such/snapshot").returncode, 1)
        self.assertEqual(self.run_cli("--png", "/no/such/directory/out.png").returncode, 1)
        self.assertEqual(self.run_cli("--rom").returncode, 2)
        self.assertEqual(self.run_cli("--unknown").returncode, 2)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "empty.gb"
            path.write_bytes(b"")
            self.assertEqual(self.run_cli("--rom", str(path)).returncode, 1)


if __name__ == "__main__":
    unittest.main()
