from pathlib import Path
p=Path(r"E:\LastLightUnreal\LastLightUE\Source\LastLightUE\LLStage.cpp")
s=p.read_text(encoding="utf-8")
if "LL_WORLD_RIG_ACTOR" in s:
    raise SystemExit("already patched - abort")
start=s.index("   EnemyPositions.Add(Spawn);")
end=s.index("  }\n  TArray<FTransform> Cliffs,Spikes;",start)
replacement=r'''    EnemyPositions.Add(Spawn);
    // Original enemy species index is stable even when dead enemies disappear.
    AddMesh(Cylinder,Spawn+FVector(0,0,-111),FVector(.72f,.72f,.15f),Accent);
    ULLGameInstance* GI=GetWorld()?GetWorld()->GetGameInstance<ULLGameInstance>():nullptr;
    const int32 MobIndex=Zone*6+I;
    if(GI&&GI->Mobs.IsValidIndex(MobIndex)
      &&GI->DefeatedIds.Contains(GI->Mobs[MobIndex].Id))continue;
    const FString Shape=GI&&GI->Mobs.IsValidIndex(MobIndex)
      ?GI->Mobs[MobIndex].Shape.ToLower():TEXT("wolf");
    FString Model=TEXT("knight");
    if(Shape.Contains(TEXT("wolf"))||Shape.Contains(TEXT("hound")))Model=TEXT("wolf");
    else if(Shape.Contains(TEXT("moth"))||Shape.Contains(TEXT("wasp"))
      ||Shape.Contains(TEXT("bee"))||Shape.Contains(TEXT("insect")))Model=TEXT("wasp");
    else if(Shape.Contains(TEXT("golem"))||Shape.Contains(TEXT("stone"))
      ||Shape.Contains(TEXT("giant")))Model=TEXT("golem");
    else if(Shape.Contains(TEXT("wraith"))||Shape.Contains(TEXT("specter"))
      ||Shape.Contains(TEXT("ghost"))||Shape.Contains(TEXT("shadow")))Model=TEXT("wraith");
    const FString BasePath=FString::Printf(
      TEXT("/Game/Characters/Joined/%s/%s/SkeletalMeshes/%s"),*Model,*Model,*Model);
    USkeletalMesh* Skin=LoadObject<USkeletalMesh>(nullptr,*(BasePath+TEXT(".")+Model));
    if(Skin)
    {
     auto* Mob=NewObject<USkeletalMeshComponent>(this);
     Mob->SetupAttachment(StageRoot);
     Mob->SetMobility(EComponentMobility::Movable);
     Mob->SetSkeletalMeshAsset(Skin);
     Mob->SetRelativeLocation(Spawn+FVector(0,0,-116));
     Mob->SetRelativeRotation(FRotator(0,Spawn.Y>0?-90.f:90.f,0));
     Mob->SetRelativeScale3D(Model==TEXT("wasp")?FVector(.47):
       (Model==TEXT("wolf")?FVector(.52):FVector(.63)));
     Mob->SetCollisionEnabled(ECollisionEnabled::NoCollision);
     Mob->SetCastShadow(false);
     Mob->RegisterComponent();
     UAnimSequence* Idle=LoadObject<UAnimSequence>(nullptr,
       *(BasePath+TEXT("Idle.")+Model+TEXT("Idle")));
     if(Idle)Mob->PlayAnimation(Idle,true);
     ++SkinnedEncounterCount;
    }
    else
    {
     AddMesh(Cube,Spawn+FVector(0,0,18),FVector(.48f,.42f,.80f),Base);
     AddMesh(Sphere,Spawn+FVector(0,0,96),FVector(.32f,.32f,.32f),
       FLinearColor(.83f,.87f,.91f));
    }
    // LL_WORLD_RIG_ACTOR
'''
s=s[:start]+replacement+s[end:]
assert "LL_WORLD_RIG_ACTOR" in s and "Mob->PlayAnimation(Idle,true);" in s
assert "AddMesh(Cube,Spawn+FVector(0,0,55)" not in s
p.write_text(s,encoding="utf-8")
print("PATCH_WORLD_RIGS_WRITTEN",p.stat().st_size,
      "new_skeletal_components",s.count("NewObject<USkeletalMeshComponent>"),
      "has_static_root", "StageRoot->SetMobility(EComponentMobility::Static)" in s)
