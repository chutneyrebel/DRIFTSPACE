/* DRIFTSPACE Phase 2: four monochrome world personalities.
 * Schwung native canvas overlay. Pure graphics, no DSP audio access. */
(function(){
"use strict";
const clamp=(x,lo,hi)=>Math.max(lo,Math.min(hi,Number.isFinite(+x)?+x:lo));
const val=(v,k,d)=>clamp(v&&v[k]!==undefined?v[k]:d,0,100);
const pi=Math.PI;
let frame=0;
function line(ctx,x,y,w,h,fn){
 let last=0;
 for(let i=0;i<w;i++){
  const y0=Math.max(0,Math.min(h-1,Math.round(h*.5-fn(i/w)*(h*.43))));
  if(i)ctx.line(x+i-1,y+last,x+i,y+y0,1);
  last=y0;
 }
}
function render(ctx,o){
 const v=o&&o.values||{},W=ctx.width||128,H=ctx.height||40;
 if(W<40||H<24)return;
 let a=clamp(v.world_a??0,0,3)|0,b=clamp(v.world_b??1,0,3)|0;
 let morph=val(v,"morph",0)/100; const journey=(v.mode|0)===1;
 if(journey){const position=morph*3;a=Math.min(2,Math.floor(position));b=a+1;morph=Math.min(1,position-a);}
 const evo=val(v,"evolution",25)/100;
 const chaos=val(v,"chaos",0)/100,voices=clamp(v.voice_activity??0,0,4);
 const now=o&&o.nowMs;
 const time=(typeof now==="number"&&Number.isFinite(now)?now/1000:frame++/11);
 const t=voices?time*(.13+.6*evo):0;
 const names=["FLUID","ORGANISM","DRIFTSPACE","BLOOM"];
 ctx.fillRect(0,0,W,H,0);
 ctx.print(2,1,journey?"J "+names[a]:names[a],1);
 ctx.print(Math.max(2,W-8-names[b].length*6),1,names[b],1);
 const top=11,ht=Math.max(12,H-23),w=W-6;
 function wave(world,u){
  const p=u*2*pi;
  if(world===0)return .55*Math.sin(p*2+t)+.2*Math.sin(p*5-t*.7)+.12*Math.sin(p*9+t*.4);
  if(world===1)return .4*Math.sin(p*3+t*.3)+.28*Math.sin(p*7-t*.15)*Math.sin(p+t);
  if(world===2)return .38*Math.sin(p*2+t*.15)+.3*Math.sin(p*10+t*.3)+.12*Math.sin(p*17-t*.1);
  return .36*Math.sin(p*2)+.25*Math.sin(p*4+t*.7)+.2*Math.sin(p*8+t*1.2);
 }
 line(ctx,3,top,w,ht,u=>(wave(a,u)*(1-morph)+wave(b,u)*morph)*(1+.13*chaos));
 /* Distinct world accents: droplets, cells, stars, petals. */
 const active=morph>=.5?b:a;
 for(let i=0;i<7;i++){
  const x=7+i*Math.floor((W-14)/7),y=top+2+(i*7)%(Math.max(5,ht-4));
  if(active===0)ctx.fillRect(x,y,2,2,1);
  else if(active===1)ctx.fillRect(x-2,y-2,4+(i%2),4+(i%2),1);
  else if(active===2){ctx.line(x-2,y,x+2,y,1);ctx.line(x,y-2,x,y+2,1);}
  else ctx.fillRect(x-1,y-1,2+(i%3),2+(i%3),1);
 }
 const meterY=H-8,barW=W-8;
 ctx.fillRect(4,meterY,barW,3,0);
 ctx.fillRect(4,meterY,Math.round(barW*morph),3,1);
 ctx.fillRect(4+Math.min(barW-1,Math.round((barW-1)*morph)),meterY-2,2,7,1);
}
globalThis.canvas_overlays=Object.assign({},globalThis.canvas_overlays||{},{
 driftspace_main:{drawPage:render}
});
globalThis.canvas_overlay={drawPage:render};
})();
