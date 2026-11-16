/* DRIFTSPACE experimental v0.1 DSP. Independent from Synth_esis.
 * Schwung plugin ABI v2; no libc, allocation, or filesystem in audio path.
 * Initial prototype: 4 timbral worlds + pair morph + gentle evolution.
 * NOT hardware validated. */
#include <stdint.h>
#include <stddef.h>
#include "coupled_core.h"
#define SR 44100.f
#define TAU 6.28318530718f
#define VOICES 4
#define INSTANCES 4
typedef struct {
 uint32_t api_version;
 void *(*create_instance)(const char *,const char *);
 void (*destroy_instance)(void *);
 void (*on_midi)(void *,const uint8_t *,int,int);
 void (*set_param)(void *,const char *,const char *);
 int (*get_param)(void *,const char *,char *,int);
 int (*get_error)(void *,char *,int);
 void (*render_block)(void *,int16_t *,int);
} plugin_api_v2_t;
typedef struct {float phase,env,velocity,generated_phase,generated_gain;int note,active,held;DSCoupled physical;} Voice;
typedef struct {
 Voice voices[VOICES]; int world_a,world_b,morph,evolution,chaos,generation,space,tone,level;
 int freeze,mode,seed;float smooth_morph,slow_phase,slow_secondary,organic,random_target,random_current;uint32_t rng;unsigned tick,gen_tick;int gen_interval;float gen_gate;
} Synth;
typedef struct {int used;Synth synth;} Slot;
static Slot slots[INSTANCES];
static int eq(const char*a,const char*b){if(!a||!b)return 0;while(*a&&*a==*b){a++;b++;}return *a==*b;}
static int num(const char*s){int v=0;if(!s)return 0;for(int i=0;i<7&&s[i]>='0'&&s[i]<='9';i++)v=v*10+s[i]-'0';return v;}
static int clamp(int v,int lo,int hi){return v<lo?lo:v>hi?hi:v;}
static float absf(float v){return v<0?-v:v;}
static float wrap(float x){while(x>=1.f)x-=1.f;while(x<0.f)x+=1.f;return x;}
static float sine(float x){/* fast bounded polynomial approximation, input cycles */
 x=wrap(x);float t=x<.5f?x*2.f:2.f*x-2.f;
 return 4.f*t*(1.f-absf(t));}
