#include "LLGameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Dom/JsonObject.h"

namespace LL
{
 static TSharedPtr<FJsonObject> JsonFile(const FString& Name)
 {
  FString Text;
  const FString Path=FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data"), Name);
  if(!FFileHelper::LoadFileToString(Text,*Path))
  {
   UE_LOG(LogTemp,Error,TEXT("LASTLIGHT DATA MISSING: %s"),*Path);
   return nullptr;
  }
  TSharedPtr<FJsonObject> Object;
  const auto Reader=TJsonReaderFactory<>::Create(Text);
  if(!FJsonSerializer::Deserialize(Reader,Object))return nullptr;
  return Object;
 }
 static TArray<TSharedPtr<FJsonValue>> ArrayFile(const FString& Name)
 {
  FString Text;
  if(!FFileHelper::LoadFileToString(Text,
   *FPaths::Combine(FPaths::ProjectContentDir(),TEXT("Data"),Name)))return {};
  TArray<TSharedPtr<FJsonValue>> Values;
  if(!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text),Values))return {};
  return Values;
 }
 static FString Str(const TSharedPtr<FJsonObject>& O, const TCHAR* Key, const FString& Default=TEXT(""))
 { FString Value;return O.IsValid() && O->TryGetStringField(Key,Value)?Value:Default; }
 static int32 Int(const TSharedPtr<FJsonObject>& O,const TCHAR* Key,int32 Default=0)
 { double V=0;return O.IsValid()&&O->TryGetNumberField(Key,V)?FMath::RoundToInt(V):Default; }
 static TArray<FString> Strings(const TSharedPtr<FJsonObject>& O,const TCHAR* Key)
 {
  TArray<FString> Output;
  const TArray<TSharedPtr<FJsonValue>>* Values=nullptr;
  if(O.IsValid()&&O->TryGetArrayField(Key,Values))
  {
   for(const auto& Value:*Values) if(Value.IsValid())Output.Add(Value->AsString());
  }
  return Output;
 }
 static TArray<TSharedPtr<FJsonValue>> Values(const TArray<FString>& List)
 { TArray<TSharedPtr<FJsonValue>> Result;for(const FString& S:List)Result.Add(MakeShared<FJsonValueString>(S));return Result; }
}

void ULLGameInstance::Init()
{
 Super::Init();
 Random.Initialize(static_cast<int32>(FDateTime::UtcNow().GetTicks()));
 bInBattle=false;Phase=ELLPhase::Menu;
 if(!LoadDatabase())SetNotice(TEXT("Unable to read original LAST LIGHT JSON data"));
 else { RandomizeStartingBuild();SetNotice(TEXT("LAST LIGHT - Unreal Engine migration. Choose 50 skills."));}
}

bool ULLGameInstance::LoadDatabase()
{
 AllSkills.Empty();Mobs.Empty();Bosses.Empty();Items.Empty();Equipped.Empty();
 bool ok=LoadSkillArray(TEXT("skills.json"))
  && LoadEnemyArray(TEXT("mobs.json"),false)
  && LoadEnemyArray(TEXT("minibosses.json"),true)
  && LoadGear(TEXT("starter_gear.json"));
 UE_LOG(LogTemp,Display,TEXT("LL_DATABASE: skills=%d enemies=%d minibosses=%d loaded=%s"),
  AllSkills.Num(),Mobs.Num(),Bosses.Num(),ok?TEXT("YES"):TEXT("NO"));
 return ok&&AllSkills.Num()>=100&&Mobs.Num()>=30&&Bosses.Num()==9;
}

bool ULLGameInstance::LoadSkillArray(const FString& File)
{
 auto Entries=LL::ArrayFile(File);
 for(const auto& E:Entries)
 {
  auto O=E->AsObject();if(!O.IsValid())continue;
  FLLSkill Skill;Skill.Id=LL::Str(O,TEXT("id"));Skill.Name=LL::Str(O,TEXT("name"));
  Skill.Display=LL::Str(O,TEXT("name_ru"),Skill.Name);
  Skill.Element=LL::Str(O,TEXT("element"),TEXT("physical"));
  Skill.Kind=LL::Str(O,TEXT("kind"),TEXT("strike"));
  Skill.Status=LL::Str(O,TEXT("status"));
  Skill.Power=LL::Int(O,TEXT("power"),8);
  Skill.Cost=LL::Int(O,TEXT("cost"),1);
  Skill.bSignature=O->GetBoolField(TEXT("signature"));
  if(!Skill.Id.IsEmpty())AllSkills.Add(MoveTemp(Skill));
 }
 return AllSkills.Num()>=100;
}

bool ULLGameInstance::LoadEnemyArray(const FString& File, bool bBossRecords)
{
 auto Entries=LL::ArrayFile(File);
 for(const auto& E:Entries)
 {
  auto O=E->AsObject();if(!O.IsValid())continue;
  FLLEnemy Enemy;Enemy.Id=LL::Str(O,TEXT("id"));
  Enemy.Name=LL::Str(O,TEXT("name"));Enemy.Shape=LL::Str(O,TEXT("shape"),TEXT("knight"));
  Enemy.Element=LL::Str(O,TEXT("element"));Enemy.Weakness=LL::Str(O,TEXT("weakness"));
  Enemy.HexColor=LL::Str(O,TEXT("color"),TEXT("d9b98c"));
  Enemy.HP=LL::Int(O,TEXT("hp"),40);Enemy.Damage=LL::Int(O,TEXT("damage"),8);
  Enemy.Gate=LL::Int(O,TEXT("gate_level"),100);
  Enemy.Attacks=LL::Strings(O,TEXT("attacks"));
  Enemy.bBoss=bBossRecords;
  if(!Enemy.Id.IsEmpty())(bBossRecords?Bosses:Mobs).Add(MoveTemp(Enemy));
 }
 return bBossRecords?Bosses.Num()==9:Mobs.Num()>=30;
}

