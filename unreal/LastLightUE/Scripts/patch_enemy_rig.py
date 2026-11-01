from pathlib import Path
p=Path(r"E:\LastLightUnreal\LastLightUE\Source\LastLightUE\LLGameMode.cpp")
src=p.read_text(encoding="utf-8")
a=src.index("void ALLGameMode::SpawnOpponent()")
b=src.index("void ALLGameMode::Tick(float DeltaSeconds)",a)
replacement=r'''void ALLGameMode::SpawnOpponent()
{
 ULLGameInstance* Game=GetGameInstance<ULLGameInstance>();
 if(!Game)return;
 EnemyVisual=GetWorld()->SpawnActor<AActor>(AActor::StaticClass(),
  FVector(0,520,135),FRotator(0,-90,0));
 if(!EnemyVisual)return;
 USceneComponent* Anchor=NewObject<USceneComponent>(EnemyVisual,TEXT("EnemyRoot"));
 Anchor->SetMobility(EComponentMobility::Movable);
 EnemyVisual->SetRootComponent(Anchor);Anchor->RegisterComponent();

 const FString Shape=Game->ActiveEnemy.Shape.ToLower();
 FString Model=TEXT("knight");
 if(Game->bFinal)Model=TEXT("hero");
 else if(Shape.Contains(TEXT("wolf"))||Shape.Contains(TEXT("hound")))Model=TEXT("wolf");
 else if(Shape.Contains(TEXT("moth"))||Shape.Contains(TEXT("wasp"))
   ||Shape.Contains(TEXT("bee"))||Shape.Contains(TEXT("insect")))Model=TEXT("wasp");
 else if(Shape.Contains(TEXT("golem"))||Shape.Contains(TEXT("stone"))
   ||Shape.Contains(TEXT("giant")))Model=TEXT("golem");
 else if(Shape.Contains(TEXT("wraith"))||Shape.Contains(TEXT("specter"))
   ||Shape.Contains(TEXT("ghost"))||Shape.Contains(TEXT("shadow")))Model=TEXT("wraith");
 const FString Base=FString::Printf(TEXT("/Game/Characters/Joined/%s/%s/SkeletalMeshes/%s"),
  *Model,*Model,*Model);
 USkeletalMesh* Visual=LoadObject<USkeletalMesh>(nullptr,*(Base+TEXT(".")+Model));
 if(Visual)
 {
  EnemyRig=NewObject<USkeletalMeshComponent>(EnemyVisual,TEXT("SkinnedOpponent"));
  EnemyRig->SetupAttachment(Anchor);
  EnemyRig->SetSkeletalMeshAsset(Visual);
  EnemyRig->SetRelativeLocation(FVector(0,0,-93));
  // Actor yaw=-90 points toward hero at -Y, matching Unreal front +X.
  EnemyRig->SetRelativeRotation(FRotator::ZeroRotator);
  EnemyRig->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  EnemyRig->SetCastShadow(true);
  EnemyRig->RegisterComponent();
  EnemyIdle=LoadObject<UAnimSequence>(nullptr,
   *(Base+TEXT("Idle.")+Model+TEXT("Idle")));
  EnemyAttack=LoadObject<UAnimSequence>(nullptr,
   *(Base+TEXT("Attack.")+Model+TEXT("Attack")));
  EnemyHit=LoadObject<UAnimSequence>(nullptr,
   *(Base+TEXT("Hit.")+Model+TEXT("Hit")));
  if(EnemyIdle)EnemyRig->PlayAnimation(EnemyIdle,true);
  EnemyCurrentClip=TEXT("idle");
  UE_LOG(LogTemp,Display,TEXT("LL_ENEMY_RIG=YES model=%s id=%s clips=%d"),
   *Model,*Game->ActiveEnemy.Id, int32(EnemyIdle!=nullptr)+int32(EnemyAttack!=nullptr)+int32(EnemyHit!=nullptr));
  return;
 }
 UE_LOG(LogTemp,Warning,TEXT("LL_ENEMY_RIG=NO fallback for model=%s path=%s"),*Model,*Base);

 UStaticMesh* ShapeProxy=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));
 UStaticMesh* Globe=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere"));
 UStaticMeshComponent* Body=NewObject<UStaticMeshComponent>(EnemyVisual,TEXT("EnemyBody"));
 Body->SetupAttachment(Anchor);Body->SetStaticMesh(ShapeProxy);
 Body->SetRelativeLocation(FVector(0,0,35));
 Body->SetRelativeScale3D(Game->bFinal?FVector(.94f,.80f,1.55f):
  (Game->ActiveEnemy.bBoss?FVector(.9f,.8f,1.40f):FVector(.62f,.56f,1.05f)));
 Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);Body->RegisterComponent();
 UStaticMeshComponent* Head=NewObject<UStaticMeshComponent>(EnemyVisual,TEXT("EnemyHead"));
 Head->SetupAttachment(Anchor);Head->SetStaticMesh(Globe);
 Head->SetRelativeLocation(FVector(12,0,145));
 Head->SetRelativeScale3D(Game->bFinal?FVector(.46f):FVector(.37f));
 Head->SetCollisionEnabled(ECollisionEnabled::NoCollision);Head->RegisterComponent();
}
'''
src=src[:a]+replacement+src[b:]
needle=' Game->TickCombat(DeltaSeconds);\n if(EnemyVisual)'
change=''' Game->TickCombat(DeltaSeconds);
 if(EnemyRig)
 {
  if(Game->Phase==ELLPhase::EnemyTiming && EnemyLastAttackIndex!=Game->AttackIndex)
  {
   if(EnemyAttack)EnemyRig->PlayAnimation(EnemyAttack,false);
   EnemyCurrentClip=TEXT("attack");
   EnemyLastAttackIndex=Game->AttackIndex;
  }
  else if(Game->Phase!=ELLPhase::EnemyTiming && EnemyCurrentClip!=TEXT("idle"))
  {
   if(EnemyIdle)EnemyRig->PlayAnimation(EnemyIdle,true);
   EnemyCurrentClip=TEXT("idle");
  }
 }
 if(EnemyVisual)'''
assert needle in src
src=src.replace(needle,change,1)
p.write_text(src,encoding="utf-8")
print("PATCH_ENEMY_RIG_DONE",len(src),"bytes",p.stat().st_size)
