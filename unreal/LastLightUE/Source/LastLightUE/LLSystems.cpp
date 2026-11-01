#include "LLGameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

TArray<int32> ULLGameInstance::FilteredSkills() const
{
 static const TCHAR* Elements[]={TEXT("all"),TEXT("fire"),TEXT("ice"),TEXT("lightning"),TEXT("poison"),TEXT("earth"),TEXT("water"),TEXT("wind"),TEXT("physical"),TEXT("dark"),TEXT("angelic")};
 static const TCHAR* Kinds[]={TEXT("all"),TEXT("strike"),TEXT("dash"),TEXT("curse"),TEXT("guard"),TEXT("heal")};
 TArray<int32> Result;
 for(int32 I=0;I<AllSkills.Num();++I)
 {
  const FLLSkill& S=AllSkills[I];
  if(MenuElement>0&&S.Element!=Elements[FMath::Clamp(MenuElement,0,10)])continue;
  if(MenuKind>0&&S.Kind!=Kinds[FMath::Clamp(MenuKind,0,5)])continue;
  if(!MenuSearch.IsEmpty()&&!S.Display.Contains(MenuSearch)&&!S.Name.Contains(MenuSearch)&&!S.Id.Contains(MenuSearch))continue;
  Result.Add(I);
 }
 if(MenuSort==1)Result.Sort([this](int32 A,int32 B){return AllSkills[A].Power>AllSkills[B].Power;});
 if(MenuSort==2)Result.Sort([this](int32 A,int32 B){return AllSkills[A].Power<AllSkills[B].Power;});
 return Result;
}

void ULLGameInstance::NextPage()
{
 if(Phase==ELLPhase::Menu)MenuPage=(MenuPage+1)%FMath::Max(1,FMath::DivideAndRoundUp(FilteredSkills().Num(),10));
 else if(Phase==ELLPhase::Inventory||Phase==ELLPhase::Forge)
  ItemPage=(ItemPage+1)%FMath::Max(1,FMath::DivideAndRoundUp(Items.Num(),5));
 else
 {
  const int32 Count=Phase==ELLPhase::BattlePlayer?GetSkills().Num()-2:Innate.Num();
  SkillPage=(SkillPage+1)%FMath::Max(1,FMath::DivideAndRoundUp(Count,5));
 }
}

void ULLGameInstance::Hunt(UWorld* World)
{
 if(Phase!=ELLPhase::Exploration||Mobs.IsEmpty())return;
 int32 Index=FMath::Clamp(CurrentBiome*6,0,Mobs.Num()-1);
 for(int32 Offset=0;Offset<Mobs.Num();++Offset)
 {
  const int32 Candidate=(Index+Offset)%Mobs.Num();
  if(!DefeatedIds.Contains(Mobs[Candidate].Id)){Index=Candidate;break;}
 }
 Engage(World,Index,false,false,false,SavedWorldLocation);
}

bool ULLGameInstance::Rest(int32 Zone)
{
 if(Phase!=ELLPhase::Exploration||bInBattle||Zone<0||Zone>4)return false;
 if(FVector::DistSquared2D(SavedWorldLocation,FVector(Zone*2000.f+100,-650,0))>450.f*450.f)return false;
 HeroHP=MaxHP();Flasks=3;
 DiscoveredShrines.AddUnique(Zone);
 Cue(TEXT("heal"));SaveRun();
 PushLog(TEXT("СВЯТИЛИЩЕ: здоровье и 3 фляги восстановлены. Путь запомнен."));
 return true;
}

bool ULLGameInstance::OpenChest(int32 Zone)
{
 if(Phase!=ELLPhase::Exploration||bInBattle||Zone<0||Zone>4||OpenedChests.Contains(Zone))return false;
 if(Items.Num()>=Balance.MaxInventory){SetNotice(TEXT("Сначала освободи место в кузнице."));return false;}
 if(FVector::DistSquared2D(SavedWorldLocation,FVector(Zone*2000.f+900,560,0))>300.f*300.f)return false;
 const bool PreviousElite=bElite;bElite=true;
 GiveLoot();bElite=PreviousElite;
 OpenedChests.Add(Zone);Cue(TEXT("victory"));SaveRun();
 PushLog(TEXT("ТАЙНИК: предмет со способностями. Открой инвентарь [I]."));
 return true;
}