bool ULLGameInstance::LoadGear(const FString& File)
{
 auto Entries=LL::ArrayFile(File);
 int32 I=0;
 for(const auto& E:Entries)
 {
  auto O=E->AsObject();if(!O.IsValid())continue;
  FLLItem Item;Item.Id=FString::Printf(TEXT("starter_%d"),I++);
  Item.Name=LL::Str(O,TEXT("name"));Item.Slot=LL::Str(O,TEXT("slot"));
  Item.Rarity=LL::Str(O,TEXT("rarity"),TEXT("rare"));
  Item.Grade=LL::Int(O,TEXT("grade"),1);Item.Skills=LL::Strings(O,TEXT("skills"));
  Items.Add(Item);Equipped.Add(Item.Slot,Item.Id);
 }
 return Items.Num()==3;
}

const FLLSkill* ULLGameInstance::FindSkill(const FString& Id) const
{
 for(const FLLSkill& S:AllSkills)if(S.Id==Id)return &S;
 for(const FLLSkill& S:Innate)if(S.Id==Id)return &S;
 return nullptr;
}
const FLLItem* ULLGameInstance::FindItem(const FString& Id) const
{
 for(const FLLItem& Item:Items)if(Item.Id==Id)return &Item;
 return nullptr;
}

TArray<FLLSkill> ULLGameInstance::GetSkills() const
{
 TArray<FLLSkill> Result;
 FLLSkill Basic;Basic.Id=TEXT("basic_strike");Basic.Name=TEXT("Traveler's Strike");
 Basic.Display=TEXT("Обычный удар");Basic.Kind=TEXT("strike");Basic.Element=TEXT("physical");
 Basic.Power=8;Result.Add(Basic);
 FLLSkill Guard;Guard.Id=TEXT("basic_guard");Guard.Name=TEXT("Last Stand");
 Guard.Display=TEXT("Защита");Guard.Kind=TEXT("guard");Guard.Element=TEXT("physical");
 Guard.Power=0;Result.Add(Guard);
 Result.Append(Innate);
 for(const FString& Slot:{FString(TEXT("weapon")),FString(TEXT("armor")),FString(TEXT("charm"))})
 {
  const FString* EquippedId=Equipped.Find(Slot);
  if(!EquippedId)continue;
  const FLLItem* Item=FindItem(*EquippedId);if(!Item)continue;
  for(const FString& SkillId:Item->Skills)
  {
   if(Item->Sealed.Contains(SkillId))continue;
   const FLLSkill* Original=FindSkill(SkillId);
   if(Original)
   {
    FLLSkill Skill=*Original;
    Skill.Id=Item->Id+TEXT("_")+Original->Id;
    Result.Add(MoveTemp(Skill));
   }
  }
 }
 return Result;
}

void ULLGameInstance::RandomizeStartingBuild()
{
 TArray<FString> Pool;
 for(const FLLSkill& Skill:AllSkills)Pool.Add(Skill.Id);
 for(int32 I=Pool.Num()-1;I>0;--I)Pool.Swap(I,Random.RandRange(0,I));
 ChosenStart=Pool;
 ChosenStart.SetNum(FMath::Min(50,Pool.Num()));
}

bool ULLGameInstance::ToggleStartSkill(int32 Index)
{
 if(!AllSkills.IsValidIndex(Index))return false;
 const FString& Id=AllSkills[Index].Id;
 if(ChosenStart.Contains(Id))ChosenStart.Remove(Id);
 else if(ChosenStart.Num()<50)ChosenStart.Add(Id);
 else {SetNotice(TEXT("Only 50 starting skills. Remove one before adding another."));return false;}
 return true;
}

bool ULLGameInstance::StartRun(UWorld* World)
{
 if(!World||ChosenStart.Num()!=50)return false;
 // Reset equipment and loot from a previous run before copying the new starting build.
 Items.Empty();Equipped.Empty();
 if(!LoadGear(TEXT("starter_gear.json")))return false;
 Innate.Empty();Snapshot.Empty();DefeatedIds.Empty();DefeatedBossIds.Empty();BattleLog.Empty();
 for(const FString& Id:ChosenStart)
 {
  const FLLSkill* Skill=FindSkill(Id);
  if(Skill){Innate.Add(*Skill);Snapshot.Add(*Skill);}
 }
 if(Innate.Num()!=50)return false;
 Level=100;HeroHP=120;Kills=0;XP=0;EvolutionSlots=0;BossesWon=0;
 LossesCompleted=0;PendingLoss=0;CurrentBiome=0;SkillPage=0;
 SavedWorldLocation=FVector(160,0,165);bHasStarted=true;bInBattle=false;bDemo=true;
 SetNotice(TEXT("100 -> 0. Every victory gives loot but accelerates the descent."));
 Phase=ELLPhase::Exploration;
 UGameplayStatics::OpenLevel(World,FName(TEXT("/Game/Maps/Explore")));
 return true;
}

