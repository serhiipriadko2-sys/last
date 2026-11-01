#include "LLGameInstance.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "HAL/FileManager.h"
#include "Kismet/GameplayStatics.h"

namespace LLSave
{
 bool Number(const TSharedPtr<FJsonObject>& O,const TCHAR* Key,double& Out,double Min,double Max)
 {return O->TryGetNumberField(Key,Out)&&FMath::IsFinite(Out)&&Out>=Min&&Out<=Max;}
 bool Strings(const TSharedPtr<FJsonObject>& O,const TCHAR* Key,TArray<FString>& Out,int32 Limit)
 {
  const TArray<TSharedPtr<FJsonValue>>* V=nullptr;
  if(!O->TryGetArrayField(Key,V)||V->Num()>Limit)return false;
  for(const auto& E:*V){FString S;if(!E->TryGetString(S)||S.Len()>500)return false;Out.Add(S);}
  return true;
 }
 bool Item(const TSharedPtr<FJsonObject>& O,FLLItem& I)
 {
  double Grade=0;
  if(!O.IsValid()||!O->TryGetStringField(TEXT("id"),I.Id)||I.Id.IsEmpty()||I.Id.Len()>100||
   !O->TryGetStringField(TEXT("name"),I.Name)||I.Name.Len()>500||
   !O->TryGetStringField(TEXT("slot"),I.Slot)||
   !(I.Slot==TEXT("weapon")||I.Slot==TEXT("armor")||I.Slot==TEXT("charm"))||
   !Number(O,TEXT("grade"),Grade,1,6)||
   !Strings(O,TEXT("skills"),I.Skills,3)||!Strings(O,TEXT("sealed"),I.Sealed,3))return false;
  O->TryGetStringField(TEXT("rarity"),I.Rarity);I.Grade=int32(Grade);return true;
 }
}

FString ULLGameInstance::SavePath() const
{
 FString Slot;
 if(FParse::Value(FCommandLine::Get(),TEXT("LLQASave="),Slot))
 {
  Slot=FPaths::MakeValidFileName(FPaths::GetCleanFilename(Slot));
  return FPaths::Combine(FPaths::ProjectSavedDir(),TEXT("Verification/v08/SaveGames"),Slot+TEXT(".json"));
 }
 if(FParse::Param(FCommandLine::Get(),TEXT("LLCampaignQA")))
  return FPaths::Combine(FPaths::ProjectSavedDir(),TEXT("Verification/v08/SaveGames/campaign.json"));
 return FPaths::Combine(FPaths::ProjectSavedDir(),TEXT("SaveGames/LastLightUE.json"));
}

bool ULLGameInstance::SaveRun() const
{
 // A combat save would discard the enemy, AP and attack timer on load.
 // Keep the last complete exploration checkpoint instead.
 if(!bHasStarted||bInBattle||Phase==ELLPhase::Menu)return false;
 const FString Text=SerializeRun();if(Text.IsEmpty())return false;
 const FString Path=SavePath(),Temp=Path+TEXT(".tmp"),Backup=Path+TEXT(".bak");
 IFileManager& Files=IFileManager::Get();
 if(!Files.MakeDirectory(*FPaths::GetPath(Path),true))return false;
 if(!FFileHelper::SaveStringToFile(Text,*Temp,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))return false;
 if(Files.FileExists(*Path)&&Files.Copy(*Backup,*Path,true,true)!=COPY_OK)return false;
 return Files.Move(*Path,*Temp,true,true,false,true);
}