void ULLGameInstance::InteractSanctuary(UWorld* World)
{
 if(Phase!=ELLPhase::Exploration)return;
 for(int32 Zone=0;Zone<5;++Zone)if(Rest(Zone)||OpenChest(Zone))return;
 SetNotice(TEXT("Подойди к светлому святилищу слева или к сундуку на возвышении."));
}

bool ULLGameInstance::Travel(UWorld* World,int32 Zone)
{
 if(!World||Phase!=ELLPhase::Map||!DiscoveredShrines.Contains(Zone))return false;
 SavedWorldLocation=FVector(Zone*2000.f+100,-650,165);
 CurrentBiome=Zone;Phase=ELLPhase::Exploration;
 if(APlayerController* PC=World->GetFirstPlayerController())
  if(APawn* Pawn=PC->GetPawn())Pawn->SetActorLocation(SavedWorldLocation);
 Rest(Zone);
 return true;
}

bool ULLGameInstance::UseFlask()
{
 if((Phase!=ELLPhase::BattlePlayer&&Phase!=ELLPhase::Exploration)||Flasks<=0||HeroHP>=MaxHP())return false;
 if(bInBattle&&AP<=0)return false;
 --Flasks;HeroHP=FMath::Min(MaxHP(),HeroHP+FMath::RoundToInt(MaxHP()*.55f));
 Cue(TEXT("heal"));PushLog(TEXT("ФЛЯГА: восстановлено 55% здоровья."));
 if(bInBattle){--AP;if(AP<=0)EndPlayerTurn();}
 else SaveRun();
 return true;
}

bool ULLGameInstance::ReinforceEquipment()
{
 if(Phase!=ELLPhase::Evolution||EvolutionSlots<3)return false;
 for(FLLItem& Item:Items)
 {
  const FString* Id=Equipped.Find(Item.Slot);
  if(Id&&*Id==Item.Id&&Item.Grade<6)
  {
   ++Item.Grade;EvolutionSlots-=3;Cue(TEXT("heal"));SaveRun();
   PushLog(TEXT("ЗАКАЛКА: 3 слота эволюции → +1 грейд надетого предмета."));
   return true;
  }
 }
 SetNotice(TEXT("Все надетые предметы уже G6."));return false;
}

void ULLGameInstance::TogglePause()
{
 if(Phase==ELLPhase::Paused){Phase=ResumePhase;return;}
 if(Phase==ELLPhase::Menu||Phase==ELLPhase::FinalVictory)return;
 ResumePhase=Phase;Phase=ELLPhase::Paused;
}

void ULLGameInstance::Cue(const FString& Name,const FString& Element)
{
 LastCue=Name;LastElement=Element;++CueSerial;
}

float ULLGameInstance::DefenseWindow(bool bParry) const
{
 return FMath::Lerp(bParry?Balance.ParryEnd:Balance.DodgeEnd,
  bParry?Balance.ParryStart:Balance.DodgeStart,FMath::Clamp(Level/100.f,0.f,1.f))+Assist;
}

float ULLGameInstance::SnapshotPower() const
{
 float Power=0.f;
 for(const FLLSkill& S:Snapshot)Power+=S.Power;
 float Mean=Snapshot.IsEmpty()?12.f:Power/Snapshot.Num();
 for(const FLLItem& I:SnapshotGear)
 {
  Mean+=.30f*I.Grade;
  for(const FString& Id:I.Skills)
   if(const FLLSkill* S=FindSkill(Id))Mean+=.015f*S->Power;
 }
 return Mean;
}
