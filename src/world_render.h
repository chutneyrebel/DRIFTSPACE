/* DRIFTSPACE 2.0 experimental world renderer.
 * Used by the Schwung ABI's WORLD 2.0 mode.
 * Eight modal oscillators per world, phase continuous and gain smoothed.
 */
#ifndef DS_WORLD_RENDER_H
#define DS_WORLD_RENDER_H
#include "world_spectrum.h"
typedef struct {
 DSWorldSpectrum spectrum;
 DSWorldExchange exchange;
 float phase[2][DS_WORLD_PARTIALS];
 float amp[2][DS_WORLD_PARTIALS];
 float frequency,level;
 int world[2];
 unsigned control_tick;
 float frozen_ratio[2][DS_WORLD_PARTIALS];
 float frozen_gain[2][DS_WORLD_PARTIALS];
 float freeze_blend;
 int freeze_enabled;
} DSWorldRender;
static float ds_wr_sine(float x){
 /* Continuous, bounded rational sine approximation, zero at phase wrap.
    Unlike a truncated Taylor series, it does not jump at +/-pi. */
 const float pi=3.14159265359f, tau=6.28318530718f;
 while(x>=tau)x-=tau;
 while(x<0.f)x+=tau;
 float sign=1.f;
 if(x>pi){x-=pi;sign=-1.f;}
 float y=x*(pi-x);
 return sign*(16.f*y)/(5.f*pi*pi-4.f*y);
}
static void ds_wr_init(DSWorldRender *s,float frequency,int a,int b){
 *s=(DSWorldRender){0};
 ds_we_init(&s->exchange);ds_ws_init(&s->spectrum);
 ds_ws_update(&s->spectrum,&s->exchange,a,b,0.f);
 s->frequency=ds_we_clip(frequency,20.f,1000.f);
 s->world[0]=a;s->world[1]=b;s->level=.15f;
}
static void ds_wr_note(DSWorldRender *s,float velocity){ds_we_strike(&s->exchange,velocity);}
static void ds_wr_freeze(DSWorldRender *s,int enabled){
 if(enabled&&!s->freeze_enabled){
  for(int w=0;w<2;w++)for(int i=0;i<DS_WORLD_PARTIALS;i++){
   s->frozen_ratio[w][i]=s->spectrum.ratio[w][i];
   s->frozen_gain[w][i]=s->amp[w][i];
  }
 }
 s->freeze_enabled=enabled?1:0;
}
static float ds_wr_sample(DSWorldRender *s,float sample_rate,float evolution){
 if(sample_rate<8000.f)return 0.f;
 if(++s->control_tick>=220){
  s->control_tick=0;
  ds_we_step(&s->exchange,220.f/sample_rate);
  ds_ws_update(&s->spectrum,&s->exchange,s->world[0],s->world[1],evolution);
 }
 s->freeze_blend+=.0005f*((s->freeze_enabled?1.f:0.f)-s->freeze_blend);
 float output=0.f;
 for(int w=0;w<2;w++){
  float world_mix=w==0?1.f-s->exchange.morph:s->exchange.morph;
  /* Coupling lets a world resonate even when it was not directly struck. */
  float energy=ds_we_clip(s->exchange.energy[w]*1.5f,0.f,1.f);
  float activity=ds_we_clip(.12f+energy,0.f,1.f);
  for(int i=0;i<DS_WORLD_PARTIALS;i++){
   float ratio=s->spectrum.ratio[w][i]*(1.f-s->freeze_blend)+s->frozen_ratio[w][i]*s->freeze_blend;
   float freq=s->frequency*ratio;
   /* Do not synthesize partials above Nyquist. */
   if(freq>=sample_rate*.48f)continue;
   float delta=6.2831853f*freq/sample_rate;
   s->phase[w][i]+=delta;
   if(s->phase[w][i]>=6.2831853f)s->phase[w][i]-=6.2831853f;
   float target=s->spectrum.gain[w][i]*activity;
   target=target*(1.f-s->freeze_blend)+s->frozen_gain[w][i]*s->freeze_blend;
   s->amp[w][i]+=.0006f*(target-s->amp[w][i]);
   output+=world_mix*s->amp[w][i]*ds_wr_sine(s->phase[w][i]);
  }
 }
 return ds_we_clip(output*s->level,-1.f,1.f);
}
#endif