int32 ULLGameInstance::MaxHP() const
{
 return FMath::RoundToInt(FMath::Lerp(70.0,120.0,Level/100.0));
}
float ULLGameInstance::XPToLose(int32 CurrentLevel) const
{
 return 300.0f*FMath::Pow(.96f,100-FMath::Clamp(CurrentLevel,0,100));
}
void ULLGameInstance::PushLog(const FString& Text)
{
 BattleLog.Add(Text);
 while(BattleLog.Num()>5)BattleLog.RemoveAt(0);
 Notice=Text;
 UE_LOG(LogTemp,Display,TEXT("LL_EVENT: %s"),*Text);
}
void ULLGameInstance::SetNotice(const FString& Text){Notice=Text;}
void ULLGameInstance::CycleTimingAssist()
{
 if(Assist<.04f)Assist=.08f;
 else if(Assist<.12f)Assist=.16f;
 else Assist=0.f;
 const TCHAR* Label=Assist>.12f?TEXT("wide"):(Assist>.03f?TEXT("moderate"):TEXT("normal"));
 SetNotice(FString::Printf(TEXT("Timing assist: %s (F2 to change)"),Label));
 if(bHasStarted&&!bInBattle)SaveRun();
}

FString ULLGameInstance::BiomeName(int32 Biome) const
{
 static const TCHAR* Names[]={TEXT("Пепельный сад"),TEXT("Стеклянные льды"),
  TEXT("Забытый храм"),TEXT("Вечное море"),TEXT("Зеркальная цитадель")};
 return Names[FMath::Clamp(Biome,0,4)];
}
FString ULLGameInstance::EnemyDisplay() const
{
 if(bFinal)return TEXT("Бывшее Я");
 return ActiveEnemy.Name;
}
bool ULLGameInstance::IsMinibossUnlocked(int32 Index) const
{
 return Bosses.IsValidIndex(Index)&&Level<=Bosses[Index].Gate
  && !DefeatedBossIds.Contains(Bosses[Index].Id);
}

void ULLGameInstance::Engage(UWorld* World,int32 EnemyIndex,bool bIsBoss,
 bool bIsElite,bool bIsFinal,const FVector& From)
{
 if(!World||bInBattle||!bHasStarted)return;
 if(bIsFinal && Level!=0){SetNotice(TEXT("The mirror only opens at level zero."));return;}
 if(bIsBoss && !IsMinibossUnlocked(EnemyIndex)){SetNotice(TEXT("Boss locked until a lower level."));return;}
 if(!bIsFinal && !(bIsBoss?Bosses:Mobs).IsValidIndex(EnemyIndex))return;
 if(From!=FVector::ZeroVector)SavedWorldLocation=From;
 bFinal=bIsFinal;bElite=bIsElite;bInBattle=true;
 CurrentEnemyIndex=EnemyIndex;
 ActiveEnemy=bFinal?FLLEnemy():(bIsBoss?Bosses[EnemyIndex]:Mobs[EnemyIndex]);
 if(bFinal)
 {
  ActiveEnemy.Id=TEXT("MIRROR");ActiveEnemy.Name=TEXT("The Former Self");
  ActiveEnemy.Shape=TEXT("mirror");ActiveEnemy.Element=TEXT("dark");
  ActiveEnemy.HexColor=TEXT("edd49b");ActiveEnemy.HP=160;ActiveEnemy.Damage=16;
  ActiveEnemy.Attacks={TEXT("Echo Slash"),TEXT("Fallen Skills"),TEXT("Final Judgment")};
  ActiveEnemy.bBoss=true;
 }
 StartBattle();
 UGameplayStatics::OpenLevel(World,FName(TEXT("/Game/Maps/Arena")));
}

void ULLGameInstance::StartBattle()
{
 Phase=ELLPhase::BattlePlayer;AP=2;Focus=0;Turn=1;
 bGuarded=false;Timing=0;EnemyStatus.Empty();HeroStatus.Empty();
 EnemyStatusTurns=0;HeroStatusTurns=0;AttackIndex=0;AttacksRemaining=0;
 const float Progress=1.0f-Level/100.0f;
 int32 Hp=FMath::RoundToInt(ActiveEnemy.HP*(1.0f+Progress*.4f));
 EnemyDamage=FMath::RoundToInt(ActiveEnemy.Damage*FMath::Lerp(1.0f,1.6f,Progress));
 if(bElite){Hp=FMath::RoundToInt(Hp*1.6f);EnemyDamage=FMath::RoundToInt(EnemyDamage*1.3f);}
 if(bFinal)
 {
  int32 TotalPower=0;for(const FLLSkill& S:Snapshot)TotalPower+=S.Power;
  float Mean=Snapshot.IsEmpty()?12.0f:float(TotalPower)/Snapshot.Num();
  Hp=FMath::RoundToInt(120+Mean*5);
  EnemyDamage=FMath::RoundToInt(7+Mean*.8f);
 }
 EnemyHP=EnemyMaxHP=FMath::Max(1,Hp);
 PushLog(TEXT("АРЕНА: ")+EnemyDisplay()+TEXT(" — ваш ход"));
}

