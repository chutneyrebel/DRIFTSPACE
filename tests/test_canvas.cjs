const fs=require('fs'),vm=require('vm'),assert=require('assert');
const code=fs.readFileSync('src/canvas.js','utf8');
const sandbox={globalThis:{}};vm.runInNewContext(code,sandbox,{timeout:1000});
const draw=sandbox.globalThis.canvas_overlays.driftspace_main.drawPage;
assert.strictEqual(typeof draw,'function');
for(let a=0;a<4;a++)for(let b=0;b<4;b++){
 const calls=[];
 const ctx={width:128,height:40,fillRect:(...v)=>calls.push(['fillRect',...v]),print:(...v)=>calls.push(['print',...v]),drawLine:(...v)=>calls.push(['drawLine',...v]),drawCircle:(...v)=>calls.push(['drawCircle',...v])};
 draw(ctx,{values:{world_a:a,world_b:b,morph:55,evolution:50,chaos:20,voice_activity:2},nowMs:500});
 assert(calls.length>40);
 for(const call of calls)for(const v of call.slice(1))if(typeof v==='number')assert(Number.isFinite(v));
}
console.log('PASS: 16 world pairs draw safely');
