#include "LLGameMode.h"
#include "LLGameInstance.h"
#include "LLStage.h"
#include "LLHero.h"
#include "LLHUD.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerController.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimSequence.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "UnrealClient.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

ALLGameMode::ALLGameMode()
{
 PrimaryActorTick.bCanEverTick=true;
 DefaultPawnClass=ALLHero::StaticClass();
 HUDClass=ALLHUD::StaticClass();
}

void ALLGameMode::StartPlay()
{
 Super::StartPlay();
 ULLGameInstance* Game=GetGameInstance<ULLGameInstance>();
 if(!Game)return;
 const FString LevelName=UGameplayStatics::GetCurrentLevelName(this,true);
 bArenaMap=LevelName.Equals(TEXT("Arena"),ESearchCase::IgnoreCase);
 Stage=GetWorld()->SpawnActor<ALLStage>(ALLStage::StaticClass(),FTransform::Identity);
 if(!Stage){UE_LOG(LogTemp,Error,TEXT("LASTLIGHT STAGE MISSING"));return;}
 if(bArenaMap)
 {
  if(!Game->bInBattle)
  {
   UE_LOG(LogTemp,Warning,TEXT("Arena opened without encounter, returning to Explore"));
   UGameplayStatics::OpenLevel(GetWorld(),FName(TEXT("/Game/Maps/Explore")));return;
  }
  Stage->BuildArena(Game->CurrentBiome,Game->bFinal);
 }
 else Stage->BuildExplore();
 APlayerController* PC=GetWorld()->GetFirstPlayerController();
 ALLHero* Hero=PC?Cast<ALLHero>(PC->GetPawn()):nullptr;
 if(PC&&!Hero)
 {
  FActorSpawnParameters Params;
  Hero=GetWorld()->SpawnActor<ALLHero>(ALLHero::StaticClass(),FVector(150,0,180),
   FRotator::ZeroRotator,Params);
  if(Hero)PC->Possess(Hero);
 }
 if(Hero)SetupPawn(Hero);
 if(Hero)
 {
  USkeletalMesh* Mesh=Hero->GetMesh()?Hero->GetMesh()->GetSkeletalMeshAsset():nullptr;
  UE_LOG(LogTemp,Display,TEXT("LL_HERO_RIG=%s imported=%s bounds=%s"),
   Mesh?TEXT("YES"):TEXT("NO"),Mesh?*Mesh->GetPathName():TEXT("none"),
   Mesh?*Mesh->GetBounds().BoxExtent.ToString():TEXT("none"));
 }
 if(bArenaMap)SpawnOpponent();
 UE_LOG(LogTemp,Display,TEXT("LL_STARTPLAY: level=%s arena=%s stage_geo=%d player=%s"),
  *LevelName,bArenaMap?TEXT("YES"):TEXT("NO"),Stage->GeometryCount,
  Hero?TEXT("YES"):TEXT("NO"));
}
void ALLGameMode::SetupPawn(ALLHero* Player)
{
 ULLGameInstance* Game=GetGameInstance<ULLGameInstance>();
 APlayerController* PC=Cast<APlayerController>(Player->GetController());
 if(!Game)return;
 if(bArenaMap)
 {
  Player->SetActorLocationAndRotation(FVector(0,-510,145),FRotator(0,90,0));
  if(PC)PC->SetControlRotation(FRotator(-13,90,0));
  // Over-the-shoulder: keep camera inside the decorative ring, not behind pillars.
  Player->CameraBoom->TargetArmLength=740.0f;
  Player->CameraBoom->SetRelativeLocation(FVector(0,0,145));
 }
 else
 {
  Player->SetActorLocationAndRotation(
   Game->bHasStarted?Game->SavedWorldLocation:FVector(160,0,165),
   FRotator(0,0,0));
  if(PC)PC->SetControlRotation(FRotator(-21,0,0));
  Player->CameraBoom->TargetArmLength=950.0f;
  Player->CameraBoom->SetRelativeLocation(FVector(0,0,135));
 }
}
void ALLGameMode::SpawnOpponent()
{
 ULLGameInstance* Game=GetGameInstance<ULLGameInstance>();
 if(!Game)return;
 EnemyVisual=GetWorld()->SpawnActor<AActor>(AActor::StaticClass(),
  FVector(0,520,105),FRotator(0,-90,0));
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
  EnemyRig->SetRelativeLocation(FVector(0,0,-50)); // Feet align to 55cm pedestal top.
  // Actor yaw=-90 points toward hero at -Y, matching Unreal front +X.
  // Blender wolf/wasp face local -Y (source snout at negative Y).
  // Turn their skeletal mesh +90 degrees, then actor yaw -90 points it toward the hero.
  EnemyRig->SetRelativeRotation(
    FRotator(0,(Model==TEXT("wolf")||Model==TEXT("wasp"))?90.f:0.f,0));
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
void ALLGameMode::Tick(float DeltaSeconds)
{
 Super::Tick(DeltaSeconds);
 Clock+=DeltaSeconds;
 ULLGameInstance* Game=GetGameInstance<ULLGameInstance>();
 if(!Game)return;
#if !UE_BUILD_SHIPPING
 // Offscreen QA creates screenshots FROM THE GAME VIEWPORT, never from the user's desktop.
 // Both options are ignored during normal play, and entirely absent in Shipping builds.
 if(!bArenaMap && !Game->bHasStarted &&
    FParse::Param(FCommandLine::Get(),TEXT("LLQAExplore")) && Clock>.5f)
 {
  Game->RandomizeStartingBuild();
  Game->StartRun(GetWorld());
  return;
 }
 if(!bQAScreenshotTaken && Clock>2.f &&
    FParse::Param(FCommandLine::Get(),TEXT("LLScreenshotQA")))
 {
  bQAScreenshotTaken=true;
  const FString ScreenshotDir=FPaths::Combine(
    FPaths::ProjectSavedDir(),TEXT("Screenshots"),TEXT("Windows"),TEXT("LL_QA"));
  IFileManager::Get().MakeDirectory(*ScreenshotDir,true);
  const FString Name=bArenaMap?TEXT("Arena_v05.png"):TEXT("Explore_v05.png");
  const FString File=FPaths::Combine(ScreenshotDir,Name);
  FScreenshotRequest::RequestScreenshot(File,true,false,false,FIntRect(),true);
  UE_LOG(LogTemp,Display,TEXT("LL_SCREENSHOT_QA_REQUEST level=%s file=%s"),
   bArenaMap?TEXT("Arena"):TEXT("Explore"),*File);
 }
 // Deterministic, isolated runtime smoke. Never enabled without explicit launch flag.
 if(!bArenaMap && FParse::Param(FCommandLine::Get(),TEXT("LLSmokeArena")) && Clock>.5f)
 {
  if(!Game->bHasStarted)
  {
   Game->RandomizeStartingBuild();
   Game->StartRun(GetWorld());
   return;
  }
  if(Game->Phase==ELLPhase::Exploration)
  {
   Game->Engage(GetWorld(),0,false,false,false,
     FVector(160.f,0.f,165.f));
   return;
  }
 }
#endif
 if(!bArenaMap)return;
 Game->TickCombat(DeltaSeconds);
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
 if(EnemyVisual)
 {
  FVector Position(0,520,105+FMath::Sin(Clock*1.9f)*5);
  if(Game->Phase==ELLPhase::EnemyTiming)
  {
   const float Ratio=FMath::Clamp(Game->Timing/FMath::Max(.1f,Game->AttackDuration),0,1);
   Position.Y-=Ratio*125.0f;Position.Z+=FMath::Sin(Ratio*PI)*22.0f;
  }
  EnemyVisual->SetActorLocation(Position);
  EnemyVisual->SetActorRotation(FRotator(0,-90,0));
 }
}
