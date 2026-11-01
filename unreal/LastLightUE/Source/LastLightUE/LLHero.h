#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LLHero.generated.h"
class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UAnimSequence;
class ULLGameInstance;
class ALLStage;

UCLASS()
class LASTLIGHTUE_API ALLHero : public ACharacter
{
 GENERATED_BODY()
public:
 ALLHero();
 virtual void BeginPlay() override;
 virtual void Tick(float DeltaSeconds) override;
 virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
 bool bOrbiting=false,bSprinting=false,bHelp=false;
 UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Camera") TObjectPtr<USpringArmComponent> CameraBoom;
 UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Camera") TObjectPtr<UCameraComponent> Camera;
private:
 UPROPERTY() TObjectPtr<UStaticMeshComponent> Torso;
 UPROPERTY() TObjectPtr<UStaticMeshComponent> Head;
 UPROPERTY() TObjectPtr<UStaticMeshComponent> LeftArm;
 UPROPERTY() TObjectPtr<UStaticMeshComponent> RightArm;
 UPROPERTY() TObjectPtr<UStaticMeshComponent> Legs;
 UPROPERTY() TObjectPtr<UAnimSequence> IdleAnim;
 UPROPERTY() TObjectPtr<UAnimSequence> WalkAnim;
 UPROPERTY() TObjectPtr<UAnimSequence> RunAnim;
 UPROPERTY() TObjectPtr<UAnimSequence> AttackAnim;
 UPROPERTY() TObjectPtr<UAnimSequence> GuardAnim;
 UPROPERTY() TObjectPtr<UAnimSequence> DodgeAnim;
 UPROPERTY() TObjectPtr<UAnimSequence> HitAnim;
 FString CurrentVisualClip;
 float ActionVisualTime=0.0f;
 void UpdateVisualAnimation();
 void SetVisualClip(UAnimSequence* Clip,bool bLoop);
 ULLGameInstance* Game() const;
 void MoveForward(float V);
 void MoveRight(float V);
 void OrbitYaw(float V);
 void OrbitPitch(float V);
 void JumpPressed();
 void SprintBegin();
 void SprintEnd();
 void OrbitBegin();
 void OrbitEnd();
 void ZoomIn();
 void ZoomOut();
 void EngagePressed();
 void ParryPressed();
 void DodgePressed();
 void AttackPressed();
 void NormalPressed();
 void UseOne();
 void UseTwo();
 void UseThree();
 void UseFour();
 void UseFive();
 void UseSix();
 void UseSeven();
 void Pick(int32 Index);
 void NextPage();
 void EndTurn();
 void SavePressed();
 void LoadPressed();
 void InventoryPressed();
 void ForgePressed();
 void EvolutionPressed();
 void MapPressed();
 void BossPressed();
 void FinalPressed();
 void RandomPressed();
 void BackPressed();
 void SelectNext();
 void SelectPrevious();
 void MousePressed();
 void HelpPressed();
 void AssistPressed();
 bool HandleHUDClick();
 int32 MenuCursor=0;
};
