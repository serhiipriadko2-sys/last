#include "LLHUD.h"
#include "LLGameInstance.h"
#include "LLGameMode.h"
#include "LLHero.h"
#include "LLStage.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameViewportClient.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SEditableTextBox.h"

namespace LLUI
{
 static const FLinearColor BG(.034f,.06f,.10f,.92f);
 static const FLinearColor PANEL(.055f,.09f,.14f,.94f);
 static const FLinearColor BORDER(.30f,.41f,.53f,1.f);
 static const FLinearColor GOLD(.94f,.70f,.40f,1.f);
 static const FLinearColor WHITE(.88f,.93f,.98f,1.f);
 static const FLinearColor MUTED(.57f,.69f,.77f,1.f);
 static const FLinearColor GREEN(.32f,.75f,.57f,1.f);
 static const FLinearColor RED(.92f,.40f,.37f,1.f);
 static const FLinearColor BUTTON(.10f,.15f,.22f,.98f);
}

void ALLHUD::Fill(float X,float Y,float W,float H,FLinearColor Color)
{ DrawRect(Color,X*Scale,Y*Scale,W*Scale,H*Scale); }

void ALLHUD::Text(const FString& Str,float X,float Y,FLinearColor Color,float Size)
{
 if(!DisplayFont)DisplayFont=GEngine?GEngine->GetSmallFont():nullptr;
 if(!DisplayFont)return;
 DrawText(Str,Color,X*Scale,Y*Scale,DisplayFont,Size*Scale,false);
}
void ALLHUD::Panel(float X,float Y,float W,float H,const FString& Label)
{
 Fill(X+2,Y+3,W,H,FLinearColor(0,0,0,.30f));
 Fill(X,Y,W,H,LLUI::PANEL);
 Fill(X,Y,3,H,LLUI::BORDER);
 Fill(X,Y,W,1,LLUI::BORDER);
 Text(Label,X+14,Y+12,LLUI::GOLD,1.50f);
 Fill(X+12,Y+44,W-24,1,LLUI::BORDER);
}
void ALLHUD::Bar(float X,float Y,float W,float H,float Value,float MaxValue,FLinearColor Tint)
{
 Fill(X,Y,W,H,LLUI::BG);
 Fill(X,Y,FMath::Clamp(Value/FMath::Max(1.f,MaxValue),0.f,1.f)*W,H,Tint);
}
void ALLHUD::Button(float X,float Y,float W,float H,const FString& Label,
 const FString& Command,int32 Index,bool bHighlight)
{
 Fill(X,Y,W,H,bHighlight?FLinearColor(.45f,.30f,.17f,.96f):LLUI::BUTTON);
 Fill(X,Y,2,H,bHighlight?LLUI::GOLD:LLUI::BORDER);
 Text(Label,X+12,Y+11,bHighlight?LLUI::WHITE:LLUI::MUTED,1.16f);
 Zones.Add({FBox2D(FVector2D(X,Y),FVector2D(X+W,Y+H)),Command,Index});
 ++ButtonsThisFrame;
}

void ALLHUD::DrawHUD()
{
 Super::DrawHUD();
 if(!Canvas)return;
 Scale=FMath::Max(.55f,FMath::Min(Canvas->SizeX/1600.f,Canvas->SizeY/900.f));
 ULLGameInstance* GI=GetWorld()?GetWorld()->GetGameInstance<ULLGameInstance>():nullptr;
 if(!GI)return;
 Zones.Reset();ButtonsThisFrame=0;
 UpdateSearch(GI);
 if(GI->Phase==ELLPhase::Paused){DrawPause(GI);return;}
 if(GI->Phase==ELLPhase::Menu){DrawMainMenu(GI);return;}
 if(GI->Phase==ELLPhase::FinalVictory||GI->Phase==ELLPhase::Defeat)
 {DrawResult(GI);return;}
 DrawStats(GI);
 if(GI->bInBattle)DrawBattle(GI);
 else if(GI->Phase==ELLPhase::LossChoice)DrawSkillLoss(GI);
 else if(GI->Phase==ELLPhase::Inventory||GI->Phase==ELLPhase::Forge||
  GI->Phase==ELLPhase::Evolution||GI->Phase==ELLPhase::Map)
  DrawInventory(GI);
 else DrawWorld(GI);
 DrawLog(GI);
 if(GI->bInBattle&&(GI->Phase==ELLPhase::AttackTiming||GI->Phase==ELLPhase::EnemyTiming))
  DrawTiming(GI);
 if(APlayerController* PC=GetWorld()->GetFirstPlayerController())
 {
  if(const ALLHero* H=Cast<ALLHero>(PC->GetPawn()))
   if(H->bHelp)DrawControlHelp(GI);
 }
}

