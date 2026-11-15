#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build dist/driftspace
CC="${CC:-clang}"
"$CC" --target=aarch64-linux-gnu -ffreestanding -nostdinc -isystem "$("$CC" -print-resource-dir)/include" -nostdlib -shared -fPIC -O2 -fvisibility=hidden -fno-stack-protector -fno-builtin -fuse-ld=lld -Wl,-soname,dsp.so -Wl,--no-undefined src/dsp.c -o build/dsp.so
readelf -h build/dsp.so | grep -q AArch64
nm -D build/dsp.so | grep -q move_plugin_init_v2
! readelf -d build/dsp.so | grep -q NEEDED
cp build/dsp.so src/module.json dist/driftspace/
tar -C dist -czf dist/driftspace-module.tar.gz driftspace
