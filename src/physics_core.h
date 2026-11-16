/* DRIFTSPACE experimental two-mass physics controller.
 * Standalone deterministic control-rate core; not connected to audio yet.
 * MIT-compatible original implementation. No allocation or libm required.
 */
#ifndef DRIFTSPACE_PHYSICS_H
#define DRIFTSPACE_PHYSICS_H
typedef struct {
 float x[2], v[2], energy[2];
 float stiffness, coupling, damping, force;
 unsigned steps;
 int frozen;
} DSPhysics;
static float ds_abs(float x){return x<0.f?-x:x;}
static float ds_bound(float x,float limit){return x>limit?limit:x< -limit?-limit:x;}
static void ds_physics_init(DSPhysics *p){
 *p=(DSPhysics){0};p->stiffness=15.f;p->coupling=5.f;p->damping=1.5f;
}
static void ds_physics_strike(DSPhysics *p,float velocity){
 if(!p||p->frozen)return;
 p->v[0]=ds_bound(p->v[0]+ds_bound(velocity,1.f)*3.f,8.f);
}
static void ds_physics_step(DSPhysics *p,float dt){
 if(!p||p->frozen)return;
 if(dt<=0.f||dt>0.01f)return;
 const float k=ds_bound(p->stiffness,80.f);
 const float c=ds_bound(p->coupling,40.f);
 const float d=ds_bound(p->damping,12.f);
 float f0=-k*p->x[0]+c*(p->x[1]-p->x[0])-d*p->v[0]+ds_bound(p->force,10.f);
 float f1=-k*p->x[1]+c*(p->x[0]-p->x[1])-d*p->v[1];
 p->v[0]=ds_bound(p->v[0]+dt*f0,8.f);
 p->v[1]=ds_bound(p->v[1]+dt*f1,8.f);
 for(int i=0;i<2;i++){
  p->x[i]=ds_bound(p->x[i]+dt*p->v[i],2.f);
  p->energy[i]=ds_bound(.5f*p->v[i]*p->v[i]+.5f*k*p->x[i]*p->x[i],100.f);
 }
 p->steps++;
}
#endif
