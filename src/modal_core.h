/* DRIFTSPACE experimental modal resonators, original implementation.
 * Standalone DSP prototype: no Schwung parameter/UI integration yet.
 * Fixed 44.1kHz reference rate, 8 modes, no allocation or libm calls.
 */
#ifndef DRIFTSPACE_MODAL_H
#define DRIFTSPACE_MODAL_H
#define DS_MODAL_MODES 8
typedef struct {
 float y1[DS_MODAL_MODES],y2[DS_MODAL_MODES];
 float coeff[DS_MODAL_MODES],decay[DS_MODAL_MODES],weight[DS_MODAL_MODES];
 float excitation;
} DSModal;
static float ds_modal_abs(float x){return x<0.f?-x:x;}
static float ds_modal_clip(float x,float limit){return x>limit?limit:x< -limit?-limit:x;}
/* Polynomial cosine approximation for normalized frequency 0..Nyquist.
 * cos(pi*x) approx with bounded Taylor polynomial; used at setup, not sample rate. */
static float ds_modal_cos(float x){
 const float pi=3.14159265358979323846f;
 float t=pi*x,t2=t*t;
 return 1.f-t2*.5f+t2*t2*(1.f/24.f)-t2*t2*t2*(1.f/720.f)+t2*t2*t2*t2*(1.f/40320.f);
}
static void ds_modal_init(DSModal *r,float hz,int material){
 static const float beam[DS_MODAL_MODES]={1.f,2.76f,5.4f,8.93f,13.34f,18.65f,24.86f,31.97f};
 static const float plate[DS_MODAL_MODES]={1.f,1.59f,2.14f,2.3f,2.92f,3.55f,4.13f,4.81f};
 *r=(DSModal){0};
 if(hz<25.f)hz=25.f;if(hz>1500.f)hz=1500.f;
 for(int i=0;i<DS_MODAL_MODES;i++){
  float ratio=material?plate[i]:beam[i];
  float f=hz*ratio;
  if(f>18000.f)f=18000.f;
  /* normalized angular frequency: 2*pi*f/SR, cosine argument / pi */
  float cosw=ds_modal_cos(2.f*f/44100.f);
  float radius=0.9993f-(float)i*.00014f;
  r->coeff[i]=2.f*radius*cosw;
  r->decay[i]=radius*radius;
  r->weight[i]=(material?0.35f:0.5f)/(1.f+(float)i*.7f);
 }
}
static void ds_modal_strike(DSModal *r,float strength){
 r->excitation=ds_modal_clip(r->excitation+strength,1.f);
}
static float ds_modal_tick(DSModal *r){
 float sum=0.f;
 float excite=r->excitation;r->excitation=0.f;
 for(int i=0;i<DS_MODAL_MODES;i++){
  float next=r->coeff[i]*r->y1[i]-r->decay[i]*r->y2[i]+excite*r->weight[i];
  next=ds_modal_clip(next,8.f);
  r->y2[i]=r->y1[i];r->y1[i]=next;
  sum+=next*r->weight[i];
 }
 /* Bound feedback and output to prevent extreme-mode overload. */
 return ds_modal_clip(sum*.09f,.8f);
}
#endif
