# Hardware test 01: compatibility repair

Reported: 9 PASS / 5 FAIL on physical Ableton Move.

## Confirmed source issues and repairs

- Canvas used `drawLine` and `drawCircle`, whereas working Synth_esis canvas uses `line` and `fillRect`. Replaced calls and aligned canvas mock.
- Root page had only eight knobs; SPACE, TONE and FREEZE were not mapped to encoders. Added SCULPT and MOTION subpages; FREEZE is on MOTION encoder 4, SPACE is on SCULPT encoder 5.
- World B contributes zero audio when MORPH=0. Set default MORPH=50 and initialize smoothing at midpoint. It remains expected that World B does not contribute at MORPH=0.
- JOURNEY mapping already interpolates three adjacent pairs; re-test now that MORPH responds more quickly. Not yet proven corrected on hardware.
- SPACE was only subtle mono-linked pan. Increased stereo motion; test with headphones or stereo outputs.
- Generated harmonic level increased; monitor headroom at high polyphony and high GENERATION.
- MORPH uses smoothstep blending to reduce sudden tonal changes, but ring artifacts require hardware listening.

## Next hardware acceptance

1. Root FACE animates during a held note, without display errors.
2. SCULPT and MOTION pages appear and each encoder changes the intended parameter.
3. FREEZE holds modulation while sound continues; test with CHAOS and GENERATION active.
4. FREE PAIR: WORLD A audible at MORPH 0; WORLD B audible at MORPH 100; both audible at 50.
5. JOURNEY: MORPH 0/33/67/100 progresses FLUID/ORGANISM/DRIFTSPACE/BLOOM.
6. SPACE changes stereo field on headphones; not reliably detectable through mono speaker.
7. Check world identity and generated voice loudness, no clipping or stuck notes.

Hardware outcomes are unverified for this patch. No new feature phase should start until retest.
