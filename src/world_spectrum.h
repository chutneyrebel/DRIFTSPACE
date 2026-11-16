/* DRIFTSPACE 2.0 phase 2: distinct, evolving modal world identities.
 * Control-rate spectral design; no audio path integration yet.
 */
#ifndef DS_WORLD_SPECTRUM_H
#define DS_WORLD_SPECTRUM_H
#include "world_exchange.h"
#define DS_WORLD_PARTIALS 8
typedef struct {
 float ratio[2][DS_WORLD_PARTIALS];
 float gain[2][DS_WORLD_PARTIALS];
 float phase[2];
 float growth[2];
} DSWorldSpectrum;
static float ds_ws_abs(float x){return x<0.f?-x:x;}
static void ds_ws_init(DSWorldSpectrum *s){*s=(DSWorldSpectrum){0};}
static void ds_ws_update(DSWorldSpectrum *s,const DSWorldExchange *p,int world_a,int world_b,float evolution){
 static const float ratios[4][DS_WORLD_PARTIALS]={
  {1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f}, /* FLUID */
  {1.f,1.5f,2.21f,3.17f,4.33f,5.58f,6.91f,8.2f}, /* ORGANISM */
  {1.f,1.414f,2.718f,3.732f,5.196f,6.283f,8.12f,9.71f}, /* DRIFTSPACE */
  {1.f,2.f,3.f,5.f,8.f,11.f,15.f,20.f} /* BLOOM */
 };
 int worlds[2]={world_a,world_b};
 float evolve=ds_we_clip(evolution,0.f,1.f);
 for(int w=0;w<2;w++){
  int kind=worlds[w]<0?0:worlds[w]>3?3:worlds[w];
  float motion=ds_ws_abs(p->x[w])*.15f+ds_ws_abs(p->v[w])*.09f;
  float e=ds_we_clip(p->energy[w]*.12f,0.f,1.f);
  s->phase[w]+=.003f+evolve*.012f+motion*.001f;
  if(s->phase[w]>1.f)s->phase[w]-=1.f;
  s->growth[w]+=.025f*(e-s->growth[w]);
  for(int i=0;i<DS_WORLD_PARTIALS;i++){
   float index=(float)i/7.f;
   float flutter=(s->phase[w]-.5f)*evolve;
   float spectral=1.f;
   if(kind==0)spectral=1.f+.035f*flutter*(1.f+index);
   if(kind==1)spectral=1.f+.13f*flutter*(.3f+index)+.03f*motion;
   if(kind==2)spectral=1.f+.24f*flutter*(.4f+index)+.06f*p->x[1-w];
   if(kind==3)spectral=1.f+.06f*flutter*(1.f+index);
   s->ratio[w][i]=ratios[kind][i]*ds_we_clip(spectral,.65f,1.4f);
   float base=1.f/(1.f+.55f*(float)i);
   if(kind==0)base*=1.f-.45f*index;
   if(kind==1)base*=.6f+.4f*ds_ws_abs(p->v[w]);
   if(kind==2)base*=.65f+.45f*index;
   if(kind==3)base*=.3f+.9f*ds_we_clip(s->growth[w]*2.f-index*.6f,0.f,1.f);
   s->gain[w][i]=ds_we_clip(base,0.f,1.f);
  }
 }
}
#endif
