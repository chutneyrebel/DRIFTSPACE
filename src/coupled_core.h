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
 float accumulator;
 float pickup[2];
} DSCoupled;
static void ds_coupled_init(DSCoupled *s,float hz){
 *s=(DSCoupled){0};
 ds_physics_init(&s->physics);
 ds_modal_init(&s->resonator[0],hz,0);
 ds_modal_init(&s->resonator[1],hz*1.17f,1);
}
static void ds_coupled_strike(DSCoupled *s,float velocity){
 ds_physics_strike(&s->physics,velocity);
 ds_modal_strike(&s->resonator[0],velocity*.6f);
}
static float ds_coupled_tick(DSCoupled *s){
 /* 44.1 kHz audio, 200 Hz physics; carry fractional samples precisely. */
 s->accumulator+=200.f/44100.f;
 if(s->accumulator>=1.f){
  s->accumulator-=1.f;
  float old0=s->physics.v[0],old1=s->physics.v[1];
  ds_physics_step(&s->physics,.005f);
  if(!s->physics.frozen){
   float delta0=ds_modal_abs(s->physics.v[0]-old0);
   float delta1=ds_modal_abs(s->physics.v[1]-old1);
   /* Coupled physical acceleration excites resonators, never direct
      audio feedback; limiting prevents self-sustaining overload. */
   float drive0=ds_modal_clip(delta0*.015f,0.04f);
   float drive1=ds_modal_clip(delta1*.025f,0.04f);
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
