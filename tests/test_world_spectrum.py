#!/usr/bin/env python3
"""Validate distinct spectra and evolution without relying on Move hardware."""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1]
src=r"""
#include <stdio.h>
#include <math.h>
#include "world_spectrum.h"
int main(void){
 DSWorldExchange p;ds_we_init(&p);
 DSWorldSpectrum s;ds_ws_init(&s);
 float signatures[4][DS_WORLD_PARTIALS];
 for(int w=0;w<4;w++){
  ds_ws_update(&s,&p,w,w,0.f);
  for(int i=0;i<DS_WORLD_PARTIALS;i++)signatures[w][i]=s.ratio[0][i];
 }
 for(int a=0;a<4;a++)for(int b=a+1;b<4;b++){
  float distance=0.f;
  for(int i=0;i<DS_WORLD_PARTIALS;i++)distance+=fabsf(signatures[a][i]-signatures[b][i]);
  if(distance<1.f)return 1;
 }
 ds_we_controls(&p,.5f,.8f,.3f);ds_we_strike(&p,1.f);
 ds_ws_init(&s);ds_ws_update(&s,&p,2,3,1.f);
 float first=s.ratio[0][6];float first_gain=s.gain[1][5];
 for(int k=0;k<1000;k++){ds_we_step(&p,.005f);ds_ws_update(&s,&p,2,3,1.f);}
 if(fabsf(first-s.ratio[0][6])<.001f)return 2;
 if(fabsf(first_gain-s.gain[1][5])<.001f)return 3;
 for(int w=0;w<2;w++)for(int i=0;i<DS_WORLD_PARTIALS;i++){
  if(!isfinite(s.ratio[w][i])||!isfinite(s.gain[w][i]))return 4;
  if(s.gain[w][i]<0.f||s.gain[w][i]>1.f)return 5;
 }
 puts("PASS: four distinct spectral signatures, evolving pitch and gain, bounded outputs");
 return 0;
}
"""
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/"t.c";p.write_text(src)
 exe=Path(d)/"t"
 subprocess.run(["cc","-std=c11","-O2","-I",str(root/"src"),str(p),"-lm","-o",str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
