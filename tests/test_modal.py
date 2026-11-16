#!/usr/bin/env python3
"""Host-only tests for independent modal resonators."""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1]
src=r"""
#include <stdio.h>
#include <math.h>
#include "modal_core.h"
int main(void){
 DSModal a,b,c;
 ds_modal_init(&a,110.f,0);
 ds_modal_init(&b,110.f,1);
 ds_modal_init(&c,110.f,0);
 if(ds_modal_tick(&a)!=0.f)return 1;
 ds_modal_strike(&a,.5f);ds_modal_strike(&b,.5f);ds_modal_strike(&c,.5f);
 float difference=0.f,energy=0.f;
 for(int i=0;i<44100;i++){
  float x=ds_modal_tick(&a),y=ds_modal_tick(&b),z=ds_modal_tick(&c);
  if(!isfinite(x)||!isfinite(y)||fabsf(x)>.80001f||fabsf(y)>.80001f)return 2;
  if(x!=z)return 3;
  difference+=fabsf(x-y);
  energy+=fabsf(x);
 }
 if(difference<.01f||energy<.01f)return 4;
 puts("PASS: modal silence, excitation, material distinction, determinism, finite bounded output");
 return 0;
}
"""
with tempfile.TemporaryDirectory() as tmp:
    source=Path(tmp)/"test.c";source.write_text(src)
    exe=Path(tmp)/"test"
    subprocess.run(["cc","-std=c11","-O2","-Wall","-Wextra","-I",str(root/"src"),str(source),"-lm","-o",str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
