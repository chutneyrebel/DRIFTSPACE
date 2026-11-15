# DRIFTSPACE v0.1 proposed specification

Status: **Design-review candidate, not implementation authorization.** DRIFTSPACE is a working name.

## Product promise
A beginner can hold one note, turn two controls and discover a living sound. An experienced musician can explore deeply without fighting menus.

## Four worlds
- **FLUID:** flowing spectral and phase deformation, ripples and elastic timbre.
- **ORGANISM:** coupled modulators and bounded stochastic voice behavior.
- **DRIFTSPACE:** additive harmonic drift, spatial motion and slowly evolving texture.
- **BLOOM:** harmonic branching, rhythmic pulses and constrained melodic fragments.
- **PORTAL is not a fifth world:** it transitions between the four worlds.

## Performance controls
EXPLORE proposed encoder map (1–8): World, Morph, Evolution, Chaos, Generation, Space, Tone, Level. SCULPT replaces mappings with contextual world-specific controls. CAPTURE exposes freeze, snapshots, favourites, history and undo. The host's native navigation and encoder-page behavior must be respected.

## Navigation
- **JOURNEY:** morph along an ordered sequence of adjacent worlds. Proposed order FLUID → ORGANISM → DRIFTSPACE → BLOOM; order is adjustable in later design reviews.
- **FREE PAIR:** morph between any selected two worlds.
- Morph transitions may interpolate compatible DSP parameters and/or crossfade independently rendered paths; behavior is pair-dependent and must be click-free.

## Evolution and musicality
- Gentle evolution at baseline; adjustable rate and intensity.
- Modes: organic/slow, tempo-synced rhythmic, dramatic/unpredictable.
- Chaos modes: MUSICAL preserves pitch relationships and constraints; UNRESTRICTED permits freer pitch, rhythm and timbre variation. Both must retain amplitude and finite-value safety.
- Generative material: harmonic layers, pulses, melodic fragments. Constraints: scale-aware, played-note-aware, unrestricted. Adjustable amount.
- Note lifecycles: one-shot, sustained, infinite; panic and bounded voice management required.

## Capture
- Parameter FREEZE holds modulation/parameter evolution; notes can continue.
- Audio FREEZE captures bounded audio and sustains it; experimental and hardware-dependent.
- Undo restores parameter snapshots, not previously heard audio.
- Discovery history, snapshots and favourites; native Schwung preset integration preferred where compatible.
- Persist random seeds and parameter states to enable repeatable discoveries.

## Visuals
- Four distinct world animation personalities, designed in parallel with audio.
- Move display is monochrome 128×64; no assumed color support.
- Rendering must be independent of audio processing; visual state is an approximation unless verified otherwise.

## Scope
v0.1 prioritizes shared DSP, four differentiated worlds, PORTAL navigation, baseline evolution, basic chaos/generation, parameter snapshots and animated pages. Audio freeze, granular comparison and a sophisticated discovery browser are experimental and may be deferred. No DSP implementation before explicit approval.

## Acceptance
Playable on Move, smooth note-on/off and world transitions, no stuck voices or unsafe peaks, reliable encoder mapping, responsive graphics, repeatable saved discoveries, and meaningful sonic differences among all four worlds. Set numeric CPU/memory/latency budgets only after profiling.
