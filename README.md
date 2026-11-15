# DRIFTSPACE

**Working title** for an experimental evolving synthesizer for Ableton Move via [Schwung](https://github.com/charlesvestal/schwung).

DRIFTSPACE is a **separate instrument**, not a rename or fork of [Synth_esis](https://github.com/chutneyrebel/synth_esis).

## Vision
Hold a single note and discover an evolving sound worth keeping. Four distinct sonic worlds (FLUID, ORGANISM, DRIFTSPACE, BLOOM) connect through PORTAL. Exploration should be immediate, expressive, recoverable and safe.

## Project status
**Phase 0: research and specification only. No instrument DSP or UI implementation is authorized yet.** The specification is proposed pending review and hardware validation. DRIFTSPACE is a working name.

## Design decisions
- Shared DSP foundation with one distinctive synthesis technique per world.
- PORTAL supports ordered JOURNEY and arbitrary FREE PAIR morphing.
- Gentle continuous evolution; organic, rhythmic and unpredictable behavior modes.
- MUSICAL and UNRESTRICTED chaos, both with output safety.
- Note-aware, scale-aware and unrestricted generation.
- Parameter freeze and separate experimental audio freeze; snapshots, favourites, undo.
- One-shot, sustained and infinite note lifecycles.
- Eight contextual encoders; distinct 128x64 monochrome world animations developed alongside sound.
- Software and hardware validation in parallel; prioritize a playable v0.1 experiment.

See `docs/` on the Phase 0 review branch for proposed contracts, risks and test gates. Do not treat proposed features as hardware-verified.