void ALLHUD::DrawStats(const ULLGameInstance* GI)
{
 Panel(20,20,300,222,TEXT("LAST LIGHT"));
 Text(FString::Printf(TEXT("УРОВЕНЬ  %03d"),GI->Level),34,74,LLUI::WHITE,1.45f);
 Text(FString::Printf(TEXT("HP  %d / %d"),GI->HeroHP,GI->MaxHP()),
  34,107,LLUI::WHITE,1.3f);
 Bar(34,138,260,12,GI->HeroHP,GI->MaxHP(),LLUI::GREEN);
 Text(FString::Printf(TEXT("НАВЫКИ %d   ·   ДОБЫЧА %d"),
  GI->Innate.Num(),GI->Items.Num()),34,163,LLUI::MUTED,1.10f);
 if(GI->bInBattle)
 {
  Text(FString::Printf(TEXT("ДЕЙСТВИЯ %d/2  ·  ФОКУС %d"),
   GI->AP,GI->Focus),34,185,LLUI::GOLD,1.06f);
 }
 else
 {
  Text(FString::Printf(TEXT("ПОБЕДЫ %d  ·  БОССЫ %d/9"),
   GI->Kills,GI->BossesWon),34,185,LLUI::MUTED,1.06f);
 }
 const TCHAR* AssistLabel=GI->Assist>.12f?TEXT("ШИРОКОЕ"):(GI->Assist>.03f?TEXT("УМЕРЕННОЕ"):TEXT("ОБЫЧНОЕ"));
 Text(FString::Printf(TEXT("F2  ·  ТАЙМИНГ: %s"),AssistLabel),34,210,LLUI::MUTED,1.05f);
 Text(FString::Printf(TEXT("C  ФЛЯГИ %d/3   ·   ESC ПАУЗА"),GI->Flasks),34,247,LLUI::WHITE,1.08f);
}

