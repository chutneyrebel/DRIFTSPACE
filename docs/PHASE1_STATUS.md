# Phase 1: experimental DSP prototype

**Development branch only. Not hardware-tested or release-ready.**

Implemented:
- Schwung plugin ABI v2 freestanding C sound-generator skeleton.
- Four deliberately distinct oscillator timbres (FLUID, ORGANISM, DRIFTSPACE, BLOOM).
- Pair selection and smoothed continuous morph.
- Four-voice MIDI note-on/note-off, release and panic.
- Gentle autonomous modulation; freeze pauses its phase.
- Host-side ABI/audio smoke test and AArch64 build pipeline.

Known limitations / intentionally not claimed:
- No canvas animation yet.
- No JOURNEY navigation, only direct world pair selection.
- No parameter snapshots, undo, audio freeze or granular synthesis.
- Generation is a reserved control, not an active note generator.
- Chaos and tone are simple timbral modifiers, not full behavior engines.
- Seed and mode are reserved for later implementation.
- No actual hardware CPU profiling or click-free guarantee.
- No full native preset state contract.
- Audio processing currently clamps each render request to 128 frames; validate host block sizes.

Hardware test gate: Back up existing Move sets, install only after CI is green, check module loading, all encoders, notes/release/panic, morph sweep, sound level and five-minute sustained play. Do not treat CI success as hardware approval.
