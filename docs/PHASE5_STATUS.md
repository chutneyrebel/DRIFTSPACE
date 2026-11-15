# Phase 5: harmonic satellites (prototype)

- GENERATION controls the probability and level of a secondary, gated oscillator per held note.
- CHAOS expands a fixed musical interval palette (root, fifth, octave, third, sixth and upper extensions).
- EVOLUTION changes pattern update rate. Seeded xorshift makes decisions reproducible after reset.
- FREEZE holds generated pattern selection and gate; it does **not** freeze audio.
- Root note is preserved; generated harmonics are internal audio, not MIDI note output.
- The oscillator sine approximation was made continuous at half-cycle to remove a hard discontinuity.

## First Ableton Move hardware test (recommended now)

1. Confirm the package contains dsp.so, module.json and canvas.js.
2. Install only on a test setup with Schwung; back up existing presets.
3. Load DRIFTSPACE, play and release one note; confirm no stuck voices.
4. Switch all four worlds and sweep FREE PAIR and JOURNEY morph.
5. Sweep GENERATION from 0 to 100 while holding one note; listen for harmonic additions.
6. Test CHAOS and EVOLUTION at moderate values, then extremes at low output level.
7. Toggle FREEZE and confirm evolving patterns pause without silencing audio.
8. Confirm screen renders, encoders map correctly, and no crashes/dropouts occur.

**Not hardware validated.** CI passing cannot establish audio quality, performance headroom or Schwung canvas compatibility. Generative output is not yet tempo-synced or scale-selectable; interval palette is fixed and non-diatonic across arbitrary chord contexts.