void ALLHUD::DrawWorld(const ULLGameInstance* GI)
{
 Panel(345,20,854,116,TEXT("ИССЛЕДОВАНИЕ   ·   "+GI->BiomeName(GI->CurrentBiome)));
 Text(TEXT("ИДИ ПО СВЕТЛОЙ ДОРОГЕ   ·   W ВПЕРЁД   ·   E ВРАГ"),
  360,77,LLUI::WHITE,1.23f);
 Text(FString::Printf(TEXT("Потеря уровня: %.1f / %.1f XP%s"),
  GI->XP,GI->XPToLose(GI->Level),
  GI->bDemo?TEXT("  ·  ускоренный режим"):TEXT("")),
  360,105,LLUI::MUTED,1.05f);
 Panel(1220,20,360,535,TEXT("ДЕЙСТВИЯ"));
 Button(1233,78,334,46,TEXT("E  ·  ВСТУПИТЬ В БОЙ"),TEXT("engage"),-1,true);
 Button(1233,130,334,42,TEXT("F  ·  НАЙТИ ВРАГА"),TEXT("hunt"));
 Button(1233,178,334,42,TEXT("B  ·  МИНИ-БОСС"),TEXT("boss"));
 Button(1233,226,334,42,TEXT("K  ·  ФИНАЛЬНОЕ ЗЕРКАЛО"),TEXT("mirror"));
 Button(1233,292,334,42,TEXT("I  ·  ЭКИПИРОВКА И ДОБЫЧА"),TEXT("inventory"));
 Button(1233,340,334,42,TEXT("G  ·  КУЗНИЦА"),TEXT("forge"));
 Button(1233,388,334,42,TEXT("V  ·  СЛИЯНИЕ НАВЫКОВ"),TEXT("evolution"));
 Button(1233,436,334,42,TEXT("M  ·  КАРТА"),TEXT("map"));
 Button(1233,484,160,42,TEXT("F5  СОХРАНИТЬ"),TEXT("save"));
 Button(1401,484,166,42,TEXT("F9  ЗАГРУЗИТЬ"),TEXT("load"));
 Text(TEXT("Shift бег  ·  Space прыжок  ·  ПКМ камера  ·  H помощь"),
  700,844,LLUI::WHITE,1.08f);
 Button(1233,565,334,42,TEXT("T  ·  СВЯТИЛИЩЕ / ТАЙНИК"),TEXT("interact"));
 Text(GI->Level==0?TEXT("ЦЕЛЬ: встреть себя в зеркале [K]"):
  TEXT("ЦЕЛЬ: исследуй, собирай добычу, спускайся к нулю"),360,146,LLUI::GOLD,1.1f);
}
void ALLHUD::DrawBattle(const ULLGameInstance* GI)
{
 Panel(345,20,854,120,TEXT("БИТВА  ·  "+GI->EnemyDisplay()));
 Text(FString::Printf(TEXT("ВРАГ  HP %d/%d   ·   ХОД %d"),
  GI->EnemyHP,GI->EnemyMaxHP,GI->Turn),360,76,LLUI::WHITE,1.20f);
 Bar(360,111,815,12,GI->EnemyHP,GI->EnemyMaxHP,LLUI::RED);
 Panel(1220,20,360,660,TEXT("ТВОИ ДЕЙСТВИЯ"));
 if(GI->Phase==ELLPhase::BattlePlayer)
 {
  Text(FString::Printf(TEXT("ТВОЙ ХОД  ·  %d/2"),GI->AP),
   1234,78,LLUI::GOLD,1.35f);
  Button(1232,117,336,42,TEXT("1  ·  ОБЫЧНЫЙ УДАР"),TEXT("skill"),0,true);
  Button(1232,166,336,42,TEXT("2  ·  ЗАЩИТА"),TEXT("skill"),1);
  Text(TEXT("НАВЫКИ  ·  Tab  следующая страница"),1234,221,LLUI::MUTED,1.16f);
  const auto Skills=GI->GetSkills();
  for(int32 J=0;J<5;++J)
  {
   int32 I=2+GI->SkillPage*5+J;
   if(!Skills.IsValidIndex(I))break;
   const FLLSkill& Skill=Skills[I];
   FString Name=Skill.Display;
   if(Name.Len()>26)Name=Name.Left(25)+TEXT("…");
   Button(1232,255+J*56,336,50,
    FString::Printf(TEXT("%d  %s  [%d]"),J+3,*Name,Skill.Power),
    TEXT("skill"),I);
  }
  Button(1232,557,336,46,TEXT("F  ·  ЗАВЕРШИТЬ ХОД"),TEXT("endturn"),-1,true);
  Text(FString::Printf(TEXT("Доступно %d навыков"),Skills.Num()),
   1236,615,LLUI::MUTED,1.10f);
  Button(1232,686,336,42,FString::Printf(TEXT("C  ФЛЯГА (%d) · +55%% HP · 1 ОД"),GI->Flasks),TEXT("flask"));
  Text(TEXT("Слабость: ")+GI->ActiveEnemy.Weakness,360,149,LLUI::GOLD,1.10f);
  if(GI->bFinal)Text(FString::Printf(TEXT("ЗЕРКАЛО · ФАЗА %d / 3"),GI->MirrorPhase),360,179,LLUI::GOLD,1.25f);
 }
 else if(GI->Phase==ELLPhase::AttackTiming)
 {
  Text(TEXT("ТВОЯ АТАКА  ·  ВОВРЕМЯ!"),1234,84,LLUI::GOLD,1.42f);
  Text(TEXT("SPACE  ·  Точный удар +25%"),1234,141,LLUI::WHITE,1.15f);
  Button(1232,203,336,48,TEXT("ENTER  ·  Обычный удар"),TEXT("normal"),-1,true);
 }
 else if(GI->Phase==ELLPhase::EnemyTiming)
 {
  Text(TEXT("ВРАГ АТАКУЕТ  ·  ГОТОВЬСЯ"),1234,84,LLUI::RED,1.37f);
  Text(GI->AttackLabel.Left(30),1234,132,LLUI::WHITE,1.13f);
  Button(1232,188,336,48,TEXT("Q  ·  ПАРИРОВАТЬ"),TEXT("parry"),-1,true);
  Button(1232,246,336,48,TEXT("E  ·  УКЛОНИТЬСЯ"),TEXT("dodge"));
  Text(TEXT("Парирование сложнее, но наносит"),1235,318,LLUI::MUTED,1.05f);
  Text(TEXT("контрудар. Следи за шкалой снизу."),1235,345,LLUI::MUTED,1.05f);
 }
}
void ALLHUD::DrawTiming(const ULLGameInstance* GI)
{
 Panel(415,655,760,112,GI->Phase==ELLPhase::AttackTiming?
  TEXT("SPACE  ·  ТОЧНЫЙ УДАР 67%"):TEXT("Q ПАРИРОВАТЬ   /   E УКЛОНИТЬСЯ"));
 const float Ratio=GI->Phase==ELLPhase::AttackTiming?
  FMath::Clamp(GI->Timing/1.05f,0.f,1.f):
  FMath::Clamp(GI->Timing/FMath::Max(.1f,GI->AttackDuration),0.f,1.f);
 const float Target=GI->Phase==ELLPhase::AttackTiming?.67f:GI->AttackFraction;
 const float Duration=GI->Phase==ELLPhase::AttackTiming?1.05f:GI->AttackDuration;
 const float Window=GI->Phase==ELLPhase::AttackTiming?(.29f+GI->Assist*.9f):GI->DefenseWindow(true);
 Fill(440,719,710,24,LLUI::BG);
 Fill(440+710*(Target-Window/Duration*.5f),713,710*Window/Duration,36,LLUI::GREEN);
 Fill(440+710*Target-2,710,4,42,LLUI::GOLD);
 Fill(440+710*Ratio-2,709,5,44,LLUI::WHITE);
 Text(TEXT("Белый курсор → золотая отметка"),654,692,LLUI::WHITE,1.10f);
}
void ALLHUD::DrawMainMenu(const ULLGameInstance* GI)
{
 Fill(0,0,1600,900,FLinearColor(.020f,.033f,.063f,.72f));
 Panel(260,94,1080,700,TEXT("LAST LIGHT  ·  ИНВЕРТИРОВАННАЯ RPG"));
 Text(TEXT("Начнёшь богом. Закончишь человеком."),295,166,LLUI::WHITE,1.85f);
 Text(TEXT("Выбери 50 из 100 навыков. Финальный босс сохранит твой начальный билд."),
  295,221,LLUI::MUTED,1.20f);
 Text(FString::Printf(TEXT("ВЫБРАНО  %d/50   ·   СТРАНИЦА %d/20"),
  GI->ChosenStart.Num(),GI->MenuPage+1),
  295,274,LLUI::GOLD,1.40f);
 for(int32 J=0;J<5;++J)
 {
  const int32 Index=GI->MenuPage*5+J;
  if(!GI->AllSkills.IsValidIndex(Index))break;
  const FLLSkill& Skill=GI->AllSkills[Index];
  const bool Selected=GI->ChosenStart.Contains(Skill.Id);
  Button(292,325+J*64,1010,56,
   FString::Printf(TEXT("%s  %s   ·   %s   ·   СИЛА %d"),
    Selected?TEXT("●"):TEXT("○"),*Skill.Display,*Skill.Element,Skill.Power),
   TEXT("toggle"),Index,Selected);
 }
 Button(292,660,312,53,TEXT("R  СЛУЧАЙНЫЙ БИЛД"),TEXT("random"));
 Button(618,660,312,53,TEXT("TAB  СЛЕД. СТРАНИЦА"),TEXT("next"));
 Button(945,660,357,53,TEXT("ENTER  НАЧАТЬ ИГРУ"),TEXT("start"),-1,true);
 Text(TEXT("Управление: WASD · Shift · Space · E · 1–5 · Q/E · F5/F9"),
  330,747,LLUI::MUTED,1.10f);
}
void ALLHUD::DrawSkillLoss(const ULLGameInstance* GI)
{
 Panel(350,180,850,610,TEXT("РАСПАД   ·   ПОТЕРЯ УРОВНЯ"));
 Text(FString::Printf(TEXT("УРОВЕНЬ %d → %d    ·   ОСТАЛОСЬ ПОТЕРЬ %d"),
  GI->Level,GI->Level-1,GI->PendingLoss),380,252,LLUI::WHITE,1.54f);
 Text(TEXT("Выбери навык для удаления. Второй исчезнет случайно, появится один новый."),
  380,297,LLUI::MUTED,1.18f);
 for(int32 J=0;J<5;++J)
 {
  const int32 I=GI->SkillPage*5+J;
  if(!GI->Innate.IsValidIndex(I))break;
  Button(375,349+J*65,785,54,
   FString::Printf(TEXT("%d  УДАЛИТЬ: %s"),J+1,*GI->Innate[I].Display.Left(42)),
   TEXT("loss"),I);
 }
 Button(385,693,365,50,TEXT("TAB  ·  ДРУГИЕ НАВЫКИ"),TEXT("next"));
 if(GI->Innate.Num()<2)
  Button(770,693,380,50,TEXT("ENTER  ·  ПРОПУСТИТЬ"),TEXT("loss"),0,true);
}
void ALLHUD::DrawInventory(const ULLGameInstance* GI)
{
 const bool Forge=GI->Phase==ELLPhase::Forge;
 const bool Evo=GI->Phase==ELLPhase::Evolution;
 const bool Map=GI->Phase==ELLPhase::Map;
 Panel(360,170,810,610,Map?TEXT("КАРТА ПЯТИ ОБЛАСТЕЙ"):
  (Forge?TEXT("КУЗНИЦА ПРЕДМЕТОВ"):
  (Evo?TEXT("СЛИЯНИЕ НАВЫКОВ"):TEXT("ИНВЕНТАРЬ / ЭКИПИРОВКА"))));
 if(Map)
 {
  for(int32 I=0;I<5;++I)
  {
   Fill(411+I*144,350,132,70,
    I==GI->CurrentBiome?LLUI::GOLD:LLUI::BUTTON);
   Text(FString::Printf(TEXT("%02d"),I+1),452+I*144,368,LLUI::WHITE,1.35f);
  }
  Text(TEXT("Следуй вперёд по основной дороге. Обходи врагов или вступай в бой."),
   398,490,LLUI::MUTED,1.18f);
 }
 else if(Evo)
 {
  Text(FString::Printf(TEXT("СВОБОДНО СЛОТОВ %d. Выбери 2 навыка (1–5)."),
   GI->EvolutionSlots),390,254,LLUI::WHITE,1.24f);
  for(int32 J=0;J<5;++J)
  {
   int32 I=GI->SkillPage*5+J;
   if(!GI->Innate.IsValidIndex(I))break;
   Button(390,316+J*68,735,56,GI->Innate[I].Display,
    TEXT("evolve"),I,I==GI->SelectedItem);
  }
 }
 else
 {
  Text(Forge?TEXT("Выбери два предмета. Появится шанс поломки и подтверждение."):
   TEXT("Выбери предмет для экипировки. Он даёт дополнительные навыки."),
   390,251,LLUI::MUTED,1.22f);
  if(Forge && GI->Items.IsValidIndex(GI->SelectedItem))
  {
   const int32 First=GI->SelectedItem,Second=GI->PendingForgeIndex;
   if(GI->Items.IsValidIndex(Second))
   {
    const float Risk=GI->ForgeBreakChance(First,Second);
    Text(FString::Printf(TEXT("РИСК ПОТЕРЯТЬ ОДИН ПРЕДМЕТ: %.0f%%   |   G%d -> G%d"),
     Risk*100.0f,
     FMath::Max(GI->Items[First].Grade,GI->Items[Second].Grade),
     FMath::Max(GI->Items[First].Grade,GI->Items[Second].Grade)+1),
     390,282,LLUI::GOLD,1.16f);
   }
   else
   {
    Text(TEXT("ПЕРВЫЙ ПРЕДМЕТ ВЫБРАН. Выбери второй."),390,282,LLUI::GOLD,1.14f);
   }
  }
  for(int32 J=0;J<5;++J)
  {
   int32 I=GI->ItemPage*5+J;if(!GI->Items.IsValidIndex(I))break;
   const FLLItem& Item=GI->Items[I];
   Button(390,310+J*67,735,55,
    FString::Printf(TEXT("%s  ·  G%d  ·  %s  ·  %d навыка"),
     *Item.Name,Item.Grade,*Item.Slot,Item.Skills.Num()),
    Forge?TEXT("forgeitem"):TEXT("equip"),I,I==GI->SelectedItem);
  }
  if(Forge && GI->Items.IsValidIndex(GI->SelectedItem)
      && GI->Items.IsValidIndex(GI->PendingForgeIndex))
  {
   Button(390,643,350,42,TEXT("ОТМЕНИТЬ СЛИЯНИЕ"),TEXT("forgecancel"));
   Button(760,643,366,42,TEXT("ENTER  ПОДТВЕРДИТЬ РИСК"),
    TEXT("forgeconfirm"),-1,true);
  }
  else
   Text(FString::Printf(TEXT("ПРЕДМЕТОВ  %d"),GI->Items.Num()),
    390,660,LLUI::MUTED,1.06f);
 }
 Button(385,690,363,50,TEXT("TAB  ·  ЕЩЁ"),TEXT("next"));
 Button(770,690,365,50,TEXT("ESC  ·  В МИР"),TEXT("back"),-1,true);
}
void ALLHUD::DrawResult(const ULLGameInstance* GI)
{
 Fill(0,0,1600,900,FLinearColor(.022f,.034f,.064f,.94f));
 Panel(370,240,860,450,GI->Phase==ELLPhase::FinalVictory?
  TEXT("ФИНАЛ  ·  ЗЕРКАЛО РАЗБИТО"):TEXT("ПОРАЖЕНИЕ"));
 Text(GI->Phase==ELLPhase::FinalVictory?
  TEXT("Ты победил того, кем был в начале."):
  TEXT("Герой пал, но забег можно продолжить."),
  418,348,LLUI::WHITE,1.75f);
 Text(FString::Printf(TEXT("УРОВЕНЬ %d   ·   ПОБЕД %d   ·   БОССОВ %d/9"),
  GI->Level,GI->Kills,GI->BossesWon),419,410,LLUI::MUTED,1.16f);
 Button(420,490,740,60,GI->Phase==ELLPhase::Defeat?
  TEXT("ENTER   ·   ПОВТОРИТЬ"):TEXT("ENTER  ·  НОВАЯ ИГРА"),
  TEXT("result"),-1,true);
 Button(420,574,740,48,TEXT("ESC  ·  НАЗАД В МИР"),TEXT("back"));
}
void ALLHUD::DrawLog(const ULLGameInstance* GI)
{
 Panel(20,775,650,104,TEXT("СОБЫТИЯ"));
 Text(GI->Notice.Left(70),35,832,LLUI::WHITE,1.15f);
}
void ALLHUD::DrawControlHelp(const ULLGameInstance* GI)
{
 Panel(545,215,530,510,TEXT("УПРАВЛЕНИЕ"));
 const TArray<FString> Lines={
  TEXT("WASD  ·  движение относительно камеры"),
  TEXT("Shift  ·  бег       Space  ·  прыжок"),
  TEXT("ПКМ + мышь  ·  вращать камеру"),
  TEXT("Колесо  ·  приблизить / отдалить"),
  TEXT("E  ·  начать бой с ближним врагом"),
  TEXT("F  ·  обычный бой / закончить ход"),
  TEXT("1–5  ·  выбрать навык"),
  TEXT("SPACE  ·  точный удар на золотой метке"),
  TEXT("Q  ·  парировать, E  ·  уклониться"),
  TEXT("I  ·  вещи   G  ·  кузница   V  ·  симбиоз"),
  TEXT("M  ·  карта, B  ·  мини-босс"),
  TEXT("K  ·  финальный босс (на уровне 0)"),
  TEXT("F5  ·  сохранить   F9  ·  загрузить"),
  TEXT("H  ·  закрыть подсказку")
 };
 for(int32 I=0;I<Lines.Num();++I)Text(Lines[I],567,273+I*29,LLUI::WHITE,1.12f);
}
bool ALLHUD::HandleClick(float ScreenX,float ScreenY)
{
 ULLGameInstance* GI=GetWorld()?GetWorld()->GetGameInstance<ULLGameInstance>():nullptr;
 if(!GI)return false;
 const FVector2D P(ScreenX/FMath::Max(.1f,Scale),ScreenY/FMath::Max(.1f,Scale));
 for(const FLLClickZone& Hit:Zones)
 {
  if(!Hit.Box.IsInside(P))continue;
  const FString& C=Hit.Command;
  if(C==TEXT("start"))GI->StartRun(GetWorld());
  else if(C==TEXT("random"))GI->RandomizeStartingBuild();
  else if(C==TEXT("toggle"))GI->ToggleStartSkill(Hit.Index);
  else if(C==TEXT("skill"))GI->UseSkill(Hit.Index);
  else if(C==TEXT("loss"))GI->LevelLoss(Hit.Index);
  else if(C==TEXT("endturn"))GI->EndPlayerTurn();
  else if(C==TEXT("normal"))GI->CompleteAttack(false);
  else if(C==TEXT("parry"))GI->Defend(true);
  else if(C==TEXT("dodge"))GI->Defend(false);
  else if(C==TEXT("next"))
  {
   if(GI->Phase==ELLPhase::Menu)GI->MenuPage=(GI->MenuPage+1)%20;
   else if(GI->Phase==ELLPhase::Inventory||GI->Phase==ELLPhase::Forge)
    GI->ItemPage=(GI->ItemPage+1)%FMath::Max(1,FMath::CeilToInt(GI->Items.Num()/5.f));
   else GI->SkillPage=(GI->SkillPage+1)%FMath::Max(1,FMath::CeilToInt(GI->Innate.Num()/5.f));
  }
  else if(C==TEXT("hunt"))GI->Engage(GetWorld(),GI->CurrentBiome*6,false,false,false,
   GI->SavedWorldLocation);
  else if(C==TEXT("engage"))
  {
   if(const ALLGameMode* Mode=Cast<ALLGameMode>(GetWorld()->GetAuthGameMode()))
   {
    if(const ALLStage* Stage=Mode->GetStage())
    {
     const int32 I=Stage->FindNearestEnemy(GI->SavedWorldLocation);
     if(I>=0)GI->Engage(GetWorld(),I,false,false,false,GI->SavedWorldLocation);
     else GI->SetNotice(TEXT("Подойди ближе к противнику"));
    }
   }
  }
  else if(C==TEXT("boss"))
  {
   for(int32 I=0;I<GI->Bosses.Num();++I)
    if(GI->IsMinibossUnlocked(I))
    {GI->Engage(GetWorld(),I,true,false,false,GI->SavedWorldLocation);break;}
  }
  else if(C==TEXT("mirror"))GI->Engage(GetWorld(),0,false,false,true,GI->SavedWorldLocation);
  else if(C==TEXT("inventory")){GI->Phase=ELLPhase::Inventory;GI->ItemPage=0;}
  else if(C==TEXT("forge")){GI->Phase=ELLPhase::Forge;GI->CancelForge();}
  else if(C==TEXT("evolution")){GI->Phase=ELLPhase::Evolution;GI->SelectedItem=-1;}
  else if(C==TEXT("map"))GI->Phase=ELLPhase::Map;
  else if(C==TEXT("save"))GI->SaveRun();
  else if(C==TEXT("load"))GI->LoadRun(GetWorld());
  else if(C==TEXT("equip"))GI->Equip(Hit.Index);
  else if(C==TEXT("forgeitem"))GI->QueueForge(Hit.Index);
  else if(C==TEXT("forgeconfirm"))GI->ConfirmForge();
  else if(C==TEXT("forgecancel"))GI->CancelForge();
  else if(C==TEXT("evolve"))
  {
   if(GI->SelectedItem<0)GI->SelectedItem=Hit.Index;
   else {GI->Evolve(GI->SelectedItem,Hit.Index);GI->SelectedItem=-1;}
  }
  else if(C==TEXT("result"))
  {
   if(GI->Phase==ELLPhase::Defeat)GI->Retry(GetWorld());
   else {GI->bInBattle=false;GI->Phase=ELLPhase::Menu;
    UGameplayStatics::OpenLevel(GetWorld(),FName(TEXT("/Game/Maps/Explore")));}
  }
  else if(C==TEXT("back"))
  {
   if(GI->Phase==ELLPhase::Defeat)GI->ReturnToExploration(GetWorld());
   else if(GI->Phase==ELLPhase::Forge||GI->Phase==ELLPhase::Inventory||
    GI->Phase==ELLPhase::Evolution||GI->Phase==ELLPhase::Map)
   {GI->CancelForge();GI->Phase=ELLPhase::Exploration;}
  }
  return true;
 }
 return false;
}
