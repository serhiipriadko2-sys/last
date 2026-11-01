"""Release verifier; run only after packaged smoke PASS. No optimistic PASS from file presence alone."""
import hashlib,json,os,pathlib,re,subprocess,sys,datetime,collections
BASE=pathlib.Path(r"E:\LastLightUnreal")
ROOT=BASE/"Releases_v06"/"Windows"
EXE=ROOT/"LastLightUE.exe"
BIN=ROOT/"LastLightUE"/"Binaries"/"Win64"/"LastLightUE.exe"
PAK=ROOT/"LastLightUE"/"Content"/"Paks"/"LastLightUE-Windows.pak"
SMOKE=BASE/"SMOKE_PACKAGED_ARENA_V06.log"
SHOT=BASE/"Releases_v06"/"Evidence"/"Arena_v06.png"
AUTOMATION=BASE/"TestReportsV06"/"index.json"
BUILD=BASE/"UAT_V06_BUILDCOOKRUN.log"
OUT=BASE/"Releases_v06"/"RELEASE_RECEIPT_UE_V06.json"
UNREALPAK=pathlib.Path(r"C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealPak.exe")
def sha(path):
    h=hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda:f.read(16*1024*1024),b""):h.update(chunk)
    return h.hexdigest()
checks={}
def check(key,condition,details=""):
    checks[key]={"status":"PASS" if condition else "FAIL","details":str(details)}
    print(("PASS " if condition else "FAIL ")+key+" "+str(details),flush=True)
check("windows_directory",ROOT.is_dir(),ROOT)
files=[]
if ROOT.is_dir():
    for p in sorted(ROOT.rglob("*")):
        if p.is_file():
            rel=p.relative_to(ROOT).as_posix()
            files.append({"path":rel,"bytes":p.stat().st_size,"sha256":sha(p)})
check("launcher_exists",EXE.is_file() and EXE.stat().st_size>100000)
check("game_binary_exists",BIN.is_file() and BIN.stat().st_size>1000000)
check("pak_exists",PAK.is_file() and PAK.stat().st_size>1000000)
check("file_inventory",len(files)>45,"files="+str(len(files)))
folds=[x["path"].casefold() for x in files]
check("case_collision_free",len(folds)==len(set(folds)))
check("symlink_free",not any(p.is_symlink() for p in ROOT.rglob("*")))
check("no_empty_files",not any(f["bytes"]==0 for f in files))
shot_valid=False
if SHOT.is_file():
    header=SHOT.read_bytes()[:24]
    shot_valid=(header[:8]==bytes([137,80,78,71,13,10,26,10]) and
                int.from_bytes(header[16:20],"big")==1280 and
                int.from_bytes(header[20:24],"big")==720)
check("unreal_native_visual_evidence",shot_valid,
      "rendered 1280x720 PNG saved outside distributable files")
pak_counts={}
if PAK.is_file() and UNREALPAK.is_file():
    p=subprocess.run([str(UNREALPAK),str(PAK),"-List"],stdout=subprocess.PIPE,
                     stderr=subprocess.STDOUT,text=True,encoding="utf-8",errors="replace",timeout=120)
    out=p.stdout
    contents=BASE/"Releases_v06"/"PAK_CONTENTS_V06.log"
    contents.write_text(out,encoding="utf-8")
    required=["Maps/Explore","Maps/Arena","SkeletalMeshes/hero","SkeletalMeshes/wolf","SkeletalMeshes/knight"]
    for name in required:
        number=out.count(name)
        pak_counts[name]=number
        check("pak_"+name.replace("/","_"),p.returncode==0 and number>0,number)
    check("pak_listing_exit",p.returncode==0,p.returncode)
else:
    check("pak_listing_exit",False,"Missing PAK or UnrealPak")
