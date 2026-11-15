# DRIFTSPACE proposed technical architecture

Status: conceptual contracts, not code.

## Boundaries
1. **Schwung adapter:** module descriptor, v2 DSP ABI, MIDI and parameter bridge, canvas and preset integration.
2. **Voice manager:** note-on/off, allocation, one-shot/sustain/infinite, stealing and panic.
3. **Shared DSP:** oscillator/additive core, filters, envelopes, modulators, effects and safety.
4. **World behaviors:** FLUID phase/spectral deformation; ORGANISM coupled modulation; DRIFTSPACE partial drift; BLOOM harmonic and rhythmic generation.
5. **PORTAL:** journey/free pair selection, mapping between world parameters, smoothing and optional short dual-path crossfades.
6. **Evolution/generation:** deterministic seeded stochastic processes, musical constraints and timing.
7. **Capture:** parameter snapshot ring, favourites, undo, optional bounded audio-freeze ring.
8. **Renderer:** pure canvas draw routines using compact read-only snapshots; no audio-thread drawing or file I/O.

## Proposed state contract
Versioned state includes world selection, journey/free pair configuration, morph position, global macros, world-local macros, evolution/chaos/generation modes and amounts, scale or note constraints, note lifecycle, random seed and snapshot metadata. Do not serialize active voices or raw audio buffers as ordinary presets. Validate ranges, finite values and schema version before applying.

## Real-time contract
No unbounded allocations, filesystem access, rendering or locks on audio callback. Preallocate voices/buffers. Parameter changes must be smoothed and DSP-safe. UI state reads should use bounded immutable snapshots or host-compatible scalar/string fields. Audio-freeze buffer memory and lifetime require measurement.

## Host integration
Prefer native Schwung chainable sound-generator module (API v2, dsp.so). Canvas pages inherit the enclosing hierarchy's knobs; native page planner may add a separate knob grid. Keep module.json under the documented 8 KB limit. Verify live DSP get_param metadata parity and preset round-trip. Avoid assuming host supports atomic multi-parameter transactions.

## Testing
Host-side DSP and state tests; JS canvas smoke tests; AArch64 cross-build and package inspection; on-Move MIDI, encoder, navigation, display, CPU/memory, glitch and long-sustain tests. A green CI run never replaces hardware testing.
