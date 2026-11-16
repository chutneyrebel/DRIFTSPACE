# DRIFTSPACE 2.0: physically interacting sound worlds

Status: Phase 1 implementation, isolated development branch. Do not merge without approval.

## Signal architecture
MIDI excitation -> World A and World B physical state -> bidirectional energy exchange -> world-specific modal excitation -> resonant audio -> spectral freeze -> stereo space -> level-matched output.

Each world has its own mass/spring state, material modal ratios, excitation rules and evolving spectral distribution. Morph is not an audio crossfade: it changes the direction and amount of energy transmitted between the two worlds. At the endpoints, the selected world is dominant. CHAOS=0 disables stochastic perturbation, but deterministic coupled movement remains possible.

## Worlds
- FLUID: strongly damped, slowly circulating resonance, smooth excitation.
- ORGANISM: nonlinear thresholds, irregular growth and contraction.
- DRIFTSPACE: drifting inharmonic partial relationships and coupled beating.
- BLOOM: cascading activation of upper partials, energy-triggered growth.

## Move layout (proposed)
EXPLORE: WORLD A | WORLD B | MORPH | EVOLVE | CHAOS | ENERGY | SPACE | LEVEL
PHYSICS: TENSION | COUPLING | DAMPING | MATERIAL | EXCITE | MOTION | INSTABILITY | FREEZE
SPECTRUM: HARMONICS | SPREAD | SHIFT | GROWTH | DECAY | TEXTURE | WIDTH | DEPTH

No dead encoders. PHYSICS mode will not expose controls it does not use. The animation must eventually read DSP state, not run independently.

## Acceptance gates
1. Interaction core: deterministic, bounded, reciprocal energy exchange; morph endpoints and chaos=0 tested.
2. Four independently identifiable world audio renders; automated spectral differences and time variation.
3. FREEZE sustains an audible captured spectral state, and unfreeze resumes smoothly.
4. Perceived loudness calibrated against legacy FREE PAIR; clicks and clipping tested.
5. Schwung ABI and metadata parity; ARM64 builds; encoder/visual mapping.
6. Ableton Move hardware validation. No main merge without explicit approval.
