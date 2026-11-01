from pathlib import Path
root=Path(r"E:\LastLightUnreal\LastLightUE\Source\LastLightUE")
total=0
for name in ("LLGameInstance.cpp","LLGameMode.cpp","LLHero.cpp"):
    p=root/name
    s=p.read_text(encoding="utf-8")
    old_e='FName(TEXT("Explore"))'
    old_a='FName(TEXT("Arena"))'
    n=s.count(old_e)+s.count(old_a)
    if n==0:
        if '/Game/Maps/Explore' in s or '/Game/Maps/Arena' in s:
            print("SKIP_ALREADY_PATCHED",name)
            continue
        raise RuntimeError("Expected map transition not found: "+name)
    s=s.replace(old_e,'FName(TEXT("/Game/Maps/Explore"))')
    s=s.replace(old_a,'FName(TEXT("/Game/Maps/Arena"))')
    assert old_e not in s and old_a not in s
    p.write_text(s,encoding="utf-8")
    total+=n
    print("MAP_URLS_FIXED",name,n)
assert total>=5,total
print("FULL_MAP_URL_PATCH_PASS",total)
