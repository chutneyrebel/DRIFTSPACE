# Source-backed compatibility audit

Reference repositories:
- https://github.com/chutneyrebel/synth_esis (existing independently developed synth)
- https://github.com/charlesvestal/schwung (upstream host)

## Verified in repository source/docs (not independently re-tested on hardware)
- Synth_esis README reports Phases 1–9 hardware-validated, eight-voice 44.1 kHz stereo DSP and multiple animated native canvas faces.
- Synth_esis `src/module.json` declares `api_version: 2`, `component_type: sound_generator`, MIDI input, audio output, chainable module, hierarchy levels with eight knob mappings, and canvas overlays.
- Synth_esis `scripts/build.sh` cross-compiles a freestanding AArch64 `dsp.so`, verifies `move_plugin_init_v2` and packages a module tarball.
- Schwung `docs/MODULES.md` documents v2 module loading and `modules/sound_generators/<id>/dsp.so` for chain generators, with an approximately 8 KB metadata cap.
- Schwung `docs/API.md` documents a 128×64 one-bit display and host drawing/refresh interfaces.
- Synth_esis `docs/PHASE0_PHASE1_COMPATIBILITY.md` notes that live DSP `get_param()` metadata, not just module.json, controls native page registration; canvas pages inherit the containing level's knob map.
- Synth_esis `docs/PHASE10_UPSTREAM_SCHWUNG_FINDINGS.md` documents upstream canvas-first plus ordinary grid duplication and generic user preset storage under the module ID.

## Unverified for DRIFTSPACE
- Four-world CPU/memory budget and polyphony at acceptable quality.
- Crossfades with two simultaneous DSP paths; all six unordered world pairs and both directions.
- Granular processing and fixed audio freeze buffers.
- Long-lived infinite voices, idle behavior and safe reset.
- Atomic full-state updates, snapshot restoration and host preset compatibility.
- Native encoder shortcuts and exact canvas frame-rate/legibility on user's installed Schwung revision.

## Required audit before code authorization
1. Read upstream chain DSP ABI headers and module examples; record exact init, process, MIDI, parameter and state callbacks.
2. Inspect Synth_esis `src/dsp.c`, `src/canvas.js`, `src/module.json`, build/test scripts for reusable patterns and licenses.
3. Compare Schwung's actual page planner and preset handler with documented behavior.
4. Establish a device test matrix and rollback/backup procedure.
5. Review source commit hashes, licenses and dependency provenance before copying any code.

**Rule:** 'Source-supported' means documented in reviewed files, not DRIFTSPACE hardware-validated.