int32 ULLGameInstance::GetDamageFor(const FLLSkill& Skill,float TimingBonus) const
{
 float Mult=Skill.Element==ActiveEnemy.Weakness?1.4f:1.0f;
 if(Skill.Element==TEXT("lightning")&&EnemyStatus==TEXT("wet"))Mult*=1.5f;
 if(Skill.Element==TEXT("fire")&&EnemyStatus==TEXT("freeze"))Mult*=1.5f;
 if(Skill.Element==TEXT("wind")&&EnemyStatus==TEXT("burn"))Mult*=1.35f;
 float LevelPenalty=FMath::Lerp(.82f,1.0f,Level/100.0f);
 int32 Damage=FMath::Max(1,FMath::RoundToInt(
  (Skill.Power*LevelPenalty+Focus*1.4f)*Mult));
 if(Skill.Kind==TEXT("curse"))Damage=FMath::Max(1,FMath::RoundToInt(Damage*.70f));
 return FMath::Max(1,FMath::RoundToInt(Damage*FMath::Clamp(TimingBonus,1.0f,1.25f)));
}
void ULLGameInstance::BasicStrike()
{
 const auto Skills=GetSkills();if(Skills.Num()>0)BeginAttack(Skills[0]);
}
void ULLGameInstance::BasicGuard()
{
 const auto Skills=GetSkills();if(Skills.Num()>1)ApplySkill(Skills[1],1.0f);
}
void ULLGameInstance::UseSkill(int32 ListIndex)
{
 if(Phase!=ELLPhase::BattlePlayer || AP<=0)return;
 const TArray<FLLSkill> Skills=GetSkills();
 if(!Skills.IsValidIndex(ListIndex))return;
 const FLLSkill& Skill=Skills[ListIndex];
 if(Skill.Kind==TEXT("guard")||Skill.Kind==TEXT("heal"))ApplySkill(Skill,1.0f);
 else BeginAttack(Skill);
}
void ULLGameInstance::BeginAttack(const FLLSkill& Skill)
{
 if(Phase!=ELLPhase::BattlePlayer||AP<=0)return;
 PendingAttack=Skill;Phase=ELLPhase::AttackTiming;Timing=0;bPerfectAttack=false;
 SetNotice(TEXT("SPACE on golden marker 67% for bonus; ENTER for normal attack"));
}
void ULLGameInstance::CompleteAttack(bool bPerfect)
{
 if(Phase!=ELLPhase::AttackTiming)return;
 const float Exact=1.05f*.67f;
 bPerfectAttack=bPerfect&&FMath::Abs(Timing-Exact)<=(.145f+Assist*.45f);
 Phase=ELLPhase::BattlePlayer;
 ApplySkill(PendingAttack,bPerfectAttack?1.25f:1.0f);
}
void ULLGameInstance::ApplySkill(const FLLSkill& Skill,float TimingBonus)
{
 if(Phase!=ELLPhase::BattlePlayer || AP<=0 || EnemyHP<=0)return;
 if(Skill.Kind==TEXT("heal"))
 {
  const int32 Amount=FMath::Min(MaxHP()-HeroHP,Skill.Power+5);
  HeroHP+=Amount;PushLog(Skill.Display+FString::Printf(TEXT(": +%d HP"),Amount));
 }
 else if(Skill.Kind==TEXT("guard"))
 {
  bGuarded=true;Focus=FMath::Min(10,Focus+1);
  PushLog(Skill.Display+TEXT(": защита усилена"));
 }
 else
 {
  const int32 Damage=GetDamageFor(Skill,TimingBonus);
  EnemyHP=FMath::Max(0,EnemyHP-Damage);
  Focus=0;PushLog(Skill.Display+FString::Printf(TEXT(": -%d HP%s"),
   Damage,TimingBonus>1?TEXT(" PERFECT"):TEXT("")));
  const float Chance=Random.FRand();
  if(Skill.Kind==TEXT("curse")||Skill.Kind==TEXT("dash")||Chance<.20f)
  {EnemyStatus=Skill.Status;EnemyStatusTurns=2;}
 }
 --AP;
 if(EnemyHP<=0){Win(GetWorld());return;}
 if(AP<=0)EndPlayerTurn();
}

