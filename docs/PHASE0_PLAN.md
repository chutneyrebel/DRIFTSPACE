# Phase 0: read-only audit and approval gates

## Gate A: Source contract
- Record Schwung source revision and exact v2 DSP entrypoints, callback types, render/parameter and state interfaces.
- Trace canvas page planner, encoder routing, native preset storage and serialization.
- Compare Synth_esis implementation with upstream; document transferable patterns and non-transferable features.
- Review licenses and attribution.

## Gate B: Test design
- Host tests: one-note evolving sound, deterministic seeds, parameter range, note-off/infinite/panic, all world pair transitions, preset roundtrip.
- Display tests: four world personalities, bounded canvas operations, page/knob labels, snapshots.
- Move tests: install/uninstall, input response, stable long notes, crossfade glitch/peak, CPU/memory, stock coexistence, rollback.
- Define performance budgets after hardware baseline, not by guesswork.

## Gate C: Authorization
- Review specification and risk register.
- Explicitly approve DSP implementation before adding any instrument code.
- Create separate feature branches and pull requests; do not merge without review.

## Deliverable
A source-backed interface matrix, updated spec, test plan, risks and an implementation order. This branch is documentation-only.
