/* DRIFTSPACE 2.0 bidirectional physical world exchange, phase 1.
 * Host-testable, deterministic, no allocation or libm.
 * Coupling is applied as equal and opposite forces, so it cannot inject
 * net momentum into the two-world system.
 */
#ifndef DS_WORLD_EXCHANGE_H
#define DS_WORLD_EXCHANGE_H
typedef struct {
 float x[2],v[2],energy[2];
 float morph,coupling,chaos;
 unsigned ticks;
} DSWorldExchange;
static float ds_we_clip(float x,float lo,float hi){return x<lo?lo:x>hi?hi:x;}
static void ds_we_init(DSWorldExchange *s){
 *s=(DSWorldExchange){0};
 s->morph=.5f;s->coupling=.5f;
}
static void ds_we_controls(DSWorldExchange *s,float morph,float coupling,float chaos){
 s->morph=ds_we_clip(morph,0.f,1.f);
 s->coupling=ds_we_clip(coupling,0.f,1.f);
 s->chaos=ds_we_clip(chaos,0.f,1.f);
}
static void ds_we_strike(DSWorldExchange *s,float velocity){
 /* Morph selects which world receives the initial excitation. */
 s->v[0]+=ds_we_clip(velocity,0.f,1.f)*(1.f-s->morph);
 s->v[1]+=ds_we_clip(velocity,0.f,1.f)*s->morph;
}
static void ds_we_step(DSWorldExchange *s,float dt){
 if(dt<=0.f||dt>.01f)return;
 /* Reciprocal spring and damper transfer, with conservative chaos-induced
    variation of coupling stiffness (no random source at chaos=0). */
 float delta=s->x[1]-s->x[0];
 float relative=s->v[1]-s->v[0];
 float irregular=1.f+s->chaos*.35f*ds_we_clip(delta*relative,-1.f,1.f);
 float force=s->coupling*16.f*irregular*delta+s->coupling*.7f*relative;
 float a0=-9.f*s->x[0]-.25f*s->v[0]+force;
 float a1=-9.f*s->x[1]-.25f*s->v[1]-force;
 s->v[0]=ds_we_clip(s->v[0]+dt*a0,-4.f,4.f);
 s->v[1]=ds_we_clip(s->v[1]+dt*a1,-4.f,4.f);
 for(int i=0;i<2;i++){
  s->x[i]=ds_we_clip(s->x[i]+dt*s->v[i],-2.f,2.f);
  s->energy[i]=.5f*s->v[i]*s->v[i]+4.5f*s->x[i]*s->x[i];
 }
 s->ticks++;
}
#endif
