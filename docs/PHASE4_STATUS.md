# Phase 4: living world engines

Development branch only. No physical Move hardware validation.

- **FLUID**: harmonic phase-bending and slowly changing spectral flow.
- **ORGANISM**: two coupled modulators plus seeded, smoothed stochastic timbral motion.
- **DRIFTSPACE**: gently inharmonic partials with independent slow modulation.
- **BLOOM**: evolving harmonic branches controlled by tone, chaos and secondary motion.
- **FREEZE**: holds periodic and random modulation state, not audio samples.
- **CHAOS**: bounded timbral complexity; no autonomous note generation.
- **JOURNEY**: continues to morph between adjacent world pairs.
- **CPU discipline**: random targets update once every 1024 samples, with sample-level smoothing; fixed storage and no heap operations in render.

Still pending: true generative MIDI/notes, snapshot/undo, audio freeze, calibrated CPU benchmarks, hardware display/knob verification and audio quality review.

Validation: CI host audio test now checks all four world outputs differ. This does not establish perceptual quality, sustained hardware CPU headroom or click-free behavior.
