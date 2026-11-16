#!/usr/bin/env python3
"""Host-only coupling test, not Schwung hardware validation."""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1]
src=r"""
#include <stdio.h>
#include <math.h>
#include "coupled_core.h"
int main(void){
 DSCoupled s,t;ds_coupled_init(&s,110.f);ds_coupled_init(&t,110.f);
 for(int i=0;i<512;i++)if(ds_coupled_tick(&s)!=0.f)return 1;
 ds_coupled_strike(&s,.8f);ds_coupled_strike(&t,.8f);
 float first=0.f,second=0.f;
 for(int i=0;i<44100;i++){
  float a=ds_coupled_tick(&s),b=ds_coupled_tick(&t);
  if(!isfinite(a)||fabsf(a)>.80001f||a!=b)return 2;
  first+=fabsf(s.pickup[0]);second+=fabsf(s.pickup[1]);
 }
 if(first<.01f||second<.01f)return 3;
 if(s.physics.steps!=200)return 4;
 s.physics.frozen=1;
 unsigned before=s.physics.steps;
 float tail=0.f;
 for(int i=0;i<256;i++){tail+=fabsf(ds_coupled_tick(&s));}
 if(s.physics.steps!=before||tail<=0.f)return 5;
 puts("PASS: coupled engine silence, deterministic audio, both resonators, 200 Hz physics, freeze tail");
 return 0;
}
"""
with tempfile.TemporaryDirectory() as tmp:
 c=Path(tmp)/"test.c";c.write_text(src)
 exe=Path(tmp)/"test"
 subprocess.run(["cc","-std=c11","-O2","-Wall","-Wextra","-I",str(root/"src"),str(c),"-lm","-o",str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
