# Architecture walkthrough
The original Spring 2024 COMP 406 assignment included a supplied Z80 CPU emulator and Qt scaffolding. The 2026 modernization is AI-assisted; review and reproduce it before discussing it as your own work.

The modern target is a background tile renderer and snapshot viewer, not a complete playable emulator. It deliberately separates deterministic video decoding from Qt presentation. The original assignment is preserved.

Video RAM -> tile map -> tile index -> tile row bitplanes -> palette -> 160x144 frame -> QImage -> QWidget paintEvent.

Each pixel maps through scrolling modulo 256, selects an 8x8 tile, decodes two bitplanes and applies a four-entry palette. Signed tile mode uses the 0x1000 base. Rendering is O(width*height).

QApplication::exec starts the event loop. A QTimer requests demo updates roughly every 16 ms; it is not a hard real-time clock. The timeout updates the frame and calls QWidget::update, which schedules a paint event and allows Qt to coalesce requests. paintEvent draws a single QImage scaled with aspect ratio preserved. All widget access stays on the GUI thread. For a full emulator, move CPU work off the GUI thread or schedule bounded batches, and transfer immutable frames via queued signals. Never block the event loop with an unbounded CPU loop.

Tests exercise bitplanes, palette, scroll wrap, both tile maps, signed tile addressing and snapshot validation. An offscreen GUI smoke test checks startup and event-loop exit, not visual correctness.

Tesla connection: deterministic rendering, separation of model and presentation, UI responsiveness, bounded work, and testable graphics logic. Be explicit about starter code and AI-assisted modernization. Do not claim QML, production automotive experience or complete ROM compatibility from this project.
