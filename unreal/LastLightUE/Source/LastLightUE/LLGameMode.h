#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LLGameMode.generated.h"
class ALLStage;
class ALLHero;
class ULLGameInstance;
class USkeletalMeshComponent;
class UAnimSequence;

UCLASS()
class LASTLIGHTUE_API ALLGameMode : public AGameModeBase
{
 GENERATED_BODY()
public:
 ALLGameMode();
 virtual void StartPlay() override;
 virtual void Tick(float DeltaSeconds) override;
 ALLStage* GetStage() const {return Stage;}
 bool IsArena() const {return bArenaMap;}
private:
 UPROPERTY() TObjectPtr<ALLStage> Stage;
 UPROPERTY() TObjectPtr<AActor> EnemyVisual;
 UPROPERTY() TObjectPtr<USkeletalMeshComponent> EnemyRig;
 UPROPERTY() TObjectPtr<UAnimSequence> EnemyIdle;
 UPROPERTY() TObjectPtr<UAnimSequence> EnemyAttack;
 UPROPERTY() TObjectPtr<UAnimSequence> EnemyHit;
 FString EnemyCurrentClip;
 int32 EnemyLastAttackIndex=-1;
 bool bArenaMap=false;
 bool bQAScreenshotTaken=false;
 float Clock=0;
 void SetupPawn(ALLHero* Player);
 void SpawnOpponent();
};
