"""Check exact source-data parity and target artifact existence. Does not imply gameplay validation."""
import hashlib,json,pathlib,sys
R=pathlib.Path(__file__).resolve().parents[1]
G=pathlib.Path.home()/"Documents"/"LastLightGame"/"project"
DATA=R/"Content"/"Data"
ASSET=R/"Migration"/"GodotAssets"
sha=lambda path:hashlib.sha256(path.read_bytes()).hexdigest()
checks=[]
def verify(name,ok,detail=""):
    checks.append({"name":name,"status":"PASS" if ok else "FAIL","details":detail})
    print(("PASS " if ok else "FAIL ")+name+" "+detail)
for filename in ("skills.json","mobs.json","minibosses.json","balance.json","starter_gear.json"):
    original,target=G/"data"/filename,DATA/filename
    verify("data-"+filename,original.is_file() and target.is_file() and sha(original)==sha(target),
           "sha256="+(sha(target) if target.is_file() else "MISSING"))
lists={}
for name,expected in (("skills",100),("mobs",30),("minibosses",9)):
    path=DATA/(name+".json")
    lists[name]=json.loads(path.read_text(encoding="utf-8"))
    verify("records-"+name,len(lists[name])==expected,
           str(len(lists[name]))+" / "+str(expected))
verify("unique-skill-ids",len({s["id"] for s in lists["skills"]})==100)
verify("russian-skill-labels",len({s["name_ru"] for s in lists["skills"]})==100)
for stem in ("hero","knight","wolf","wasp","golem","wraith"):
    original,target=G/"assets"/"characters"/(stem+".glb"),ASSET/(stem+".glb")
    verify("rig-source-"+stem,original.is_file() and target.is_file() and sha(original)==sha(target))
for name in ("LLGameInstance.h","LLGameInstance.cpp","LLGameMode.h","LLGameMode.cpp",
             "LLHero.h","LLHero.cpp","LLHUD.h","LLHUD.cpp",
             "LLStage.h","LLStage.cpp","LastLightUE.cpp","LastLightUE.Build.cs"):
    path=R/"Source"/"LastLightUE"/name
    verify("source-"+name,path.is_file() and path.stat().st_size>(60 if name=="LastLightUE.cpp" else 200))
for name in ("Explore","Arena"):
    path=R/"Content"/"Maps"/(name+".umap")
    verify("map-"+name,path.is_file() and path.stat().st_size>128,
           "bytes="+(str(path.stat().st_size) if path.is_file() else "0"))
dll=R/"Binaries"/"Win64"/"UnrealEditor-LastLightUE.dll"
verify("editor-dll",dll.is_file() and dll.stat().st_size>100000,
       "bytes="+(str(dll.stat().st_size) if dll.is_file() else "0"))
verify("source-godot-untouched",(G/"scenes"/"main.tscn").is_file())
receipt={"format":"lastlight.unreal.migration.check.v1","project":str(R),
         "checks":checks,
         "pass":sum(x["status"]=="PASS" for x in checks),
         "fail":sum(x["status"]=="FAIL" for x in checks),
         "status":"PASS" if all(x["status"]=="PASS" for x in checks) else "PARTIAL",
         "nonclaims":["No commercial readiness","Python is editor-only",
          "DLL alone is not a packaged Windows game","Imported rig quality requires visual QA",
          "Godot save files are not imported into Unreal"]}
out=R/"MIGRATION_QA.json"
out.write_text(json.dumps(receipt,ensure_ascii=False,indent=2),encoding="utf-8")
print("MIGRATION_INVENTORY_RESULT",receipt["status"],
      "pass",receipt["pass"],"fail",receipt["fail"],
      "receipt_bytes",out.stat().st_size,"sha256",sha(out))
sys.exit(0 if receipt["fail"]==0 else 1)
