#!/usr/bin/env python3
"""Standalone physics contract tests. No Move or DSP integration implied."""
from pathlib import Path
import subprocess, tempfile
root=Path(__file__).resolve().parents[1]
src=r"""
#include <stdio.h>
#include <math.h>
#include "physics_core.h"
int main(void){
 DSPhysics p; ds_physics_init(&p);
 ds_physics_strike(&p,0.8f);
 for(int i=0;i<10000;i++){
   ds_physics_step(&p,0.005f);
   for(int j=0;j<2;j++) if(!isfinite(p.x[j])||!isfinite(p.v[j])||!isfinite(p.energy[j]))return 1;
 }
 if(p.steps!=10000)return 2;
 DSPhysics q;ds_physics_init(&q);ds_physics_strike(&q,0.8f);
 for(int i=0;i<10000;i++)ds_physics_step(&q,0.005f);
 if(p.x[0]!=q.x[0]||p.x[1]!=q.x[1])return 3;
 float before=p.x[0];unsigned steps=p.steps;
 p.frozen=1;ds_physics_strike(&p,1.f);ds_physics_step(&p,0.005f);
 if(p.x[0]!=before||p.steps!=steps)return 4;
 DSPhysics uncoupled;ds_physics_init(&uncoupled);uncoupled.coupling=0;
 ds_physics_strike(&uncoupled,1.f);
 for(int i=0;i<100;i++)ds_physics_step(&uncoupled,0.005f);
 if(uncoupled.x[1]!=0.f)return 5;
 DSPhysics coupled;ds_physics_init(&coupled);ds_physics_strike(&coupled,1.f);
 for(int i=0;i<100;i++)ds_physics_step(&coupled,0.005f);
 if(fabsf(coupled.x[1])<0.0001f)return 6;
 puts("PASS: physics deterministic, finite, frozen, coupled and uncoupled");
 return 0;
}
"""
with tempfile.TemporaryDirectory() as tmp:
    c=Path(tmp)/"test.c";c.write_text(src)
    exe=Path(tmp)/"test"
    subprocess.run(["cc","-std=c11","-O2","-Wall","-Wextra","-I",str(root/"src"),str(c),"-lm","-o",str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