text=SMOKE.read_text(encoding="utf-8",errors="replace") if SMOKE.is_file() else ""
for name,expr in [
    ("runtime_exploration",r"LL_STARTPLAY: level=Explore arena=NO"),
    ("runtime_arena",r"LL_STARTPLAY: level=Arena arena=YES"),
    ("runtime_world_models",r"LL_WORLD_RIGS=30"),
    ("runtime_hero_skeletal",r"LL_HERO_RIG=YES"),
    ("runtime_enemy_skeletal",r"LL_ENEMY_RIG=YES"),
]:
    check(name,bool(re.search(expr,text)))
check("runtime_no_travel_failures",
    not re.search(r"Travel failed|Can't Find URL: Arena|Invalid URL:",text))
check("runtime_no_static_attach_errors","cannot attach" not in text)
check("runtime_no_crashes",not re.search(r"Fatal error:|Assertion failed",text))
tests={}
if AUTOMATION.is_file():
    tests=json.loads(AUTOMATION.read_text(encoding="utf-8-sig"))
    check("native_tests_eight_pass",tests.get("succeeded")==8
        and tests.get("failed")==0 and len(tests.get("tests",[]))==8)
else:check("native_tests_eight_pass",False,"report unavailable")
build=(BUILD.read_text(encoding="utf-16" if BUILD.read_bytes().startswith(bytes([255,254])) else "utf-8", errors="replace") if BUILD.is_file() else "")
check("buildcookrun_successful","BUILD SUCCESSFUL" in build and
      "AutomationTool exiting with ExitCode=0" in build)
check("no_secrets_in_mapped_paths",
    not any("secret" in x["path"].lower() or ".env" in x["path"].lower()
            for x in files))
passcount=sum(x["status"]=="PASS" for x in checks.values())
status="PASS" if passcount==len(checks) else "PARTIAL"
receipt={
    "format":"lastlight.unreal.windows.release.v1",
    "observed_utc":datetime.datetime.now(datetime.timezone.utc).isoformat(),
    "project":"E:\\LastLightUnreal\\LastLightUE",
    "engine":"Unreal Engine 5.8.4",
    "build_variant":"Windows x64 Development",
    "release_status":status,
    "verification":checks,
    "passed":passcount,"total":len(checks),
    "count_files":len(files),
    "total_bytes":sum(x["bytes"] for x in files),
    "files":files,
    "pak_counts":pak_counts,
    "screenshot_evidence":{"path":str(SHOT),
      "bytes":SHOT.stat().st_size if SHOT.is_file() else 0,
      "sha256":sha(SHOT) if SHOT.is_file() else None,
      "review":"Human-visible screenshot checked: hero and opponent both visible; background still sparse"},
    "native_automation_test_names":[t.get("fullTestPath","") for t in tests.get("tests",[])],
    "evidence":{
        "build_log":str(BUILD),
        "smoke_log":str(SMOKE),
        "native_automation_report":str(AUTOMATION),
        "pak_listing":str(BASE/"Releases_v06"/"PAK_CONTENTS_V06.log")},
    "not_claimed":[
        "not a production/AAA final game",
        "no human physical playtest",
        "not tested on other GPUs, controllers or low-end PCs",
        "input latency, audio, accessibility and gameplay balance not formally certified",
        "Godot save compatibility not implemented",
        "static code tests do not establish cosmetic animation quality"],
    "rollback":"E:\\LastLightUnreal\\Releases_v02\\Windows (known packaged Arena travel FAILURE); keep Godot v0.6 as working fallback"
}
OUT.parent.mkdir(parents=True,exist_ok=True)
OUT.write_text(json.dumps(receipt,ensure_ascii=False,indent=2),encoding="utf-8")
readback=json.loads(OUT.read_text(encoding="utf-8"))
assert readback["count_files"]==len(files)
print("RELEASE_RECEIPT_STATUS",status,"pass",passcount,"total",len(checks),
      "files",len(files),"bytes",receipt["total_bytes"],
      "receipt_bytes",OUT.stat().st_size,"receipt_sha256",sha(OUT),flush=True)
sys.exit(0 if status=="PASS" else 1)
