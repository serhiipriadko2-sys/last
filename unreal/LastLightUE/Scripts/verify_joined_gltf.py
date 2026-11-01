import json,struct,pathlib,sys,hashlib
R=pathlib.Path(__file__).resolve().parents[1]
D=R/"Migration"/"JoinedExports"
results=[]
for name in ("hero","knight","wolf","wasp","golem","wraith"):
    p=D/(name+".glb")
    if not p.is_file():results.append((name,False,"MISSING"));continue
    b=p.read_bytes()
    try:
        magic,version,total=struct.unpack_from("<4sII",b,0)
        length,chunk=struct.unpack_from("<I4s",b,12)
        gltf=json.loads(b[20:20+length].decode("utf-8"))
        meshes=gltf.get("meshes",[])
        skins=gltf.get("skins",[])
        anim=gltf.get("animations",[])
        channels=[len(a.get("channels",[])) for a in anim]
        works=(magic==b"glTF" and version==2 and len(meshes)==1
               and len(skins)==1 and len(anim)>=7 and min(channels)>0)
        details=f"meshes={len(meshes)} skins={len(skins)} animations={len(anim)} animation_channels={channels} size={len(b)} sha256={hashlib.sha256(b).hexdigest()}"
        print(("LL_GLB_PASS" if works else "LL_GLB_FAIL"),name,details)
        results.append((name,works,details))
    except Exception as exc:
        results.append((name,False,str(exc)))
print("LL_JOINED_GLB_QA",sum(x[1] for x in results),"/",len(results))
sys.exit(0 if len(results)==6 and all(x[1] for x in results) else 1)