void ULLGameInstance::EndPlayerTurn()
{
 if(Phase!=ELLPhase::BattlePlayer)return;
 AttacksRemaining=(Level<12?3:(Level<45?2:1));
 if(bFinal)AttacksRemaining=EnemyHP<float(EnemyMaxHP)*.33f?3:2;
 if(EnemyStatus==TEXT("freeze"))AttacksRemaining=FMath::Max(1,AttacksRemaining-1);
 AdvanceEnemyTurn();
}
const FLLSkill* ULLGameInstance::GetMirrorPatternSkill(int32 AttackNumber) const
{
 if(Snapshot.IsEmpty())return nullptr;
 // Opening strike recalls the very first chosen skill; later strikes use a
 // readable six-skill repertoire instead of cycling through all 50 skills.
 if(AttackNumber<=0)return &Snapshot[0];
 TArray<int32> Ranked;
 Ranked.Reserve(Snapshot.Num());
 for(int32 I=0;I<Snapshot.Num();++I)Ranked.Add(I);
 Ranked.Sort([this](int32 A,int32 B)
 {
  if(Snapshot[A].Power!=Snapshot[B].Power)return Snapshot[A].Power>Snapshot[B].Power;
  return Snapshot[A].Id.Compare(Snapshot[B].Id)<0;
 });
 const int32 RepertoireSize=FMath::Min(6,Ranked.Num());
 return &Snapshot[Ranked[(AttackNumber-1)%RepertoireSize]];
}
FString ULLGameInstance::NextAttackName()
{
 if(bFinal)
 {
  if(const FLLSkill* Skill=GetMirrorPatternSkill(AttackIndex))
   return FString::Printf(TEXT("MIRROR / %s / %s"),*Skill->Display,*Skill->Element);
 }
 if(ActiveEnemy.Attacks.IsEmpty())return ActiveEnemy.Name+TEXT(" / Strike");
 return ActiveEnemy.Name+TEXT(" / ")+ActiveEnemy.Attacks[AttackIndex%ActiveEnemy.Attacks.Num()];
}
void ULLGameInstance::PrepareEnemyAttack()
{
 AttackLabel=NextAttackName();
 int32 Sum=0;
 const FString Key=AttackLabel+ActiveEnemy.Id;
 for(TCHAR Ch:Key)Sum+=static_cast<int32>(Ch);
 const int32 Type=Sum%3;
 static const float Durs[]={1.45f,1.65f,1.85f};
 static const float Fracs[]={.63f,.72f,.81f};
 static const float Mults[]={.90f,1.0f,1.18f};
 AttackDuration=Durs[Type];AttackFraction=Fracs[Type];AttackMultiplier=Mults[Type];
 if(bFinal)
 {
  if(const FLLSkill* Skill=GetMirrorPatternSkill(AttackIndex))
  {
   float SumPower=0.f;
   for(const FLLSkill& Original:Snapshot)SumPower+=Original.Power;
   const float Average=SumPower/FMath::Max(1,Snapshot.Num());
   AttackMultiplier*=FMath::Clamp(1.0f+.015f*(Skill->Power-Average),.75f,1.30f);
  }
 }
 ++AttackIndex;
 Timing=0;
 Phase=ELLPhase::EnemyTiming;
 SetNotice(TEXT("Атака врага: Q парировать / E уклониться на золотой метке"));
}
void ULLGameInstance::AdvanceEnemyTurn()
{
 if(AttacksRemaining<=0)
 {
  if(HeroStatusTurns>0){HeroHP=FMath::Max(0,HeroHP-3);--HeroStatusTurns;}
  if(EnemyStatusTurns>0)
  {
   if(EnemyStatus==TEXT("poison")||EnemyStatus==TEXT("burn"))
    EnemyHP=FMath::Max(0,EnemyHP-4);
   --EnemyStatusTurns;
  }
  if(HeroHP<=0){Lose();return;}
  if(EnemyHP<=0){Win(GetWorld());return;}
  AP=2;++Turn;Phase=ELLPhase::BattlePlayer;PushLog(TEXT("ТВОЙ ХОД — 2 действия"));
  return;
 }
 PrepareEnemyAttack();
}
void ULLGameInstance::EnemyTurn(){if(Phase==ELLPhase::BattlePlayer)EndPlayerTurn();}
void ULLGameInstance::TickCombat(float DeltaSeconds)
{
 if(Phase==ELLPhase::AttackTiming)
 {
  Timing+=DeltaSeconds;
  if(Timing>=1.05f)CompleteAttack(false);
 }
 else if(Phase==ELLPhase::EnemyTiming)
 {
  Timing+=DeltaSeconds;
  if(Timing>=AttackDuration)ResolveAttack(false,false);
 }
}
void ULLGameInstance::Defend(bool bParry)
{
 if(Phase!=ELLPhase::EnemyTiming)return;
 ResolveAttack(true,bParry);
}
void ULLGameInstance::ResolveAttack(bool bAttempt,bool bParry)
{
 if(Phase!=ELLPhase::EnemyTiming)return;
 const float Target=AttackDuration*AttackFraction;
 const float T=FMath::Clamp(Level/100.0f,0.0f,1.0f);
 const float Window=FMath::Lerp(bParry?.17f:.26f,bParry?.27f:.38f,T)+Assist;
 const bool Success=bAttempt&&FMath::Abs(Timing-Target)<=Window*.5f;
 int32 Dealt=0;
 if(Success)
 {
  if(bParry)
  {
   Dealt=8+Focus*2;EnemyHP=FMath::Max(0,EnemyHP-Dealt);
   Focus=FMath::Min(10,Focus+2);
   PushLog(FString::Printf(TEXT("ПАРИРОВАНИЕ: контратака -%d"),Dealt));
  }
  else {Focus=FMath::Min(10,Focus+1);PushLog(TEXT("УКЛОНЕНИЕ: урон отменён"));}
 }
 else
 {
  int32 Damage=FMath::Max(1,FMath::RoundToInt(EnemyDamage*AttackMultiplier));
  if(bGuarded){Damage=FMath::Max(1,FMath::RoundToInt(Damage*.4f));bGuarded=false;}
  HeroHP=FMath::Max(0,HeroHP-Damage);
  PushLog(FString::Printf(TEXT("ПРОМАХ: -%d HP"),Damage));
 }
 --AttacksRemaining;
 if(HeroHP<=0){Lose();return;}
 if(EnemyHP<=0){Win(GetWorld());return;}
 AdvanceEnemyTurn();
}

