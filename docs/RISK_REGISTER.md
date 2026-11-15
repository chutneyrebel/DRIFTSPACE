# DRIFTSPACE v0.1 risk register

| ID | Risk | Impact | Mitigation / release gate |
| --- | --- | --- | --- |
| R01 | Host ABI or metadata mismatch | Critical | Verify upstream v2 callbacks and live metadata before coding |
| R02 | CPU overload on dual-path morph | High | Benchmark on Move; bounded polyphony, fallback to parameter morph |
| R03 | Audio callback stalls | Critical | No allocation, locks, I/O or graphics in realtime path |
| R04 | Clicks / abrupt level changes | High | Parameter smoothing, crossfade and peak monitoring |
| R05 | Stuck infinite voices | High | Panic, voice stealing and deterministic cleanup |
| R06 | Unrestricted chaos produces invalid values | Critical | Finite checks, bounded gain and safe recovery |
| R07 | Audio-freeze buffer memory growth | High | Fixed buffers, measured capacity and explicit ownership |
| R08 | Canvas overhead or illegibility | Medium | Limited draw complexity, update rate, on-device tests |
| R09 | Duplicate canvas/grid pages | Medium | Respect page planner, no undocumented metadata flags |
| R10 | Preset state corruption / partial apply | High | Versioned validation, rollback snapshots, host integration tests |
| R11 | Undo fails to reproduce sound | Medium | Seeded randomness and immutable parameter snapshots |
| R12 | Worlds lack sonic differentiation | High | Listening comparison with one-note and full-pattern tests |
| R13 | Metadata exceeds loader limit | High | CI enforce descriptor <8192 bytes and live parity |
| R14 | Upstream updates break module | High | Pin tested upstream revision, CI and hardware regression checks |

Severity is preliminary; no quantitative likelihood is claimed.