static float frequency(int n){/* 12-TET semitone ratio table, A4=440 */
 static const float ratios[12]={1.f,1.0594631f,1.122462f,1.1892071f,1.259921f,1.3348399f,1.41421356f,1.498307f,1.587401f,1.6817928f,1.7817974f,1.8877486f};
 int delta=n-69,oct=0;while(delta<0){delta+=12;oct--;}while(delta>=12){delta-=12;oct++;}
 float hz=440.f*ratios[delta];while(oct<0){hz*=.5f;oct++;}while(oct>0){hz*=2.f;oct--;}return hz;
}
static int put(char*b,int cap,int v){if(!b||cap<2)return -1;char tmp[16];int n=0;if(v<0)v=0;do{tmp[n++]=(char)('0'+v%10);v/=10;}while(v&&n<15);if(n>=cap)return -1;for(int i=0;i<n;i++)b[i]=tmp[n-i-1];b[n]=0;return n;}
static int literal(char*b,int cap,const char*s){int n=0;if(!b||cap<1)return -1;while(s[n]){if(n>=cap-1)return -1;b[n]=s[n];n++;}b[n]=0;return n;}
static void reset(Synth*s){*s=(Synth){0};s->world_b=1;s->morph=50;s->smooth_morph=.5f;s->evolution=25;s->tone=50;s->level=60;s->seed=1;s->rng=1;}
static void *create(const char*d,const char*j){(void)d;(void)j;for(int i=0;i<INSTANCES;i++){int z=0;if(__atomic_compare_exchange_n(&slots[i].used,&z,1,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE)){reset(&slots[i].synth);return &slots[i].synth;}}return 0;}
static void destroy(void*p){for(int i=0;i<INSTANCES;i++)if(p==&slots[i].synth){__atomic_store_n(&slots[i].used,0,__ATOMIC_RELEASE);return;}}
static float frequency(int n);
static void midi(void*p,const uint8_t*m,int n,int source){if(!p||!m||n<3||source==5)return;Synth*s=p;int kind=m[0]&240;
 if(kind==176&&(m[1]==120||m[1]==123)){for(int i=0;i<VOICES;i++){s->voices[i].held=0;s->voices[i].active=0;}return;}
 if((kind!=144&&kind!=128)||m[1]>=128)return;
 if(kind==144&&m[2]){
 int idx=-1;for(int i=0;i<VOICES;i++)if(!s->voices[i].active){idx=i;break;}if(idx<0)idx=0;
 s->voices[idx]=(Voice){0};s->voices[idx].active=1;s->voices[idx].held=1;s->voices[idx].note=m[1];s->voices[idx].velocity=(float)m[2]/127.f;
 ds_coupled_init(&s->voices[idx].physical,frequency(s->voices[idx].note));
 ds_coupled_strike(&s->voices[idx].physical,s->voices[idx].velocity);
 }else for(int i=0;i<VOICES;i++)if(s->voices[i].active&&s->voices[i].note==m[1])s->voices[i].held=0;
}
typedef struct{const char*name;int max;size_t offset;} Param;
#define P(n,maxv) {#n,maxv,offsetof(Synth,n)}
static const Param params[]={P(world_a,3),P(world_b,3),P(morph,100),P(evolution,100),P(chaos,100),P(generation,100),P(space,100),P(tone,100),P(level,100),P(freeze,1),P(mode,2),P(seed,999999)};
static void set(void*p,const char*k,const char*v){if(!p||!k||!v)return;Synth*s=p;
 if(eq(k,"panic")){for(int i=0;i<VOICES;i++)s->voices[i].active=0;return;}
 for(unsigned i=0;i<sizeof(params)/sizeof(params[0]);i++)if(eq(k,params[i].name)){int x=clamp(num(v),0,params[i].max);*(int*)((char*)s+params[i].offset)=x;if(eq(k,"seed"))s->rng=(uint32_t)(x?x:1);
 if(eq(k,"mode")&&x==2){for(int j=0;j<VOICES;j++)if(s->voices[j].active){ds_coupled_init(&s->voices[j].physical,frequency(s->voices[j].note));ds_coupled_strike(&s->voices[j].physical,s->voices[j].velocity);}}
 return;}
}
static const char chain_params_json[]="[{\"key\":\"world_a\",\"type\":\"enum\",\"options\":[\"FLUID\",\"ORGANISM\",\"DRIFTSPACE\",\"BLOOM\"],\"default\":0},{\"key\":\"world_b\",\"type\":\"enum\",\"options\":[\"FLUID\",\"ORGANISM\",\"DRIFTSPACE\",\"BLOOM\"],\"default\":1},{\"key\":\"mode\",\"type\":\"enum\",\"options\":[\"FREE PAIR\",\"JOURNEY\",\"PHYSICS\"],\"default\":0},{\"key\":\"morph\",\"type\":\"int\",\"min\":0,\"max\":100,\"default\":50},{\"key\":\"evolution\",\"type\":\"int\",\"min\":0,\"max\":100,\"default\":25},{\"key\":\"chaos\",\"type\":\"int\",\"min\":0,\"max\":100,\"default\":0},{\"key\":\"generation\",\"type\":\"int\",\"min\":0,\"max\":100,\"default\":0},{\"key\":\"space\",\"type\":\"int\",\"min\":0,\"max\":100,\"default\":0},{\"key\":\"tone\",\"type\":\"int\",\"min\":0,\"max\":100,\"default\":50},{\"key\":\"level\",\"type\":\"int\",\"min\":0,\"max\":100,\"default\":60},{\"key\":\"freeze\",\"type\":\"enum\",\"options\":[\"Off\",\"On\"],\"default\":0},{\"key\":\"face\",\"type\":\"canvas\",\"canvas_script\":\"canvas.js\",\"canvas_overlay\":\"driftspace_main\",\"as_page\":true,\"page_first\":true,\"show_value\":false,\"extra_keys\":[\"voice_activity\"]},{\"key\":\"voice_activity\",\"type\":\"int\",\"min\":0,\"max\":4,\"access\":\"read\"}]";
static const char ui_hierarchy_json[]="{\"levels\":{\"root\":{\"knobs\":[\"world_a\",\"world_b\",\"mode\",\"morph\",\"evolution\",\"chaos\",\"generation\",\"level\"],\"params\":[{\"key\":\"face\",\"type\":\"canvas\",\"canvas_script\":\"canvas.js\",\"canvas_overlay\":\"driftspace_main\",\"as_page\":true,\"page_first\":true,\"extra_keys\":[\"voice_activity\"]},{\"level\":\"sculpt\"},{\"level\":\"motion\"}]},\"sculpt\":{\"knobs\":[\"world_a\",\"world_b\",\"morph\",\"tone\",\"space\",\"level\",\"chaos\",\"generation\"],\"params\":[\"world_a\",\"world_b\",\"morph\",\"tone\",\"space\",\"level\",\"chaos\",\"generation\"]},\"motion\":{\"knobs\":[\"evolution\",\"chaos\",\"generation\",\"freeze\",\"space\",\"tone\",\"morph\",\"level\"],\"params\":[\"evolution\",\"chaos\",\"generation\",\"freeze\",\"space\",\"tone\",\"morph\",\"level\"]}}}";
static int get(void*p,const char*k,char*b,int cap){
 if(eq(k,"chain_params"))return literal(b,cap,chain_params_json);
 if(eq(k,"ui_hierarchy"))return literal(b,cap,ui_hierarchy_json);
 if(eq(k,"module_id"))return literal(b,cap,"driftspace");if(!p||!k)return -1;Synth*s=p;
 for(unsigned i=0;i<sizeof(params)/sizeof(params[0]);i++)if(eq(k,params[i].name))return put(b,cap,*(int*)((char*)s+params[i].offset));
 if(eq(k,"voice_activity")){int n=0;for(int i=0;i<VOICES;i++)n+=s->voices[i].active;return put(b,cap,n);}
 return -1;
}
static int error(void*p,char*b,int n){(void)p;return literal(b,n,"");}
/* Phase 4: four lightweight but genuinely different synthesis behaviors.
 * Deterministic stochastic modulation advances at control rate, not per sample. */
