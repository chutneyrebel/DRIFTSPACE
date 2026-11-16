#!/usr/bin/env python3
"""Host-only dynamic modal retuning checks."""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1]
src=r"""
#include <stdio.h>
#include <math.h>
#include "coupled_core.h"
int main(void){
 DSCoupled s;ds_coupled_init(&s,110.f);
 float old=s.resonator[0].coeff[0];
 ds_coupled_controls(&s,1.f,.5f,.5f,.5f);
 if(s.resonator[0].coeff[0]==old)return 1;
 float before=s.resonator[1].coeff[1];
 ds_coupled_controls(&s,1.f,1.f,.5f,.5f);
 if(s.resonator[1].coeff[1]==before)return 2;
 ds_coupled_strike(&s,1.f);
 for(int i=0;i<100;i++)ds_coupled_tick(&s);
 float tail=s.resonator[0].y1[0];
 if(tail==0.f)return 3;
 ds_coupled_controls(&s,.25f,.25f,.5f,.5f);
 if(s.resonator[0].y1[0]!=tail)return 4;
 for(int i=0;i<44100;i++){
  float x=ds_coupled_tick(&s);
  if(!isfinite(x)||fabsf(x)>.80001f)return 5;
 }
 puts("PASS: tension retunes; coupling retunes; ringing state preserved; bounded audio");
 return 0;
}
"""
with tempfile.TemporaryDirectory() as tmp:
 c=Path(tmp)/"test.c";c.write_text(src)
 exe=Path(tmp)/"test"
 subprocess.run(["cc","-std=c11","-O2","-I",str(root/"src"),str(c),"-lm","-o",str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
