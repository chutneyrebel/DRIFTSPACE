/* DRIFTSPACE coupled physics/modal proof of concept.
 * Standalone host-renderable core; not yet wired to Schwung ABI.
 */
#ifndef DRIFTSPACE_COUPLED_H
#define DRIFTSPACE_COUPLED_H
#include "physics_core.h"
#include "modal_core.h"
typedef struct {
 DSPhysics physics;
 DSModal resonator[2];
 float last_velocity[2];
 unsigned accumulator;
 float pickup[2];
 float tension, coupling, damping, energy;
 float base_hz;
 float tuned_hz;
 float tuned_coupling;
} DSCoupled;
static void ds_coupled_init(DSCoupled *s,float hz){
 *s=(DSCoupled){0};
 ds_physics_init(&s->physics);
 s->base_hz=hz;
 s->tuned_hz=hz;
 s->tuned_coupling=.5f;
 s->tension=.5f;s->coupling=.5f;s->damping=.5f;s->energy=.5f;
 ds_modal_init(&s->resonator[0],hz,0);
 ds_modal_init(&s->resonator[1],hz*1.17f,1);
 s->physics.stiffness=37.5f;s->physics.coupling=12.5f;s->physics.damping=3.65f;
}
static void ds_coupled_controls(DSCoupled *s,float tension,float coupling,float damping,float energy){
 s->tension=ds_modal_clip(tension,1.f);
 s->coupling=ds_modal_clip(coupling,1.f);
 s->damping=ds_modal_clip(damping,1.f);
 s->energy=ds_modal_clip(energy,1.f);
 if(s->tension<0.f)s->tension=0.f;
 if(s->coupling<0.f)s->coupling=0.f;
 if(s->damping<0.f)s->damping=0.f;
 if(s->energy<0.f)s->energy=0.f;
 s->physics.stiffness=5.f+65.f*s->tension;
 s->physics.coupling=25.f*s->coupling;
 s->physics.damping=.15f+7.f*s->damping;
 /* Update modal coefficients at control rate while retaining resonator
    histories, so changing TENSION or COUPLING does not erase the tail. */
 float target=s->base_hz*(.75f+.5f*s->tension);
 if(ds_modal_abs(target-s->tuned_hz)>.01f ||
    ds_modal_abs(s->coupling-s->tuned_coupling)>.0001f){
  DSModal a,b;
  ds_modal_init(&a,target,0);
  ds_modal_init(&b,target*(1.05f+.24f*s->coupling),1);
  for(int i=0;i<DS_MODAL_MODES;i++){
   s->resonator[0].coeff[i]=a.coeff[i];
   s->resonator[1].coeff[i]=b.coeff[i];
  }
  s->tuned_hz=target;
  s->tuned_coupling=s->coupling;
 }
}
static void ds_coupled_strike(DSCoupled *s,float velocity){
 float impulse=velocity*(.15f+.85f*s->energy);
 ds_physics_strike(&s->physics,impulse);
 ds_modal_strike(&s->resonator[0],impulse*.6f);
}
static float ds_coupled_tick(DSCoupled *s){
 /* 44.1 kHz audio, 200 Hz physics; carry fractional samples precisely. */
 s->accumulator+=200u;
 if(s->accumulator>=44100u){
  s->accumulator-=44100u;
  float old0=s->physics.v[0],old1=s->physics.v[1];
  ds_physics_step(&s->physics,.005f);
  if(!s->physics.frozen){
   float delta0=ds_modal_abs(s->physics.v[0]-old0);
   float delta1=ds_modal_abs(s->physics.v[1]-old1);
   /* Coupled physical acceleration excites resonators, never direct
      audio feedback; limiting prevents self-sustaining overload. */
   float drive0=ds_modal_clip(delta0*.015f*(.25f+s->energy),0.04f);
   float drive1=ds_modal_clip(delta1*.025f*(.25f+s->energy),0.04f);
   if(drive0>.0001f)ds_modal_strike(&s->resonator[0],drive0);
   if(drive1>.0001f)ds_modal_strike(&s->resonator[1],drive1);
  }
 }
 float a=ds_modal_tick(&s->resonator[0]);
 float b=ds_modal_tick(&s->resonator[1]);
 s->pickup[0]=a;s->pickup[1]=b;
 return ds_modal_clip((a+b)*.5f,.8f);
}
#endif
