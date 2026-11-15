/* DRIFTSPACE experimental v0.1 DSP. Independent from Synth_esis.
 * Schwung plugin ABI v2; no libc, allocation, or filesystem in audio path.
 * Initial prototype: 4 timbral worlds + pair morph + gentle evolution.
 * NOT hardware validated. */
#include <stdint.h>
#include <stddef.h>
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
typedef struct {float phase,env,velocity;int note,active,held;} Voice;
typedef struct {
 Voice voices[VOICES]; int world_a,world_b,morph,evolution,chaos,generation,space,tone,level;
 int freeze,mode,seed;float smooth_morph,slow_phase;uint32_t rng;
} Synth;
typedef struct {int used;Synth synth;} Slot;
static Slot slots[INSTANCES];
static int eq(const char*a,const char*b){if(!a||!b)return 0;while(*a&&*a==*b){a++;b++;}return *a==*b;}
static int num(const char*s){int v=0;if(!s)return 0;for(int i=0;i<7&&s[i]>='0'&&s[i]<='9';i++)v=v*10+s[i]-'0';return v;}
static int clamp(int v,int lo,int hi){return v<lo?lo:v>hi?hi:v;}
static float absf(float v){return v<0?-v:v;}
static float wrap(float x){while(x>=1.f)x-=1.f;while(x<0.f)x+=1.f;return x;}
static float sine(float x){/* fast bounded polynomial approximation, input cycles */
 x=wrap(x);float t=x<.5f?x*4.f:4.f*x-4.f;
 return t*(1.27323954f-.405284735f*absf(t));}