void ULLGameInstance::GiveLoot()
{
 if(Items.Num()>=64||AllSkills.IsEmpty())return;
 FLLItem Item;Item.Id=FString::Printf(TEXT("loot_%d_%d"),Kills,Random.RandRange(100,999999));
 Item.Slot=(Random.RandRange(0,2)==0?TEXT("weapon"):
   (Random.RandRange(0,1)==0?TEXT("armor"):TEXT("charm")));
 Item.Rarity=bElite?TEXT("rare"):TEXT("common");
 Item.Grade=1;Item.Name=FString::Printf(TEXT("Echo %s %d"),*Item.Slot,Kills);
 int32 Count=bElite?Random.RandRange(2,3):Random.RandRange(1,3);
 for(int32 I=0;I<Count;++I)
 {
  const FString& SkillId=AllSkills[Random.RandRange(0,AllSkills.Num()-1)].Id;
  Item.Skills.AddUnique(SkillId);
 }
 if(bElite&&Item.Skills.Num()>1)Item.Sealed.Add(Item.Skills.Last());
 Items.Add(Item);
}
void ULLGameInstance::ApplyXP()
{
 if(Level<=0)return;
 if(bDemo){PendingLoss=FMath::Min(5,Level);return;}
 XP+=10.0f;
 int32 Losses=0;
 while(Level-Losses>0&&XP>=XPToLose(Level-Losses)&&Losses<10)
 {
  XP-=XPToLose(Level-Losses);
  ++Losses;
 }
 PendingLoss=Losses;
}
void ULLGameInstance::Win(UWorld* World)
{
 if(!World)return;
 if(bFinal){Phase=ELLPhase::FinalVictory;bInBattle=true;PushLog(TEXT("ПОБЕДА: ты пережил собственное прошлое"));return;}
 ++Kills;
 if(ActiveEnemy.bBoss&&!DefeatedBossIds.Contains(ActiveEnemy.Id))
 {DefeatedBossIds.Add(ActiveEnemy.Id);BossesWon=DefeatedBossIds.Num();}
 else if(!ActiveEnemy.bBoss)DefeatedIds.AddUnique(ActiveEnemy.Id);
 GiveLoot();ApplyXP();
 PushLog(FString::Printf(TEXT("ПОБЕДА: добыча, уровни к потере = %d"),PendingLoss));
 bInBattle=false;
 Phase=PendingLoss>0?ELLPhase::LossChoice:ELLPhase::Exploration;
 SkillPage=0;
 SaveRun();
 UGameplayStatics::OpenLevel(World,FName(TEXT("/Game/Maps/Explore")));
}
void ULLGameInstance::Lose()
{
 Phase=ELLPhase::Defeat;
 PushLog(TEXT("ПОРАЖЕНИЕ: ENTER повторить, ESC вернуться"));
}
void ULLGameInstance::Retry(UWorld* World)
{
 if(!World)return;
 HeroHP=MaxHP();StartBattle();bInBattle=true;
 UGameplayStatics::OpenLevel(World,FName(TEXT("/Game/Maps/Arena")));
}
void ULLGameInstance::ReturnToExploration(UWorld* World)
{
 if(!World)return;
 HeroHP=FMath::Max(1,HeroHP);bInBattle=false;Phase=ELLPhase::Exploration;
 UGameplayStatics::OpenLevel(World,FName(TEXT("/Game/Maps/Explore")));
}
void ULLGameInstance::LevelLoss(int32 SelectedIndex)
{
 if(Phase!=ELLPhase::LossChoice||PendingLoss<=0||Level<=0)return;
 if(Innate.Num()>=2)
 {
  const int32 Index=FMath::Clamp(SelectedIndex,0,Innate.Num()-1);
  const FString Removed=Innate[Index].Display;Innate.RemoveAt(Index);
  const int32 RandomIndex=Random.RandRange(0,Innate.Num()-1);
  Innate.RemoveAt(RandomIndex);
  const FLLSkill& RandomNew=AllSkills[Random.RandRange(0,AllSkills.Num()-1)];
  Innate.Add(RandomNew);
  PushLog(TEXT("РАСПАД: ")+Removed+TEXT(" / -2 +1"));
 }
 else PushLog(TEXT("РАСПАД: набор пуст, пропуск без штрафа"));
 --Level;--PendingLoss;++LossesCompleted;
 HeroHP=FMath::Min(HeroHP,MaxHP());
 if(Level%5==0)++EvolutionSlots;
 if(PendingLoss<=0)CompleteLoss(GetWorld());
 else SaveRun(); // Preserve exact pending level losses if the player quits.
}
void ULLGameInstance::CompleteLoss(UWorld* World)
{
 PendingLoss=0;Phase=ELLPhase::Exploration;
 SaveRun();
 SetNotice(Level==0?TEXT("УРОВЕНЬ НОЛЬ: войди в зеркало (K)"):
  FString::Printf(TEXT("УРОВЕНЬ %d: ищи следующее сражение"),Level));
}
bool ULLGameInstance::Equip(int32 Index)
{
 if(!Items.IsValidIndex(Index))return false;
 Equipped.Add(Items[Index].Slot,Items[Index].Id);
 PushLog(TEXT("НАДЕТО: ")+Items[Index].Name);
 SaveRun();return true;
}
float ULLGameInstance::ForgeBreakChance(int32 A,int32 B) const
{
 if(!Items.IsValidIndex(A)||!Items.IsValidIndex(B)||A==B)return -1.0f;
 const int32 Grade=FMath::Max(Items[A].Grade,Items[B].Grade);
 if(Grade>=6)return -1.0f;
 return FMath::Min(.80f,.10f+.12f*(Grade-1));
}
void ULLGameInstance::CancelForge()
{
 SelectedItem=-1;
 PendingForgeIndex=-1;
}
bool ULLGameInstance::QueueForge(int32 Index)
{
 if(Phase!=ELLPhase::Forge||!Items.IsValidIndex(Index))return false;
 if(!Items.IsValidIndex(SelectedItem))
 {
  SelectedItem=Index;
  PendingForgeIndex=-1;
  SetNotice(TEXT("Выбери второй предмет. Слияние пока не выполняется."));
  return true;
 }
 const float Risk=ForgeBreakChance(SelectedItem,Index);
 if(Risk<0.f)
 {
  SetNotice(TEXT("Выбери другой предмет. Максимальный грейд G6."));
  return false;
 }
 PendingForgeIndex=Index;
 SetNotice(FString::Printf(TEXT("Риск поломки %.0f%%. Enter — подтвердить, Esc — отменить."),
  Risk*100.0f));
 return true;
}
bool ULLGameInstance::ConfirmForge()
{
 if(Phase!=ELLPhase::Forge||ForgeBreakChance(SelectedItem,PendingForgeIndex)<0.f)
  return false;
 const int32 A=SelectedItem,B=PendingForgeIndex;
 CancelForge();
 return Forge(A,B);
}
bool ULLGameInstance::Forge(int32 A,int32 B)
{
 if(!Items.IsValidIndex(A)||!Items.IsValidIndex(B)||A==B)return false;
 const FLLItem First=Items[A],Second=Items[B];
 const int32 Grade=FMath::Max(First.Grade,Second.Grade);
 if(Grade>=6){PushLog(TEXT("КУЗНИЦА: максимальный грейд 6"));return false;}
 const float Break=ForgeBreakChance(A,B); // Preview and RNG use exactly the same formula.
 if(Random.FRand()<Break)
 {
  const int32 ToDelete=Random.RandBool()?A:B;
  PushLog(FString::Printf(TEXT("ПОЛОМКА: %s (%.0f%%)"),*Items[ToDelete].Name,Break*100));
  const FLLItem Lost=Items[ToDelete];
  if(const FString* EquippedId=Equipped.Find(Lost.Slot))
   if(*EquippedId==Lost.Id)Equipped.Remove(Lost.Slot);
  Items.RemoveAt(ToDelete);SaveRun();return false;
 }
 FLLItem New=First;
 New.Id=FString::Printf(TEXT("fusion_%d_%d"),Kills,Random.RandRange(1,999999));
 New.Grade=Grade+1;New.Rarity=TEXT("symbiote");
 New.Name=FString::Printf(TEXT("Симбиот G%d"),New.Grade);
 for(const FString& S:Second.Skills)New.Skills.AddUnique(S);
 while(New.Skills.Num()>3)New.Skills.RemoveAt(New.Skills.Num()-1);
 New.Sealed.Empty();
 for(const FLLItem& Lost:{First,Second})
 {
  if(const FString* EquippedId=Equipped.Find(Lost.Slot))
   if(*EquippedId==Lost.Id)Equipped.Remove(Lost.Slot);
 }
 Items.RemoveAt(FMath::Max(A,B));Items.RemoveAt(FMath::Min(A,B));Items.Add(New);
 Equipped.Add(New.Slot,New.Id);
 PushLog(TEXT("КУЗНИЦА: создан ")+New.Name);SaveRun();return true;
}
bool ULLGameInstance::Evolve(int32 A,int32 B)
{
 if(EvolutionSlots<=0||!Innate.IsValidIndex(A)||!Innate.IsValidIndex(B)||A==B)return false;
 FLLSkill First=Innate[A],Second=Innate[B];
 if(FMath::Max(First.Depth,Second.Depth)>=4)return false;
 FLLSkill Merged;
 Merged.Id=FString::Printf(TEXT("evo_%d_%d"),LossesCompleted,Random.RandRange(1,999999));
 Merged.Name=First.Name+TEXT(" + ")+Second.Name;
 Merged.Display=First.Display+TEXT(" + ")+Second.Display;
 Merged.Kind=First.Kind;Merged.Element=First.Element;
 Merged.Power=FMath::Min(40,First.Power+Second.Power/2);
 Merged.Status=Second.Status;Merged.Depth=FMath::Max(First.Depth,Second.Depth)+1;
 Innate.RemoveAt(FMath::Max(A,B));Innate.RemoveAt(FMath::Min(A,B));
 Innate.Add(Merged);--EvolutionSlots;
 PushLog(TEXT("СИМБИОЗ: ")+Merged.Display);SaveRun();return true;
}

