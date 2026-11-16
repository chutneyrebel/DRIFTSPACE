#!/usr/bin/env python3
"""Host-only physical control mapping checks."""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1]
src=r"""
#include <stdio.h>
#include <math.h>
#include "coupled_core.h"
static float run(float t,float c,float d,float e,int which){
 DSCoupled s;ds_coupled_init(&s,110.f);ds_coupled_controls(&s,t,c,d,e);
 ds_coupled_strike(&s,.8f);
 float metric=0.f;
 for(int i=0;i<44100;i++){
  float a=ds_coupled_tick(&s);
  if(!isfinite(a)||fabsf(a)>.80001f)return -1.f;
  if(i>22050)metric+=fabsf(which==1?s.physics.x[1]:a);
 }
 return metric;
}
int main(void){
 DSCoupled s;ds_coupled_init(&s,110.f);
 ds_coupled_controls(&s,2.f,-1.f,2.f,-1.f);
 if(s.tension!=1.f||s.coupling!=0.f||s.damping!=1.f||s.energy!=0.f)return 1;
 float low=run(.5f,0.f,.5f,.5f,1),high=run(.5f,1.f,.5f,.5f,1);
 if(low<0.f||high<0.f||high<=low+.001f)return 2;
 float soft=run(.5f,.5f,.5f,0.f,0),hard=run(.5f,.5f,.5f,1.f,0);
 if(soft<0.f||hard<=soft+.01f)return 3;
 DSCoupled a,b;ds_coupled_init(&a,110.f);ds_coupled_init(&b,110.f);
 ds_coupled_controls(&a,0.f,.5f,.5f,.5f);
 ds_coupled_controls(&b,1.f,.5f,.5f,.5f);
 if(a.physics.stiffness==b.physics.stiffness)return 4;
 ds_coupled_controls(&a,.5f,.5f,0.f,.5f);
 ds_coupled_controls(&b,.5f,.5f,1.f,.5f);
 if(a.physics.damping==b.physics.damping)return 5;
 puts("PASS: controls clamped; coupling transfers motion; energy affects audio; tension and damping map to physics");
 return 0;
}
"""
with tempfile.TemporaryDirectory() as tmp:
 c=Path(tmp)/"test.c";c.write_text(src)
 exe=Path(tmp)/"test"
 subprocess.run(["cc","-std=c11","-O2","-I",str(root/"src"),str(c),"-lm","-o",str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
