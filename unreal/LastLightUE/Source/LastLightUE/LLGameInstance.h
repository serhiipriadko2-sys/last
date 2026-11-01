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
 Defeat, FinalVictory, Inventory, Forge, Evolution, Map
};

UCLASS()
class LASTLIGHTUE_API ULLGameInstance : public UGameInstance
{
 GENERATED_BODY()
public:
 virtual void Init() override;
 bool LoadDatabase();
 ELLPhase Phase=ELLPhase::Menu;
 bool bInBattle=false, bDemo=true, bElite=false, bFinal=false, bGuarded=false, bPerfectAttack=false;
 bool bHasStarted=false;
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
 TMap<FString,FString> Equipped;
 TArray<FString> ChosenStart, DefeatedIds, DefeatedBossIds;
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
