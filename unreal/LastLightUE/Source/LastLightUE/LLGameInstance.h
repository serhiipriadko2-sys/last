#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LLGameInstance.generated.h"

// All identifiers are preserved from LastLight Godot v0.6 JSON.
// Game state is intentionally separate from maps, meshes and controls.
struct FLLSkill
{
 FString Id, Name, Display, Element, Kind, Status;
 int32 Power=8, Cost=1, Depth=0;
 bool bSignature=false;
 TArray<FString> Parents;
};

struct FLLEnemy
{
 FString Id, Name, Element, Weakness, Shape, HexColor;
 int32 HP=40, Damage=8, Gate=100;
 TArray<FString> Attacks;
 bool bBoss=false;
};

struct FLLItem
{
 FString Id, Name, Slot, Rarity;
 int32 Grade=1;
 TArray<FString> Skills, Sealed;
};

enum class ELLPhase : uint8
{
 Menu, Exploration, BattlePlayer, AttackTiming, EnemyTiming, LossChoice,
 Defeat, FinalVictory, Inventory, Forge, Evolution, Map, Paused
};

struct FLLBalance
{
 int32 Loadout=50, MaxLosses=12, DemoLosses=5, MaxInventory=64, MaxDepth=4;
 int32 HPStart=120, HPEnd=70, BasicDamage=8;
 float XPBase=120.f, XPK=.96f, XPReward=120.f, Difficulty=1.6f;
 float ParryStart=.27f, ParryEnd=.17f, DodgeStart=.38f, DodgeEnd=.26f;
 float ForgeBase=.10f, ForgeStep=.12f, VictoryHeal=.25f;
 bool bDemo=false;
};

UCLASS()
class LASTLIGHTUE_API ULLGameInstance : public UGameInstance
{
 GENERATED_BODY()
public:
 virtual void Init() override;
 bool LoadDatabase();
 bool LoadBalance();
 ELLPhase Phase=ELLPhase::Menu;
 bool bInBattle=false, bDemo=true, bElite=false, bFinal=false, bGuarded=false, bPerfectAttack=false;
 bool bHasStarted=false;
 bool bSymbiote=false, bWon=false, bMuted=false;
 ELLPhase ResumePhase=ELLPhase::Exploration;
 FLLBalance Balance;
 int32 Flasks=3, MirrorPhase=1;
 int32 MenuElement=0, MenuKind=0, MenuSort=0;
 FString MenuSearch;
 bool bSearchFocused=false;
 FString LastCue, LastElement=TEXT("physical");
 int32 CueSerial=0;
 int32 Level=100, HeroHP=120, Kills=0, EvolutionSlots=0, BossesWon=0;
 int32 AP=2, Focus=0, Turn=1, EnemyHP=0, EnemyMaxHP=0, EnemyDamage=0;
 int32 PendingLoss=0, LossesCompleted=0, AttackIndex=0, AttacksRemaining=0;
 int32 SkillPage=0, MenuPage=0, ItemPage=0, SelectedItem=-1, PendingForgeIndex=-1;
 int32 CurrentBiome=0, CurrentEnemyIndex=0;
 float XP=0.0f, Timing=0.0f, AttackDuration=1.65f, AttackFraction=.72f;
 float AttackMultiplier=1.0f, Assist=0.0f;
 FVector SavedWorldLocation=FVector(160.f,0.f,165.f);
 FLLSkill PendingAttack;
 FLLEnemy ActiveEnemy;
 FString Notice, AttackLabel, EnemyStatus, HeroStatus;
 int32 EnemyStatusTurns=0, HeroStatusTurns=0;
 TArray<FLLSkill> AllSkills, Innate, Snapshot;
 TArray<FLLEnemy> Mobs, Bosses;
 TArray<FLLItem> Items;
 TArray<FLLItem> SnapshotGear;
 TMap<FString,FString> Equipped;
 TArray<FString> ChosenStart, DefeatedIds, DefeatedBossIds;
 TArray<int32> OpenedChests, DiscoveredShrines;
 TArray<FString> BattleLog;
 FRandomStream Random;

 const FLLSkill* FindSkill(const FString& Id) const;
 const FLLItem* FindItem(const FString& Id) const;
 TArray<FLLSkill> GetSkills() const;
 void RandomizeStartingBuild();
 bool ToggleStartSkill(int32 Index);
 bool StartRun(UWorld* World);
 int32 MaxHP() const;
 float XPToLose(int32 CurrentLevel) const;
 void Engage(UWorld* World, int32 EnemyIndex, bool bIsBoss=false, bool bIsElite=false,
             bool bIsFinal=false, const FVector& From=FVector::ZeroVector);
 void StartBattle();
 void UseSkill(int32 ListIndex);
 void BasicStrike();
 void BasicGuard();
 void BeginAttack(const FLLSkill& Skill);
 void CompleteAttack(bool bPerfect);
 void EndPlayerTurn();
 void TickCombat(float DeltaSeconds);
 void Defend(bool bParry);
 void ResolveAttack(bool bAttempt, bool bParry);
 void AdvanceEnemyTurn();
 void EnemyTurn();
 void Win(UWorld* World);
 void Lose();
 void Retry(UWorld* World);
 void ReturnToExploration(UWorld* World);
 void LevelLoss(int32 SelectedIndex);
 void CompleteLoss(UWorld* World);
 float ForgeBreakChance(int32 A, int32 B) const;
 bool QueueForge(int32 Index);
 bool ConfirmForge();
 void CancelForge();
 bool Forge(int32 A, int32 B);
 bool Equip(int32 Index);
 bool Evolve(int32 A, int32 B);
 bool SaveRun() const;
 bool LoadRun(UWorld* World);
 void PushLog(const FString& Text);
 void SetNotice(const FString& Text);
 void CycleTimingAssist();
 FString BiomeName(int32 Biome) const;
 FString EnemyDisplay() const;
 bool IsMinibossUnlocked(int32 Index) const;
 bool CanEnterFinal() const { return Level==0; }
 TArray<int32> FilteredSkills() const;
 void NextPage();
 void Hunt(UWorld* World);
 void InteractSanctuary(UWorld* World);
 bool OpenChest(int32 Zone);
 bool Rest(int32 Zone);
 bool Travel(UWorld* World,int32 Zone);
 bool UseFlask();
 bool ReinforceEquipment();
 void TogglePause();
 void Cue(const FString& Name,const FString& Element=TEXT("physical"));
 float DefenseWindow(bool bParry) const;
 float SnapshotPower() const;
 FString SavePath() const;
 FString SerializeRun() const;
 bool RestoreRun(const FString& Text);

private:
 bool LoadSkillArray(const FString& File);
 bool LoadEnemyArray(const FString& File, bool bBossRecords);
 bool LoadGear(const FString& File);
 int32 GetDamageFor(const FLLSkill& Skill, float TimingBonus) const;
 void ApplySkill(const FLLSkill& Skill, float TimingBonus);
 void GiveLoot();
 void ApplyXP();
 void PrepareEnemyAttack();
 const FLLSkill* GetMirrorPatternSkill(int32 AttackNumber) const;
 FString NextAttackName();
};
