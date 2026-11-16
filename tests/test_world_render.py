#!/usr/bin/env python3
"""Host audio acceptance: four worlds render finite, distinct non-silent waveforms."""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1]
src=r"""
#include <stdio.h>
#include <math.h>
#include "world_render.h"
#define N 22050
static float audio[4][N];
int main(void){
 for(int w=0;w<4;w++){
  DSWorldRender r;ds_wr_init(&r,110.f,w,w);
  ds_we_controls(&r.exchange,0.f,.75f,0.f);
  ds_wr_note(&r,1.f);
  float power=0.f;
  for(int i=0;i<N;i++){
   float v=ds_wr_sample(&r,44100.f,1.f);
   if(!isfinite(v)||fabsf(v)>1.f)return 1;
   audio[w][i]=v;power+=v*v;
  }
  if(power<.001f)return 2;
 }
 for(int a=0;a<4;a++)for(int b=a+1;b<4;b++){
  double diff=0.,power=0.;
  for(int i=0;i<N;i++){double d=audio[a][i]-audio[b][i];diff+=d*d;power+=audio[a][i]*audio[a][i];}
  if(diff/(power+1e-12)<.03)return 3;
 }
 DSWorldRender coupled;ds_wr_init(&coupled,110.f,0,3);
 ds_we_controls(&coupled.exchange,.5f,1.f,0.f);
 ds_wr_note(&coupled,1.f);
 for(int i=0;i<N;i++)ds_wr_sample(&coupled,44100.f,1.f);
 if(coupled.exchange.energy[0]<.00001f||coupled.exchange.energy[1]<.00001f)return 4;
 puts("PASS: finite audio, four distinct rendered waveforms, two-world physical activity");
 return 0;
}
"""
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/"render.c";p.write_text(src)
 exe=Path(d)/"render"
 subprocess.run(["cc","-std=c11","-O2","-I",str(root/"src"),str(p),"-lm","-o",str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
