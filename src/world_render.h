/* DRIFTSPACE 2.0 experimental world renderer.
 * Host-only proof of concept, not connected to the Schwung ABI.
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
} DSWorldRender;
static float ds_wr_sine(float x){
 /* Periodic odd polynomial approximation, bounded for normalized phase. */
 while(x>3.14159265f)x-=6.2831853f;
 while(x< -3.14159265f)x+=6.2831853f;
 float x2=x*x;
 return x*(1.f-x2/6.f+x2*x2/120.f-x2*x2*x2/5040.f);
}
static void ds_wr_init(DSWorldRender *s,float frequency,int a,int b){
 *s=(DSWorldRender){0};
 ds_we_init(&s->exchange);ds_ws_init(&s->spectrum);
 s->frequency=ds_we_clip(frequency,20.f,1000.f);
 s->world[0]=a;s->world[1]=b;s->level=.15f;
}
static void ds_wr_note(DSWorldRender *s,float velocity){ds_we_strike(&s->exchange,velocity);}
static float ds_wr_sample(DSWorldRender *s,float sample_rate,float evolution){
 if(sample_rate<8000.f)return 0.f;
 if(++s->control_tick>=220){
  s->control_tick=0;
  ds_we_step(&s->exchange,220.f/sample_rate);
  ds_ws_update(&s->spectrum,&s->exchange,s->world[0],s->world[1],evolution);
 }
 float output=0.f;
 for(int w=0;w<2;w++){
  float world_mix=w==0?1.f-s->exchange.morph:s->exchange.morph;
  /* Coupling lets a world resonate even when it was not directly struck. */
  float energy=ds_we_clip(s->exchange.energy[w]*1.5f,0.f,1.f);
  float activity=ds_we_clip(.12f+energy,0.f,1.f);
  for(int i=0;i<DS_WORLD_PARTIALS;i++){
   float freq=s->frequency*s->spectrum.ratio[w][i];
   float delta=6.2831853f*freq/sample_rate;
   s->phase[w][i]+=delta;
   if(s->phase[w][i]>=6.2831853f)s->phase[w][i]-=6.2831853f;
   float target=s->spectrum.gain[w][i]*activity;
   s->amp[w][i]+=.0006f*(target-s->amp[w][i]);
   output+=world_mix*s->amp[w][i]*ds_wr_sine(s->phase[w][i]);
  }
 }
 return ds_we_clip(output*s->level,-1.f,1.f);
}
#endif
