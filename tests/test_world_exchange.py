#!/usr/bin/env python3
"""DRIFTSPACE 2.0 reciprocal world coupling acceptance tests."""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1]
source=r"""
#include <stdio.h>
#include <math.h>
#include "world_exchange.h"
int main(void){
 DSWorldExchange a,b;
 ds_we_init(&a);ds_we_init(&b);
 ds_we_controls(&a,0.f,0.f,0.f);
 ds_we_strike(&a,1.f);
 if(a.v[0]!=1.f||a.v[1]!=0.f)return 1;
 for(int i=0;i<200;i++)ds_we_step(&a,.005f);
 if(fabsf(a.x[1])+fabsf(a.v[1])>1e-6f)return 2;
 ds_we_controls(&b,0.f,1.f,0.f);ds_we_strike(&b,1.f);
 for(int i=0;i<200;i++)ds_we_step(&b,.005f);
 if(fabsf(b.x[1])+fabsf(b.v[1])<.01f)return 3;
 DSWorldExchange c;ds_we_init(&c);
 ds_we_controls(&c,1.f,0.f,0.f);ds_we_strike(&c,1.f);
 if(c.v[0]!=0.f||c.v[1]!=1.f)return 4;
 DSWorldExchange d;ds_we_init(&d);
 ds_we_controls(&d,.5f,.8f,0.f);ds_we_strike(&d,.8f);
 DSWorldExchange e=d;
 for(int i=0;i<5000;i++){
  ds_we_step(&d,.005f);ds_we_step(&e,.005f);
  for(int j=0;j<2;j++){
   if(!isfinite(d.x[j])||!isfinite(d.v[j])||!isfinite(d.energy[j]))return 5;
   if(d.x[j]!=e.x[j]||d.v[j]!=e.v[j])return 6;
  }
 }
 ds_we_controls(&d,-3.f,3.f,-2.f);
 if(d.morph!=0.f||d.coupling!=1.f||d.chaos!=0.f)return 7;
 puts("PASS: isolated worlds, reciprocal exchange, morph endpoints, deterministic chaos=0, finite bounded states");
 return 0;
}
"""
with tempfile.TemporaryDirectory() as tmp:
 c=Path(tmp)/"test.c";c.write_text(source)
 exe=Path(tmp)/"test"
 subprocess.run(["cc","-std=c11","-O2","-I",str(root/"src"),str(c),"-lm","-o",str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
