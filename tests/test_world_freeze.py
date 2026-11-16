#!/usr/bin/env python3
"""FREEZE captures spectral state, holds nonzero audio, releases smoothly."""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1]
src=r"""
#include <math.h>
#include <stdio.h>
#include "world_render.h"
int main(void){
 DSWorldRender r;ds_wr_init(&r,110.f,1,3);
 ds_we_controls(&r.exchange,.5f,.8f,.1f);ds_wr_note(&r,1.f);
 for(int i=0;i<5000;i++)ds_wr_sample(&r,44100.f,1.f);
 ds_wr_freeze(&r,1);
 float captured=r.frozen_ratio[0][4];
 float power=0.f;
 for(int i=0;i<15000;i++){float x=ds_wr_sample(&r,44100.f,1.f);power+=x*x;}
 if(power<.0001f)return 1;
 if(fabsf(r.frozen_ratio[0][4]-captured)>1e-7f)return 2;
 if(r.freeze_blend<.99f)return 3;
 ds_wr_freeze(&r,0);
 for(int i=0;i<15000;i++)ds_wr_sample(&r,44100.f,1.f);
 if(r.freeze_blend>.01f)return 4;
 puts("PASS: spectral capture, non-silent freeze, smooth release");
 return 0;
}
"""
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/"test.c";p.write_text(src);exe=Path(d)/"test"
 subprocess.run(["cc","-std=c11","-O2","-I",str(root/"src"),str(p),"-lm","-o",str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
