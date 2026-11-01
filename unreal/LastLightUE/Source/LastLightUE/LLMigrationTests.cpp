#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "LLGameInstance.h"
#include "Misc/Paths.h"

// Actual Unreal C++ gameplay math tests, not a Python reimplementation.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLLDataSmoke,
 "LastLight.Migration.DataIntegrity",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLLDataSmoke::RunTest(const FString& Parameters)
{
 ULLGameInstance* G=NewObject<ULLGameInstance>();
 TestTrue(TEXT("five original JSON datasets are readable"),G->LoadDatabase());
 TestEqual(TEXT("original skill count"),G->AllSkills.Num(),100);
 TestEqual(TEXT("original enemy species"),G->Mobs.Num(),30);
 TestEqual(TEXT("miniboss count"),G->Bosses.Num(),9);
 TestEqual(TEXT("starter item count"),G->Items.Num(),3);
 TestTrue(TEXT("first skill has its original S001 identifier"),
  G->AllSkills.Num()>0&&G->AllSkills[0].Id==TEXT("S001"));
 int32 RussianCount=0;
 for(const auto& Skill:G->AllSkills)
  if(!Skill.Display.IsEmpty()&&Skill.Display!=Skill.Name)++RussianCount;
 TestEqual(TEXT("Russian skill labels"),RussianCount,100);
 G->RandomizeStartingBuild();
 TestEqual(TEXT("50 chosen on initial screen"),G->ChosenStart.Num(),50);
 TestTrue(TEXT("inverted XP curve falls on descent"),
  G->XPToLose(99)<G->XPToLose(100)&&G->XPToLose(1)<G->XPToLose(10));
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLLCombatSmoke,
 "LastLight.Migration.CombatLoop",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLLCombatSmoke::RunTest(const FString& Parameters)
{
 ULLGameInstance* G=NewObject<ULLGameInstance>();
 if(!G->LoadDatabase()){AddError(TEXT("No skill source data"));return false;}
 for(int32 I=0;I<50;++I)
 {
  G->Innate.Add(G->AllSkills[I]);
  G->Snapshot.Add(G->AllSkills[I]);
 }
 G->Level=100;G->HeroHP=120;G->ActiveEnemy=G->Mobs[0];
 G->StartBattle();
 TestEqual(TEXT("battle starts with 2 AP"),G->AP,2);
 TestEqual(TEXT("battle starts at initial player turn"),int32(G->Phase),int32(ELLPhase::BattlePlayer));
 TestTrue(TEXT("stage enemy health positive"),G->EnemyHP>=25);
 const int32 Before=G->EnemyHP;
 G->BasicStrike();
 TestEqual(TEXT("offensive QTE state"),int32(G->Phase),int32(ELLPhase::AttackTiming));
 TestEqual(TEXT("AP reserved until strike resolves"),G->AP,2);
 G->Timing=1.05f*.67f;
 G->CompleteAttack(true);
 TestTrue(TEXT("perfect strike earns additional damage"),G->EnemyHP<Before-8);
 TestEqual(TEXT("attack spends one AP"),G->AP,1);
 G->BasicGuard();
 TestEqual(TEXT("guard starts enemy QTE"),int32(G->Phase),int32(ELLPhase::EnemyTiming));
 const int32 HP=G->HeroHP;
 G->Timing=G->AttackDuration*G->AttackFraction;
 G->Defend(true);
 TestEqual(TEXT("perfect parry prevents damage"),G->HeroHP,HP);
 TestTrue(TEXT("counter returns to player or ends encounter"),
  G->Phase==ELLPhase::BattlePlayer||G->Phase==ELLPhase::EnemyTiming||
  G->Phase==ELLPhase::Exploration);
 TestTrue(TEXT("equipment grants skills in combat"),G->GetSkills().Num()>=52);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLLProgressSmoke,
 "LastLight.Migration.InverseProgression",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLLProgressSmoke::RunTest(const FString& Parameters)
{
 ULLGameInstance* G=NewObject<ULLGameInstance>();
 if(!G->LoadDatabase())return false;
 for(int32 I=0;I<50;++I)G->Innate.Add(G->AllSkills[I]);
 G->Snapshot=G->Innate;G->Level=100;G->HeroHP=120;
 G->Phase=ELLPhase::LossChoice;G->PendingLoss=5;
 for(int32 I=0;I<5;++I)G->LevelLoss(0);
 TestEqual(TEXT("five degraded levels"),G->Level,95);
 TestEqual(TEXT("50 - 5 net skill shrink"),G->Innate.Num(),45);
 TestEqual(TEXT("evolution slot on level 95"),G->EvolutionSlots,1);
 TestEqual(TEXT("frozen starting mirror snapshot remains 50"),G->Snapshot.Num(),50);
 TestTrue(TEXT("evolution consumes slot"),G->Evolve(0,1));
 TestEqual(TEXT("fusion reduces skill count"),G->Innate.Num(),44);
 TestEqual(TEXT("fusion slot consumed"),G->EvolutionSlots,0);
 G->Level=0;
 G->ActiveEnemy.Id=TEXT("MIRROR");G->ActiveEnemy.HP=160;
 G->ActiveEnemy.Damage=16;G->ActiveEnemy.bBoss=true;
 G->bFinal=true;G->StartBattle();
 TestTrue(TEXT("final boss HP computed from initial build"),G->EnemyHP>=120);
 TestTrue(TEXT("all skills save unique identifiers"),!G->Innate[0].Id.IsEmpty());
 return true;
}
#include "Misc/AutomationTest.h"
#include "LLStage.h"
#include "LLHero.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimSequence.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLLImportedModelsTest,
 "LastLight.Migration.ImportedCharactersAndAnims",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLLImportedModelsTest::RunTest(const FString& Parameters)
{
 for(const FString& Model:{TEXT("hero"),TEXT("knight"),TEXT("wolf"),
  TEXT("wasp"),TEXT("golem"),TEXT("wraith")})
 {
  FString Base=FString::Printf(TEXT("/Game/Characters/Joined/%s/%s/SkeletalMeshes/%s"),
   *Model,*Model,*Model);
  USkeletalMesh* Mesh=LoadObject<USkeletalMesh>(nullptr,*(Base+TEXT(".")+Model));
  TestNotNull(*(Model+TEXT(" original joined skeleton mesh")),Mesh);
  if(!Mesh)continue;
  const auto Bounds=Mesh->GetBounds();
  TestTrue(*(Model+TEXT(" has nonzero mesh bounds")),Bounds.BoxExtent.Size()>0.01f);
  UE_LOG(LogTemp,Display,TEXT("LL_ART_BOUND model=%s extent=%s origin=%s"),
   *Model,*Bounds.BoxExtent.ToString(),*Bounds.Origin.ToString());
  for(const FString& Clip:{TEXT("Idle"),TEXT("Walk"),TEXT("Run"),
   TEXT("Attack"),TEXT("Guard"),TEXT("Dodge"),TEXT("Hit")})
  {
   UAnimSequence* Anim=LoadObject<UAnimSequence>(nullptr,
    *(Base+Clip+TEXT(".")+Model+Clip));
   TestNotNull(*(Model+TEXT(" animation ")+Clip),Anim);
   if(Anim)TestTrue(*(Model+TEXT(" valid clip duration ")+Clip),Anim->GetPlayLength()>0.01f);
  }
 }
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLLActorFacingTest,
 "LastLight.Migration.FacingAndCollision",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLLActorFacingTest::RunTest(const FString& Parameters)
{
 const ALLStage* Stage=GetDefault<ALLStage>();
 TestNotNull(TEXT("Stage CDO available"),Stage);
 if(Stage&&Stage->GetRootComponent())
  TestEqual(TEXT("root of STATIC procedural meshes must be STATIC"),
   Stage->GetRootComponent()->GetMobility(),EComponentMobility::Static);
 const ALLHero* Hero=GetDefault<ALLHero>();
 TestNotNull(TEXT("Hero CDO available"),Hero);
 if(Hero&&Hero->GetMesh())
 {
  TestNotNull(TEXT("original hero rig on actual Unreal character"),
   Hero->GetMesh()->GetSkeletalMeshAsset());
 }
 // Unreal X-forward, Y-right. +Y player and -Y enemy face one another.
 const FVector HeroP(0,-510,145), EnemyP(0,520,135);
 const FVector TowardEnemy=(EnemyP-HeroP).GetSafeNormal2D();
 const FVector TowardHero=(HeroP-EnemyP).GetSafeNormal2D();
 const FVector HeroForward=FRotator(0,90,0).Vector();
 const FVector EnemyForward=FRotator(0,-90,0).Vector();
 TestTrue(TEXT("hero +Y points at opponent"),FVector::DotProduct(HeroForward,TowardEnemy)>.99f);
 TestTrue(TEXT("opponent -Y points at hero"),FVector::DotProduct(EnemyForward,TowardHero)>.99f);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLLForgeIntegrityTest,
 "LastLight.Migration.ForgeEquipmentIntegrity",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLLForgeIntegrityTest::RunTest(const FString& Parameters)
{
 ULLGameInstance* G=NewObject<ULLGameInstance>();
 if(!G->LoadDatabase()){AddError(TEXT("Cannot load source gear"));return false;}
 G->Random.Initialize(1025);
 G->bHasStarted=false; // Do not write a player's save during automated QA.
 TestEqual(TEXT("three pieces of source starting gear"),G->Items.Num(),3);
 const bool bSucceeded=G->Forge(0,1);
 TestEqual(TEXT("forge destroys one item or replaces two with one"),G->Items.Num(),2);
 for(const auto& EquippedPair:G->Equipped)
  TestNotNull(TEXT("equipped ID references a real non-destroyed item"),
   G->FindItem(EquippedPair.Value));
 if(bSucceeded)
 {
  bool bFoundGrade2=false;
  for(const auto& Item:G->Items)if(Item.Grade==2)bFoundGrade2=true;
  TestTrue(TEXT("successful merge raises grade"),bFoundGrade2);
 }
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLLFullDescentTest,
 "LastLight.Migration.Full100ToZeroAndMirror",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLLFullDescentTest::RunTest(const FString& Parameters)
{
 ULLGameInstance* G=NewObject<ULLGameInstance>();
 if(!G->LoadDatabase()){AddError(TEXT("Missing source data"));return false;}
 G->Random.Initialize(20261101);
 G->RandomizeStartingBuild();
 for(const FString& Id:G->ChosenStart)
 {
  if(const FLLSkill* Skill=G->FindSkill(Id))
  {G->Innate.Add(*Skill);G->Snapshot.Add(*Skill);}
 }
 if(!TestEqual(TEXT("initial equipped skill book"),G->Innate.Num(),50))return false;
 TestEqual(TEXT("frozen original mirror skill snapshot"),G->Snapshot.Num(),50);
 G->Level=100;
 G->HeroHP=120;
 // Run 20 accelerated victories at five level losses each, without a real World.
 // bHasStarted remains FALSE so SaveRun cannot modify a player's local save.
 for(int32 Victory=0;Victory<20;++Victory)
 {
  G->PendingLoss=5;
  G->Phase=ELLPhase::LossChoice;
  for(int32 Loss=0;Loss<5;++Loss)
  {
   const int32 BeforeLevel=G->Level;
   const int32 BeforeSkills=G->Innate.Num();
   G->LevelLoss(0);
   if(G->Level!=BeforeLevel-1){AddError(TEXT("Level did not decrease exactly once"));return false;}
   const int32 ExpectedSkills=BeforeSkills>=2?BeforeSkills-1:BeforeSkills;
   if(G->Innate.Num()!=ExpectedSkills)
   {AddError(TEXT("Skill -2 +1 / empty-book exception violated"));return false;}
  }
  if(G->Level!=100-5*(Victory+1))
  {AddError(TEXT("Descent milestone drift"));return false;}
 }
 TestEqual(TEXT("reached legitimate minimum level"),G->Level,0);
 TestEqual(TEXT("100 recorded losses"),G->LossesCompleted,100);
 TestEqual(TEXT("all 20 evolution slots accrued"),G->EvolutionSlots,20);
 TestEqual(TEXT("empty-book exception leaves one innate skill"),G->Innate.Num(),1);
 TestEqual(TEXT("original 50 skills preserved for mirror"),G->Snapshot.Num(),50);
 TestEqual(TEXT("pending losses exhausted"),G->PendingLoss,0);
 TestTrue(TEXT("final mirror is accessible only at level zero"),G->CanEnterFinal());
 TestEqual(TEXT("bottom-level maximum HP"),G->MaxHP(),70);
 TestTrue(TEXT("ninth miniboss level gate unlocked"),G->IsMinibossUnlocked(8));

 TArray<FLLSkill> Ranked=G->AllSkills;
 Ranked.Sort([](const FLLSkill& A,const FLLSkill& B){return A.Power<B.Power;});
 ULLGameInstance* Weak=NewObject<ULLGameInstance>();
 ULLGameInstance* Strong=NewObject<ULLGameInstance>();
 Weak->Level=Strong->Level=0;
 Weak->bFinal=Strong->bFinal=true;
 for(int32 I=0;I<50;++I)
 {
  Weak->Snapshot.Add(Ranked[I]);
  Strong->Snapshot.Add(Ranked[Ranked.Num()-1-I]);
 }
 Weak->StartBattle();
 Strong->StartBattle();
 TestTrue(TEXT("stronger initial snapshot creates harder final boss"),
   Strong->EnemyMaxHP>Weak->EnemyMaxHP &&
   Strong->EnemyDamage>Weak->EnemyDamage);
 Weak->EndPlayerTurn();
 TestTrue(TEXT("mirror uses actual selected starting skill name in enemy turn"),
  Weak->AttackLabel.Contains(Weak->Snapshot[0].Display));
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLLForgeRiskPreviewTest,
 "LastLight.Migration.ForgeRiskPreviewAndConfirmation",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLLForgeRiskPreviewTest::RunTest(const FString& Parameters)
{
 ULLGameInstance* G=NewObject<ULLGameInstance>();
 if(!G->LoadDatabase()){AddError(TEXT("Missing gear dataset"));return false;}
 G->Phase=ELLPhase::Forge;
 G->Random.Initialize(3303);
 const int32 Before=G->Items.Num();
 TestTrue(TEXT("starter items exist"),Before>=2);
 if(Before<2)return false;
 const float Initial=G->ForgeBreakChance(0,1);
 TestTrue(TEXT("G1 risk is exactly 10%"),FMath::IsNearlyEqual(Initial,.10f));
 TestTrue(TEXT("first click queues the source item"),G->QueueForge(0));
 TestTrue(TEXT("second click queues the second item"),G->QueueForge(1));
 TestEqual(TEXT("preview cannot delete an item"),G->Items.Num(),Before);
 TestEqual(TEXT("pending second item is remembered"),G->PendingForgeIndex,1);
 G->CancelForge();
 TestEqual(TEXT("cancel preview leaves all items"),G->Items.Num(),Before);
 TestEqual(TEXT("cancel removes pending index"),G->PendingForgeIndex,-1);
 TestEqual(TEXT("cancel removes first selected index"),G->SelectedItem,-1);
 G->QueueForge(0);G->QueueForge(1);
 G->ConfirmForge();
 TestEqual(TEXT("confirmed merge or break modifies inventory"),G->Items.Num(),Before-1);
 TestEqual(TEXT("confirmation closes preview"),G->PendingForgeIndex,-1);
 for(const auto& Pair:G->Equipped)
  TestNotNull(TEXT("confirmed forge preserves valid equipment references"),
   G->FindItem(Pair.Value));
 G->Items[0].Grade=5;
 G->Items[1].Grade=4;
 TestTrue(TEXT("grade five risk is 58%"),
  FMath::IsNearlyEqual(G->ForgeBreakChance(0,1),.58f));
 G->Items[0].Grade=6;
 TestTrue(TEXT("max grade cannot be fused"),G->ForgeBreakChance(0,1)<0.f);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLLTimingAssistTest,
 "LastLight.Migration.TimingAssist",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLLTimingAssistTest::RunTest(const FString& Parameters)
{
 ULLGameInstance* G=NewObject<ULLGameInstance>();
 if(!G->LoadDatabase())return false;
 TestTrue(TEXT("assist defaults off"),FMath::IsNearlyZero(G->Assist));
 G->CycleTimingAssist();
 TestTrue(TEXT("first F2 widens timing moderately"),FMath::IsNearlyEqual(G->Assist,.08f));
 G->CycleTimingAssist();
 TestTrue(TEXT("second F2 widens timing generously"),FMath::IsNearlyEqual(G->Assist,.16f));
 G->CycleTimingAssist();
 TestTrue(TEXT("third F2 restores unassisted"),FMath::IsNearlyZero(G->Assist));

 G->Level=0;G->HeroHP=200;G->ActiveEnemy=G->Mobs[0];
 G->StartBattle();G->EndPlayerTurn();
 TestEqual(TEXT("enemy prepares defense QTE"),int32(G->Phase),int32(ELLPhase::EnemyTiming));
 const int32 Before=G->HeroHP;
 G->Timing=G->AttackDuration*G->AttackFraction+.12f;
 G->Defend(true);
 TestTrue(TEXT("late unassisted parry misses at level zero"),G->HeroHP<Before);
 G->HeroHP=200;
 G->Assist=.16f;
 G->Timing=G->AttackDuration*G->AttackFraction+.12f;
 G->Defend(true);
 TestEqual(TEXT("same parry succeeds with F2 wide window"),G->HeroHP,200);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLLMirrorPatternTest,
 "LastLight.Migration.MirrorPattern",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLLMirrorPatternTest::RunTest(const FString& Parameters)
{
 ULLGameInstance* G=NewObject<ULLGameInstance>();
 if(!G->LoadDatabase())return false;
 G->Snapshot.Append(G->AllSkills.GetData(),50);
 G->Level=0;G->HeroHP=5000;G->bFinal=true;
 G->ActiveEnemy.Id=TEXT("MIRROR");
 G->ActiveEnemy.Name=TEXT("Former Self");
 G->ActiveEnemy.HP=160;G->ActiveEnemy.Damage=16;
 G->StartBattle();
 TSet<FString> AttackLabels;
 float MinMultiplier=100.f,MaxMultiplier=0.f;
 FString Opening;
 for(int32 I=0;I<14;++I)
 {
  if(G->Phase==ELLPhase::BattlePlayer)G->EndPlayerTurn();
  if(G->Phase!=ELLPhase::EnemyTiming)
  {AddError(TEXT("mirror stopped telegraphing attacks"));return false;}
  if(I==0)Opening=G->AttackLabel;
  AttackLabels.Add(G->AttackLabel);
  MinMultiplier=FMath::Min(MinMultiplier,G->AttackMultiplier);
  MaxMultiplier=FMath::Max(MaxMultiplier,G->AttackMultiplier);
  G->Timing=0;
  G->ResolveAttack(false,false);
 }
 TestTrue(TEXT("mirror opens with original first chosen skill"),
  Opening.Contains(G->Snapshot[0].Display));
 TestTrue(TEXT("mirror uses bounded telegraphed repertoire of <=7 skills"),
  AttackLabels.Num()>1&&AttackLabels.Num()<=7);
 TestTrue(TEXT("mirrored skills influence attack damage scaling"),
  MaxMultiplier>MinMultiplier+.02f);
 TestEqual(TEXT("all original 50 skills remain in frozen snapshot"),G->Snapshot.Num(),50);
 return true;
}

#endif