bool ULLGameInstance::SaveRun() const
{
 if(!bHasStarted)return false;
 TSharedPtr<FJsonObject> Root=MakeShared<FJsonObject>();
 Root->SetNumberField(TEXT("version"),2);
 Root->SetNumberField(TEXT("level"),Level);Root->SetNumberField(TEXT("hp"),HeroHP);
 Root->SetNumberField(TEXT("kills"),Kills);Root->SetNumberField(TEXT("xp"),XP);
 Root->SetNumberField(TEXT("evolution_slots"),EvolutionSlots);
 Root->SetNumberField(TEXT("pending_loss"),PendingLoss);
 Root->SetNumberField(TEXT("losses_completed"),LossesCompleted);
 Root->SetBoolField(TEXT("demo"),bDemo);
 Root->SetNumberField(TEXT("timing_assist"),Assist);
 Root->SetNumberField(TEXT("biome"),CurrentBiome);
 Root->SetArrayField(TEXT("chosen_start"),LL::Values(ChosenStart));
 Root->SetArrayField(TEXT("defeated_ids"),LL::Values(DefeatedIds));
 Root->SetArrayField(TEXT("defeated_boss_ids"),LL::Values(DefeatedBossIds));
 Root->SetArrayField(TEXT("snapshot"),LL::Values(ChosenStart));
 TArray<TSharedPtr<FJsonValue>> InnateValues;
 for(const FLLSkill& S:Innate)
 {
  auto O=MakeShared<FJsonObject>();
  O->SetStringField(TEXT("id"),S.Id);O->SetStringField(TEXT("name"),S.Name);
  O->SetStringField(TEXT("display"),S.Display);O->SetStringField(TEXT("element"),S.Element);
  O->SetStringField(TEXT("kind"),S.Kind);O->SetStringField(TEXT("status"),S.Status);
  O->SetNumberField(TEXT("power"),S.Power);O->SetNumberField(TEXT("depth"),S.Depth);
  InnateValues.Add(MakeShared<FJsonValueObject>(O));
 }
 Root->SetArrayField(TEXT("innate"),InnateValues);
 TArray<TSharedPtr<FJsonValue>> ItemValues;
 for(const FLLItem& I:Items)
 {
  auto O=MakeShared<FJsonObject>();O->SetStringField(TEXT("id"),I.Id);
  O->SetStringField(TEXT("name"),I.Name);O->SetStringField(TEXT("slot"),I.Slot);
  O->SetStringField(TEXT("rarity"),I.Rarity);O->SetNumberField(TEXT("grade"),I.Grade);
  O->SetArrayField(TEXT("skills"),LL::Values(I.Skills));
  O->SetArrayField(TEXT("sealed"),LL::Values(I.Sealed));
  ItemValues.Add(MakeShared<FJsonValueObject>(O));
 }
 Root->SetArrayField(TEXT("items"),ItemValues);
 auto E=MakeShared<FJsonObject>();
 for(const auto& Pair:Equipped)E->SetStringField(Pair.Key,Pair.Value);
 Root->SetObjectField(TEXT("equipped"),E);
 auto Pos=MakeShared<FJsonObject>();
 Pos->SetNumberField(TEXT("x"),SavedWorldLocation.X);
 Pos->SetNumberField(TEXT("y"),SavedWorldLocation.Y);
 Pos->SetNumberField(TEXT("z"),SavedWorldLocation.Z);
 Root->SetObjectField(TEXT("location"),Pos);
 FString Text;const auto Writer=TJsonWriterFactory<>::Create(&Text);
 if(!FJsonSerializer::Serialize(Root.ToSharedRef(),Writer))return false;
 const FString Dir=FPaths::Combine(FPaths::ProjectSavedDir(),TEXT("SaveGames"));
 IFileManager::Get().MakeDirectory(*Dir,true);
 const FString Path=FPaths::Combine(Dir,TEXT("LastLightUE.json"));
 return FFileHelper::SaveStringToFile(Text,*Path,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}

bool ULLGameInstance::LoadRun(UWorld* World)
{
 if(!World)return false;
 FString Text;
 const FString Path=FPaths::Combine(FPaths::ProjectSavedDir(),TEXT("SaveGames"),TEXT("LastLightUE.json"));
 if(!FFileHelper::LoadFileToString(Text,*Path))return false;
 TSharedPtr<FJsonObject> O;
 if(!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text),O)||!O.IsValid())return false;
 Level=LL::Int(O,TEXT("level"),100);HeroHP=LL::Int(O,TEXT("hp"),120);
 Kills=LL::Int(O,TEXT("kills"));XP=static_cast<float>(O->GetNumberField(TEXT("xp")));
 EvolutionSlots=LL::Int(O,TEXT("evolution_slots"));
 PendingLoss=FMath::Clamp(LL::Int(O,TEXT("pending_loss")),0,100);
 LossesCompleted=LL::Int(O,TEXT("losses_completed"));
 SkillPage=0;
 CurrentBiome=LL::Int(O,TEXT("biome"));bDemo=O->GetBoolField(TEXT("demo"));
 double SavedAssist=0.0;
 O->TryGetNumberField(TEXT("timing_assist"),SavedAssist);
 Assist=FMath::Clamp(static_cast<float>(SavedAssist),0.0f,.16f);
 ChosenStart=LL::Strings(O,TEXT("chosen_start"));
 DefeatedIds=LL::Strings(O,TEXT("defeated_ids"));
 DefeatedBossIds=LL::Strings(O,TEXT("defeated_boss_ids"));
 BossesWon=DefeatedBossIds.Num();
 const auto InnateValues=O->GetArrayField(TEXT("innate"));Innate.Empty();
 for(const auto& Value:InnateValues)
 {
  auto Entry=Value->AsObject();if(!Entry.IsValid())continue;
  FLLSkill S;S.Id=LL::Str(Entry,TEXT("id"));S.Name=LL::Str(Entry,TEXT("name"));
  S.Display=LL::Str(Entry,TEXT("display"));S.Element=LL::Str(Entry,TEXT("element"));
  S.Kind=LL::Str(Entry,TEXT("kind"));S.Status=LL::Str(Entry,TEXT("status"));
  S.Power=LL::Int(Entry,TEXT("power"));S.Depth=LL::Int(Entry,TEXT("depth"));
  Innate.Add(S);
 }
 Snapshot.Empty();for(const FString& Id:ChosenStart)
 {if(const FLLSkill* S=FindSkill(Id))Snapshot.Add(*S);}
 Items.Empty();
 const auto ItemValues=O->GetArrayField(TEXT("items"));
 for(const auto& Value:ItemValues)
 {
  auto Entry=Value->AsObject();if(!Entry.IsValid())continue;
  FLLItem I;I.Id=LL::Str(Entry,TEXT("id"));I.Name=LL::Str(Entry,TEXT("name"));
  I.Slot=LL::Str(Entry,TEXT("slot"));I.Rarity=LL::Str(Entry,TEXT("rarity"));
  I.Grade=LL::Int(Entry,TEXT("grade"),1);
  I.Skills=LL::Strings(Entry,TEXT("skills"));I.Sealed=LL::Strings(Entry,TEXT("sealed"));Items.Add(I);
 }
 Equipped.Empty();
 auto Equ=O->GetObjectField(TEXT("equipped"));
 for(const TCHAR* Slot:{TEXT("weapon"),TEXT("armor"),TEXT("charm")})
 {
  FString Id;
  if(Equ->TryGetStringField(Slot,Id))Equipped.Add(FString(Slot),Id);
 }
 auto Pos=O->GetObjectField(TEXT("location"));
 SavedWorldLocation=FVector(Pos->GetNumberField(TEXT("x")),
  Pos->GetNumberField(TEXT("y")),Pos->GetNumberField(TEXT("z")));
 bHasStarted=true;bInBattle=false;
 Phase=PendingLoss>0?ELLPhase::LossChoice:ELLPhase::Exploration;
 SetNotice(PendingLoss>0?TEXT("Saved descent resumed: choose a skill to lose"):
   TEXT("Saved run loaded successfully"));
 UGameplayStatics::OpenLevel(World,FName(TEXT("/Game/Maps/Explore")));
 return true;
}