bool ULLGameInstance::RestoreRun(const FString& Text)
{
 if(Text.Len()>1024*1024)return false;
 TSharedPtr<FJsonObject> O;
 if(!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text),O)||!O.IsValid())return false;
 double Version=0,L=0,HP=0,K=0,X=0,Slots=0,Pending=0,Losses=0,Biome=0;
 if(!LLSave::Number(O,TEXT("version"),Version,2,3)||
  !LLSave::Number(O,TEXT("level"),L,0,100)||
  !LLSave::Number(O,TEXT("hp"),HP,1,10000)||
  !LLSave::Number(O,TEXT("kills"),K,0,100000)||
  !LLSave::Number(O,TEXT("xp"),X,0,10000000)||
  !LLSave::Number(O,TEXT("evolution_slots"),Slots,0,100)||
  !LLSave::Number(O,TEXT("pending_loss"),Pending,0,L)||
  !LLSave::Number(O,TEXT("losses_completed"),Losses,0,100)||
  !LLSave::Number(O,TEXT("biome"),Biome,0,4))return false;
 bool Demo=false;if(!O->TryGetBoolField(TEXT("demo"),Demo))return false;
 TArray<FString> Chosen,Defeated,BossIds;
 if(!LLSave::Strings(O,TEXT("chosen_start"),Chosen,100)||Chosen.IsEmpty()||
  !LLSave::Strings(O,TEXT("defeated_ids"),Defeated,30)||
  !LLSave::Strings(O,TEXT("defeated_boss_ids"),BossIds,9))return false;
 TArray<FLLSkill> Frozen,NewInnate;
 TSet<FString> Seen;
 for(const FString& Id:Chosen)
 {
  const FLLSkill* S=AllSkills.FindByPredicate([&](const FLLSkill& Skill){return Skill.Id==Id;});
  if(!S||Seen.Contains(Id))return false;
  Seen.Add(Id);Frozen.Add(*S);
 }
 const TArray<TSharedPtr<FJsonValue>>* Values=nullptr;
 if(!O->TryGetArrayField(TEXT("innate"),Values)||Values->Num()>100)return false;
 for(const auto& V:*Values)
 {
  const auto E=V->AsObject();if(!E.IsValid())return false;
  FLLSkill S;double Power=0,Depth=0,Cost=1;
  if(!E->TryGetStringField(TEXT("id"),S.Id)||S.Id.IsEmpty()||S.Id.Len()>100||
   !E->TryGetStringField(TEXT("name"),S.Name)||S.Name.Len()>2000||
   !E->TryGetStringField(TEXT("display"),S.Display)||S.Display.Len()>2000||
   !E->TryGetStringField(TEXT("element"),S.Element)||
   !E->TryGetStringField(TEXT("kind"),S.Kind)||
   !LLSave::Number(E,TEXT("power"),Power,0,1000)||
   !LLSave::Number(E,TEXT("depth"),Depth,0,8))return false;
  E->TryGetStringField(TEXT("status"),S.Status);
  if(Version>=3&&(!LLSave::Number(E,TEXT("cost"),Cost,1,2)||!LLSave::Strings(E,TEXT("parents"),S.Parents,512)))return false;
  S.Power=int32(Power);S.Depth=int32(Depth);S.Cost=int32(Cost);
  NewInnate.Add(S);
 }
 TArray<FLLItem> NewItems,FrozenGear;
 if(!O->TryGetArrayField(TEXT("items"),Values)||Values->Num()>128)return false;
 Seen.Empty();
 for(const auto& V:*Values)
 {
  FLLItem I;if(!LLSave::Item(V->AsObject(),I)||Seen.Contains(I.Id))return false;
  for(const FString& Id:I.Skills)if(!AllSkills.ContainsByPredicate([&](const FLLSkill& S){return S.Id==Id;}))return false;
  for(const FString& Id:I.Sealed)if(!I.Skills.Contains(Id))return false;
  Seen.Add(I.Id);NewItems.Add(I);
 }
 if(Version>=3)
 {
  if(!O->TryGetArrayField(TEXT("snapshot_gear"),Values)||Values->Num()!=3)return false;
  for(const auto& V:*Values){FLLItem I;if(!LLSave::Item(V->AsObject(),I))return false;FrozenGear.Add(I);}
 }
 else
 {
  // v2 did not store gear; its start equipment was fixed, so migrate from that table.
  ULLGameInstance* Defaults=NewObject<ULLGameInstance>();
  if(!Defaults->LoadDatabase())return false;
  FrozenGear=Defaults->Items;
 }
 const TSharedPtr<FJsonObject>* Equ=nullptr;const TSharedPtr<FJsonObject>* Pos=nullptr;
 if(!O->TryGetObjectField(TEXT("equipped"),Equ)||!O->TryGetObjectField(TEXT("location"),Pos))return false;
 TMap<FString,FString> NewEquipped;
 for(const auto& Pair:(*Equ)->Values)
 {
  FString Id;if(!Pair.Value->TryGetString(Id))return false;
  const FLLItem* I=NewItems.FindByPredicate([&](const FLLItem& Item){return Item.Id==Id&&Item.Slot==Pair.Key;});
  if(!I)return false;NewEquipped.Add(Pair.Key,Id);
 }
 double PX=0,PY=0,PZ=0;
 if(!LLSave::Number(*Pos,TEXT("x"),PX,-500,10500)||
  !LLSave::Number(*Pos,TEXT("y"),PY,-3000,3000)||
  !LLSave::Number(*Pos,TEXT("z"),PZ,-300,3000))return false;
 double NewAssist=0;O->TryGetNumberField(TEXT("timing_assist"),NewAssist);
 if(!FMath::IsFinite(NewAssist))return false;
 double Seed=1,NewFlasks=3;bool Won=false,Muted=false;
 TArray<int32> Chests,Shrines;
 if(Version>=3)
 {
  if(!LLSave::Number(O,TEXT("rng"),Seed,MIN_int32,MAX_int32)||
   !LLSave::Number(O,TEXT("flasks"),NewFlasks,0,3))return false;
  O->TryGetBoolField(TEXT("won"),Won);O->TryGetBoolField(TEXT("muted"),Muted);
  for(const TCHAR* Key:{TEXT("chests"),TEXT("shrines")})
  {
   if(!O->TryGetArrayField(Key,Values)||Values->Num()>5)return false;
   for(const auto& V:*Values)
   {
    double N=0;if(!V->TryGetNumber(N)||N<0||N>4||!FMath::IsFinite(N))return false;
    if(FCString::Strcmp(Key,TEXT("chests"))==0)Chests.AddUnique(int32(N));else Shrines.AddUnique(int32(N));
   }
  }
 }
 else Shrines.Add(0);
 if(Won&&L!=0)return false;
 // Commit only after every structure is validated. A corrupt file leaves the run untouched.
 Level=int32(L);HeroHP=FMath::Clamp(int32(HP),1,MaxHP());Kills=int32(K);XP=float(X);
 EvolutionSlots=int32(Slots);PendingLoss=int32(Pending);LossesCompleted=int32(Losses);
 CurrentBiome=int32(Biome);bDemo=Demo;Assist=FMath::Clamp(float(NewAssist),0.f,.16f);
 ChosenStart=MoveTemp(Chosen);Snapshot=MoveTemp(Frozen);SnapshotGear=MoveTemp(FrozenGear);
 Innate=MoveTemp(NewInnate);Items=MoveTemp(NewItems);Equipped=MoveTemp(NewEquipped);
 DefeatedIds=MoveTemp(Defeated);DefeatedBossIds=MoveTemp(BossIds);BossesWon=DefeatedBossIds.Num();
 OpenedChests=MoveTemp(Chests);DiscoveredShrines=MoveTemp(Shrines);
 SavedWorldLocation=FVector(PX,PY,FMath::Max(105.,PZ));
 Flasks=int32(NewFlasks);bWon=Won;bMuted=Muted;Random.Initialize(int32(Seed));
 SkillPage=ItemPage=0;CancelForge();bHasStarted=true;bInBattle=false;bFinal=false;
 Phase=bWon?ELLPhase::FinalVictory:(PendingLoss>0?ELLPhase::LossChoice:ELLPhase::Exploration);
 return true;
}

bool ULLGameInstance::LoadRun(UWorld* World)
{
 if(!World||bInBattle){SetNotice(TEXT("Загрузка доступна вне боя."));return false;}
 FString Text;const FString Path=SavePath();
 bool Loaded=FFileHelper::LoadFileToString(Text,*Path)&&RestoreRun(Text);
 if(!Loaded)Loaded=FFileHelper::LoadFileToString(Text,*(Path+TEXT(".bak")))&&RestoreRun(Text);
 if(!Loaded){SetNotice(TEXT("Нет корректного сохранения. Текущий забег сохранён."));return false;}
 SetNotice(TEXT("Сохранение загружено. Продолжай спуск."));
 UGameplayStatics::OpenLevel(World,FName(TEXT("/Game/Maps/Explore")));return true;
}