static float frequency(int n){/* 12-TET semitone ratio table, A4=440 */
 static const float ratios[12]={1.f,1.0594631f,1.122462f,1.1892071f,1.259921f,1.3348399f,1.41421356f,1.498307f,1.587401f,1.6817928f,1.7817974f,1.8877486f};
 int delta=n-69,oct=0;while(delta<0){delta+=12;oct--;}while(delta>=12){delta-=12;oct++;}
 float hz=440.f*ratios[delta];while(oct<0){hz*=.5f;oct++;}while(oct>0){hz*=2.f;oct--;}return hz;
}
static int put(char*b,int cap,int v){if(!b||cap<2)return -1;char tmp[16];int n=0;if(v<0)v=0;do{tmp[n++]=(char)('0'+v%10);v/=10;}while(v&&n<15);if(n>=cap)return -1;for(int i=0;i<n;i++)b[i]=tmp[n-i-1];b[n]=0;return n;}
static int literal(char*b,int cap,const char*s){int n=0;if(!b||cap<1)return -1;while(s[n]){if(n>=cap-1)return -1;b[n]=s[n];n++;}b[n]=0;return n;}
static void reset(Synth*s){*s=(Synth){0};s->world_b=1;s->evolution=25;s->tone=50;s->level=60;s->seed=1;s->rng=1;}
static void *create(const char*d,const char*j){(void)d;(void)j;for(int i=0;i<INSTANCES;i++){int z=0;if(__atomic_compare_exchange_n(&slots[i].used,&z,1,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE)){reset(&slots[i].synth);return &slots[i].synth;}}return 0;}
static void destroy(void*p){for(int i=0;i<INSTANCES;i++)if(p==&slots[i].synth){__atomic_store_n(&slots[i].used,0,__ATOMIC_RELEASE);return;}}
static void midi(void*p,const uint8_t*m,int n,int source){if(!p||!m||n<3||source==5)return;Synth*s=p;int kind=m[0]&240;
 if(kind==176&&(m[1]==120||m[1]==123)){for(int i=0;i<VOICES;i++){s->voices[i].held=0;s->voices[i].active=0;}return;}
 if((kind!=144&&kind!=128)||m[1]>=128)return;
 if(kind==144&&m[2]){
 int idx=-1;for(int i=0;i<VOICES;i++)if(!s->voices[i].active){idx=i;break;}if(idx<0)idx=0;
 s->voices[idx]=(Voice){0};s->voices[idx].active=1;s->voices[idx].held=1;s->voices[idx].note=m[1];s->voices[idx].velocity=(float)m[2]/127.f;
 }else for(int i=0;i<VOICES;i++)if(s->voices[i].active&&s->voices[i].note==m[1])s->voices[i].held=0;
}
typedef struct{const char*name;int max;size_t offset;} Param;
#define P(n,maxv) {#n,maxv,offsetof(Synth,n)}
static const Param params[]={P(world_a,3),P(world_b,3),P(morph,100),P(evolution,100),P(chaos,100),P(generation,100),P(space,100),P(tone,100),P(level,100),P(freeze,1),P(mode,1),P(seed,999999)};
static void set(void*p,const char*k,const char*v){if(!p||!k||!v)return;Synth*s=p;
 if(eq(k,"panic")){for(int i=0;i<VOICES;i++)s->voices[i].active=0;return;}
 for(unsigned i=0;i<sizeof(params)/sizeof(params[0]);i++)if(eq(k,params[i].name)){int x=clamp(num(v),0,params[i].max);*(int*)((char*)s+params[i].offset)=x;if(eq(k,"seed"))s->rng=(uint32_t)(x?x:1);return;}
}
static int get(void*p,const char*k,char*b,int cap){if(eq(k,"module_id"))return literal(b,cap,"driftspace");if(!p||!k)return -1;Synth*s=p;
 for(unsigned i=0;i<sizeof(params)/sizeof(params[0]);i++)if(eq(k,params[i].name))return put(b,cap,*(int*)((char*)s+params[i].offset));
 if(eq(k,"voice_activity")){int n=0;for(int i=0;i<VOICES;i++)n+=s->voices[i].active;return put(b,cap,n);}
 return -1;
}
static int error(void*p,char*b,int n){(void)p;return literal(b,n,"");}
static float world(int w,float ph,float slow,float chaos,float tone){
 float a=sine(ph),b=sine(ph*2.f),c=sine(ph*3.f);
 float bright=.5f+.5f*tone,distort=.02f*chaos*sine(ph*9.f+slow*.2f);
 if(w==0)return .65f*sine(ph+.05f*slow*b)+bright*.22f*b+.1f*c+distort; /* FLUID */
 if(w==1)return .60f*a+.20f*sine(ph*1.002f+slow*.12f)+bright*.12f*c*(.5f+.5f*slow)+distort; /* ORGANISM */
 if(w==2)return .46f*a+bright*.20f*sine(ph*2.f+slow*.3f)+.13f*sine(ph*5.f+slow*.15f)+distort; /* DRIFTSPACE */
 return .50f*a+bright*.18f*b+.14f*sine(ph*4.f+slow*.22f)+.07f*sine(ph*7.f)+distort; /* BLOOM */
}
static int16_t pcm(float v){if(v>1.f)v=1.f;if(v< -1.f)v=-1.f;return (int16_t)(v*32767.f);}
static void render(void*p,int16_t*out,int frames){if(!out||frames<=0)return;if(frames>128)frames=128;Synth*s=p;
 if(!s){for(int j=0;j<frames*2;j++)out[j]=0;return;}
 for(int j=0;j<frames;j++){
 s->smooth_morph+=.0015f*((float)s->morph*.01f-s->smooth_morph);
 if(!s->freeze)s->slow_phase=wrap(s->slow_phase+(0.03f+.0008f*s->evolution)/SR);
 float slow=sine(s->slow_phase);
 float l=0.f,r=0.f;
 for(int i=0;i<VOICES;i++){Voice*v=&s->voices[i];if(!v->active)continue;
 float hz=frequency(v->note);v->phase=wrap(v->phase+hz/SR);
 float x=world(s->world_a,v->phase,slow,(float)s->chaos*.01f,(float)s->tone*.01f);
 float y=world(s->world_b,v->phase,slow,(float)s->chaos*.01f,(float)s->tone*.01f);
 float mixed=x+(y-x)*s->smooth_morph;
 float evolve=1.f+.06f*slow*((float)s->evolution*.01f);
 float gain=(float)s->level*.01f*.28f*v->velocity;
 float target=v->held?1.f:0.f;v->env+=(target-v->env)*(v->held?.002f:.0007f);
 if(!v->held&&v->env<.0005f){v->active=0;continue;}
 float sample=mixed*evolve*gain*v->env;
 float pan=(float)s->space*.01f*.3f*slow;
 l+=sample*(1.f-pan);r+=sample*(1.f+pan);
 }
 out[j*2]=pcm(l);out[j*2+1]=pcm(r);
 }
}
static plugin_api_v2_t api={2,create,destroy,midi,set,get,error,render};
__attribute__((visibility("default"))) plugin_api_v2_t*move_plugin_init_v2(const void*h){(void)h;return &api;}
