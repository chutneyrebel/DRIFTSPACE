# DRIFTSPACE Physics Engine — Experimental Development Plan

Status: architecture and baseline only. Do not install this branch on Move until a new physics DSP build passes CI and hardware-test instructions are issued.

## Isolation
- Branch: `experiment/physics-engine`, forked from `phase1-audio-prototype`.
- Do not modify `main`, merge, or publish a release without explicit user approval.
- Existing DSP and canvas behavior are baseline references, not verified fixes.
- Maintain compatibility with Schwung plugin API v2, 128x64 monochrome canvas, and 8 encoders per page.

## References and provenance
- Fizzik: https://github.com/filliformes/fizzik-move (MIT): C DSP, 2 coupled resonators, waveguide, modal banks, cross-feedback safety, native tests, pressure.
- Surge XT port: https://github.com/charlesvestal/schwung-surge (GPL-3.0): embedded/headless build and integration reference only; do not copy GPL code without a licensing decision.
- Anukari: https://anukari.com/ : conceptual inspiration; no proprietary code/assets.

## Engine contracts
1. Physics: two 1D masses, anchors, spring, damping, bounded external forces; fixed step target 200 Hz. Semi-implicit Euler with numerical stability tests.
2. Sound: one voice, two independently audible resonators; begin with 6–8 modes per modal object, plus a selectable lightweight string waveguide. Real-time audio must not allocate, lock, print, or wait for UI.
3. Coupling: feedback with DC blocking, saturation, finite checks, and output headroom. Position controls coupling coefficient; energy controls excitation rather than output gain.
4. Parameters: FORM, TENSION, COUPLING, DAMPING, ENERGY, MOTION, CHAOS, SPACE. FREEZE pauses physical state changes but allows audio resonance to ring.
5. Visual: positions, links, and energy derive from physics snapshots published safely from DSP. Do not invent independent time-based animation to imply DSP state. Check Schwung's actual canvas data transport before implementing.
6. State: deterministic reset, state round-trip, stable note-off, bounded simulation, and backward-compatible host metadata.

## Implementation gates
P0 baseline: existing host DSP and canvas tests pass; ARM64 package builds on experiment branch.
P1: independent two-resonator mono DSP and native tests (silence, note-off, stability, energy, bounded feedback).
P2: fixed-step physics with determinism and extreme-parameter stress tests.
P3: physical parameters audibly drive resonator excitation, tuning, and coupling.
P4: real physics-to-canvas snapshot transport validated against host contract.
P5: third object only after Move CPU benchmark and P1–P4 hardware acceptance.

## Required hardware validation before advancement
- Both resonators independently audible and distinct.
- Coupling audibly changes held notes.
- ENERGY, TENSION, DAMPING, MOTION respond without clicks.
- FREEZE holds physics without muting audio.
- Canvas matches real physics, with no crashes or encoder misrouting.
- No NaN/Inf, stuck notes, runaway volume, or severe dropouts.
- Measure processing headroom before adding polyphony or third resonator.

## Reporting protocol
Every development update must state:
- Fixed/implemented (verified versus unverified).
- Remaining work and known failures.
- Exact tests to perform.
- Whether to install on Ableton Move.
- Whether awaiting user approval, build results, or hardware results.
No claim of hardware success from CI alone.