static float world(int w,float ph,float slow,float slow2,float chaos,float tone,float organic){
 float bright=.35f+.65f*tone;
 if(w==0){ /* FLUID: phase-bent harmonic flow */
  float bend=.045f*(1.f+chaos)*slow2;
  return .60f*sine(ph+bend*sine(ph*2.f))+
         .24f*bright*sine(ph*2.f+.08f*slow)+.10f*sine(ph*3.f+.12f*slow2);
 }
 if(w==1){ /* ORGANISM: stochastic timbre with coupled modulators */
  float living=.45f*organic+.35f*slow2+.20f*slow;
  return .56f*sine(ph+.018f*living)+
         (.12f+.19f*bright)*sine(ph*3.f+.1f*living)+
         (.06f+.10f*chaos)*sine(ph*7.f+.2f*organic);
 }
 if(w==2){ /* DRIFTSPACE: inharmonic drifting partials */
  return .45f*sine(ph)+
         .23f*bright*sine(ph*2.003f+.13f*slow)+
         .16f*sine(ph*4.009f+.20f*slow2)+
         .08f*chaos*sine(ph*6.017f+.1f*organic);
 }
 /* BLOOM: branching harmonics, movement biased toward upper partials */
 float branch=.5f+.5f*slow2;
 return .43f*sine(ph)+
        .20f*bright*sine(ph*2.f)+
        (.07f+.15f*branch)*sine(ph*4.f+.08f*slow)+
        (.04f+.10f*chaos)*sine(ph*7.f+.10f*organic);
}
static int16_t pcm(float v){if(v>1.f)v=1.f;if(v< -1.f)v=-1.f;return (int16_t)(v*32767.f);}
static void render(void*p,int16_t*out,int frames){if(!out||frames<=0)return;if(frames>128)frames=128;Synth*s=p;
 if(!s){for(int j=0;j<frames*2;j++)out[j]=0;return;}
 if(s->mode==2){for(int i=0;i<VOICES;i++)if(s->voices[i].active)ds_coupled_controls(&s->voices[i].physical,(float)s->evolution*.01f,(float)s->chaos*.01f,(float)s->tone*.01f,(float)s->generation*.01f);}
 for(int j=0;j<frames;j++){
 s->smooth_morph+=.004f*((float)s->morph*.01f-s->smooth_morph);
 if(!s->freeze){s->slow_phase=wrap(s->slow_phase+(0.03f+.0008f*s->evolution)/SR);s->slow_secondary=wrap(s->slow_secondary+(.013f+.0002f*s->evolution)/SR);}
 /* Advance bounded pseudo-random target every 1024 samples.
  * Freeze holds both periodic and stochastic evolution. */
 if(!s->freeze && (++s->tick>=1024)){
  s->tick=0;
  s->rng^=s->rng<<13;s->rng^=s->rng>>17;s->rng^=s->rng<<5;
  s->random_target=((float)(s->rng&65535u)/32767.5f)-1.f;
 }
 if(!s->freeze)s->random_current+=.00012f*(s->random_target-s->random_current);
 /* Note-aware harmonic satellites: a quiet, gated secondary oscillator
  * per held voice. Root notes remain unchanged; no emitted MIDI events.
  * Frozen state holds pattern and gate; phase continues for uninterrupted audio. */
 if(!s->freeze && s->generation>0 && ++s->gen_tick >= (unsigned)(22050-150*s->evolution)){
  static const int musical_intervals[7]={0,7,12,4,9,16,19};
  s->gen_tick=0;
  s->rng^=s->rng<<13;s->rng^=s->rng>>17;s->rng^=s->rng<<5;
  int spread=1+s->chaos*6/100;
  s->gen_interval=musical_intervals[(s->rng>>8)%(unsigned)spread];
  s->gen_gate=((s->rng>>17)%100u)<(unsigned)s->generation?1.f:0.f;
 }
 float slow=sine(s->slow_phase),slow2=sine(s->slow_secondary);
 float organic=s->random_current*((float)s->chaos*.01f)*(.25f+.75f*(float)s->evolution*.01f);
 float journey=s->smooth_morph*3.f;
 int ja=(int)journey;if(ja>2)ja=2;
 float jm=journey-(float)ja;if(jm>1.f)jm=1.f;
 int wa=s->mode?ja:s->world_a,wb=s->mode?ja+1:s->world_b;
 float wm=s->mode?jm:s->smooth_morph;
 float l=0.f,r=0.f;
 for(int i=0;i<VOICES;i++){Voice*v=&s->voices[i];if(!v->active)continue;
 if(s->mode==2){
  float target=v->held?1.f:0.f;
  v->env+=(target-v->env)*(v->held?.002f:.0007f);
  if(!v->held&&v->env<.0005f){v->active=0;continue;}
  v->physical.physics.frozen=s->freeze;
  float physical=ds_coupled_tick(&v->physical);
  float gain=(float)s->level*.01f*.7f*v->env;
  l+=physical*gain;r+=physical*gain;
  continue;
 }
 float hz=frequency(v->note);v->phase=wrap(v->phase+hz/SR);
 float x=world(wa,v->phase,slow,slow2,(float)s->chaos*.01f,(float)s->tone*.01f,organic);
 float y=world(wb,v->phase,slow2,slow,(float)s->chaos*.01f,(float)s->tone*.01f,organic);
 /* Smoothstep crossfade reduces sudden coloration around the midpoint. */
 float blend=wm*wm*(3.f-2.f*wm);
 float mixed=x*(1.f-blend)+y*blend;
 float evolve=1.f+.06f*slow*((float)s->evolution*.01f);
 float gain=(float)s->level*.01f*.28f*v->velocity;
 float target=v->held?1.f:0.f;v->env+=(target-v->env)*(v->held?.002f:.0007f);
 if(!v->held&&v->env<.0005f){v->active=0;continue;}
 float sample=mixed*evolve*gain*v->env;
 if(s->generation>0 && v->held){
  float target_gen=s->gen_gate*((float)s->generation*.01f)*.85f;
  v->generated_gain+=(target_gen-v->generated_gain)*.00035f;
  v->generated_phase=wrap(v->generated_phase+frequency(clamp(v->note+s->gen_interval,0,127))/SR);
  sample+=sine(v->generated_phase)*v->generated_gain*gain*v->env;
 }else v->generated_gain*=.99965f;
 /* SPACE controls stereo width directly and adds slow opposing movement. */
 float width=(float)s->space*.01f;
 float pan=width*(.50f+.35f*slow2);
 l+=sample*(1.f-pan);r+=sample*(1.f+pan);
 }
 out[j*2]=pcm(l);out[j*2+1]=pcm(r);
 }
}
static plugin_api_v2_t api={2,create,destroy,midi,set,get,error,render};
__attribute__((visibility("default"))) plugin_api_v2_t*move_plugin_init_v2(const void*h){(void)h;return &api;}
