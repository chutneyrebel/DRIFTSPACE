#!/usr/bin/env python3
"""Host-side ABI, audio and MIDI smoke test. Not a Move hardware test."""
import ctypes as C
import json
from pathlib import Path
import subprocess
import tempfile
import math
ROOT=Path(__file__).resolve().parents[1]
meta=json.loads((ROOT/'src/module.json').read_text())
assert len((ROOT/'src/module.json').read_bytes())<8192
assert meta['api_version']==2 and meta['id']=='driftspace'
with tempfile.TemporaryDirectory() as d:
    so=str(Path(d)/'dsp.so')
    subprocess.run(['cc','-std=c11','-O2','-shared','-fPIC','-ffreestanding','-fvisibility=hidden',str(ROOT/'src/dsp.c'),'-o',so],check=True)
    CREATE=C.CFUNCTYPE(C.c_void_p,C.c_char_p,C.c_char_p)
    DESTROY=C.CFUNCTYPE(None,C.c_void_p)
    MIDI=C.CFUNCTYPE(None,C.c_void_p,C.POINTER(C.c_uint8),C.c_int,C.c_int)
    SET=C.CFUNCTYPE(None,C.c_void_p,C.c_char_p,C.c_char_p)
    GET=C.CFUNCTYPE(C.c_int,C.c_void_p,C.c_char_p,C.c_char_p,C.c_int)
    RENDER=C.CFUNCTYPE(None,C.c_void_p,C.POINTER(C.c_int16),C.c_int)
    class API(C.Structure):
        _fields_=[('version',C.c_uint32),('create',CREATE),('destroy',DESTROY),('midi',MIDI),('set',SET),('get',GET),('error',GET),('render',RENDER)]
    lib=C.CDLL(so)
    lib.move_plugin_init_v2.argtypes=[C.c_void_p]
    lib.move_plugin_init_v2.restype=C.POINTER(API)
    api=lib.move_plugin_init_v2(None).contents
    assert api.version==2
    for key in ('chain_params','ui_hierarchy'):
        buf=C.create_string_buffer(8192)
        assert api.get(None,key.encode(),buf,8192)>0
        live=json.loads(buf.value)
        assert live==meta['capabilities'][key],(key,'metadata mismatch')
    instance=api.create(b'.',None)
    assert instance
    def send(st,n,v):
        msg=(C.c_uint8*3)(st,n,v)
        api.midi(instance,msg,3,0)
    def setp(k,v):api.set(instance,k.encode(),str(v).encode())
    def getp(k):
        b=C.create_string_buffer(128)
        assert api.get(instance,k.encode(),b,128)>=0
        return b.value.decode()
    def audio(blocks=16):
        b=(C.c_int16*256)()
        samples=[]
        for _ in range(blocks):
            api.render(instance,b,128)
            samples.extend(b)
        return samples
    assert not any(audio())
    send(0x90,60,110)
    assert getp('voice_activity')=='1'
    initial=audio()
    assert any(initial) and max(abs(x) for x in initial)<=32767
    assert any(x<0 for x in initial) and any(x>0 for x in initial)
    for w in range(4):
        setp('world_a',w);setp('world_b',(w+1)%4);setp('morph',50)
        samples=audio(8)
        assert any(samples)
        assert max(abs(x) for x in samples)<=32767
    # Phase 4: verify that each world is audible and timbrally distinguishable.
    setp('mode',0);setp('morph',0);setp('chaos',80);setp('evolution',90)
    fingerprints=[]
    for w in range(4):
        setp('world_a',w)
        audio(8)  # settle morph and discard transient
        samples=audio(8)
        fingerprints.append(tuple(samples[::64]))
    assert len(set(fingerprints))==4, 'world outputs must differ'
    # Phase 5: generative satellites are bounded and do not create MIDI voices.
    setp('generation',100);setp('chaos',100)
    generated=audio(200)
    assert any(generated) and max(abs(x) for x in generated)<=32767
    assert getp('voice_activity')=='1'
    setp('freeze',1)
    assert any(audio(32))
    setp('freeze',0);setp('generation',0)
    setp('mode',1)
    assert getp('mode')=='1'
    for position in (0,16,33,34,50,66,67,84,100):
        setp('morph',position)
        samples=audio(8)
        assert any(samples)
        assert max(abs(x) for x in samples)<=32767
    setp('mode',2)
    assert getp('mode')=='2'
    send(0x90,67,110)
    physics_samples=audio(32)
    assert any(physics_samples), 'physics mode should produce sound'
    assert max(abs(x) for x in physics_samples)<=32767
    setp('freeze',1)
    assert any(audio(8)), 'frozen physics should retain resonator audio tail'
    setp('freeze',0)
    send(0x80,67,0)
    audio(160)
    setp('mode',0)
    assert getp('mode')=='0'
    setp('morph',999)
    assert getp('morph')=='100'
    setp('freeze',1)
    assert getp('freeze')=='1'
    send(0x80,60,0)
    audio(160)
    assert getp('voice_activity')=='0'
    send(0x90,64,100)
    send(0xB0,123,0)
    assert getp('voice_activity')=='0'
    api.destroy(instance)
print('PASS: v2 ABI, metadata, four worlds, bipolar audio, morph, clamping, freeze, note-off, panic')
