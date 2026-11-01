#include "LLHero.h"
#include "LLGameInstance.h"
#include "LLGameMode.h"
#include "LLStage.h"
#include "LLHUD.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

ALLHero::ALLHero()
{
 PrimaryActorTick.bCanEverTick=true;
 GetCapsuleComponent()->InitCapsuleSize(39.f,92.f);
 GetCharacterMovement()->MaxWalkSpeed=440.f;
 GetCharacterMovement()->JumpZVelocity=520.f;
 GetCharacterMovement()->AirControl=.30f;
 GetCharacterMovement()->bOrientRotationToMovement=true;
 GetCharacterMovement()->RotationRate=FRotator(0,560.f,0);
 bUseControllerRotationYaw=false;
 GetMesh()->SetVisibility(false,true);
 CameraBoom=CreateDefaultSubobject<USpringArmComponent>(TEXT("Boom"));
 CameraBoom->SetupAttachment(GetRootComponent());
 CameraBoom->TargetArmLength=820.f;
 CameraBoom->SetRelativeLocation(FVector(0,0,122));
 CameraBoom->bUsePawnControlRotation=true;
 CameraBoom->bDoCollisionTest=true;
 CameraBoom->ProbeSize=14;
 Camera=CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
 Camera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
 Camera->bUsePawnControlRotation=false;
 static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeRef(TEXT("/Engine/BasicShapes/Cube.Cube"));
 static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereRef(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
 Torso=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeroChest"));
 Torso->SetupAttachment(GetCapsuleComponent());
 Head=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeroHead"));
 Head->SetupAttachment(GetCapsuleComponent());
 LeftArm=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeroLeftArm"));
 LeftArm->SetupAttachment(GetCapsuleComponent());
 RightArm=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeroRightArm"));
 RightArm->SetupAttachment(GetCapsuleComponent());
 Legs=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeroLegs"));
 Legs->SetupAttachment(GetCapsuleComponent());
 if(CubeRef.Succeeded())
 {
  Torso->SetStaticMesh(CubeRef.Object);LeftArm->SetStaticMesh(CubeRef.Object);
  RightArm->SetStaticMesh(CubeRef.Object);Legs->SetStaticMesh(CubeRef.Object);
 }
 if(SphereRef.Succeeded())Head->SetStaticMesh(SphereRef.Object);
 Torso->SetRelativeLocation(FVector(0,0,4));
 Torso->SetRelativeScale3D(FVector(.55f,.42f,.68f));
 Head->SetRelativeLocation(FVector(0,0,69));
 Head->SetRelativeScale3D(FVector(.31f,.32f,.35f));
 LeftArm->SetRelativeLocation(FVector(0,-37,4));
 LeftArm->SetRelativeScale3D(FVector(.23f,.24f,.70f));
 RightArm->SetRelativeLocation(FVector(0,37,4));
 RightArm->SetRelativeScale3D(FVector(.23f,.24f,.70f));
 Legs->SetRelativeLocation(FVector(0,0,-62));
 Legs->SetRelativeScale3D(FVector(.37f,.35f,.47f));
 for(UStaticMeshComponent* Piece:{Torso.Get(),Head.Get(),LeftArm.Get(),RightArm.Get(),Legs.Get()})
 {
  Piece->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  Piece->SetCastShadow(true);
 }
 // Joined original Blender rig: one mesh, one skeleton, seven UE animation assets.
 // These direct asset references also make Unreal cooker include the playable art.
 static ConstructorHelpers::FObjectFinder<USkeletalMesh> Visual(
  TEXT("/Game/Characters/Joined/hero/hero/SkeletalMeshes/hero.hero"));
 static ConstructorHelpers::FObjectFinder<UAnimSequence> AIdle(
  TEXT("/Game/Characters/Joined/hero/hero/SkeletalMeshes/heroIdle.heroIdle"));
 static ConstructorHelpers::FObjectFinder<UAnimSequence> AWalk(
  TEXT("/Game/Characters/Joined/hero/hero/SkeletalMeshes/heroWalk.heroWalk"));
 static ConstructorHelpers::FObjectFinder<UAnimSequence> ARun(
  TEXT("/Game/Characters/Joined/hero/hero/SkeletalMeshes/heroRun.heroRun"));
 static ConstructorHelpers::FObjectFinder<UAnimSequence> AAttack(
  TEXT("/Game/Characters/Joined/hero/hero/SkeletalMeshes/heroAttack.heroAttack"));
 static ConstructorHelpers::FObjectFinder<UAnimSequence> AGuard(
  TEXT("/Game/Characters/Joined/hero/hero/SkeletalMeshes/heroGuard.heroGuard"));
 static ConstructorHelpers::FObjectFinder<UAnimSequence> ADodge(
  TEXT("/Game/Characters/Joined/hero/hero/SkeletalMeshes/heroDodge.heroDodge"));
 static ConstructorHelpers::FObjectFinder<UAnimSequence> AHit(
  TEXT("/Game/Characters/Joined/hero/hero/SkeletalMeshes/heroHit.heroHit"));
 if(Visual.Succeeded())
 {
  GetMesh()->SetSkeletalMeshAsset(Visual.Object);
  // Original Blender rig is approximately 300cm tall, while the capsule is 184cm.
  // Scale to 201cm and align feet with capsule base (actor origin stands 92cm over ground).
  GetMesh()->SetRelativeScale3D(FVector(.67f));
  GetMesh()->SetRelativeLocation(FVector(0,0,-90.f));
  GetMesh()->SetVisibility(true,true);
  for(UStaticMeshComponent* Piece:{Torso.Get(),Head.Get(),LeftArm.Get(),RightArm.Get(),Legs.Get()})
   Piece->SetVisibility(false,true);
  IdleAnim=AIdle.Object;WalkAnim=AWalk.Object;RunAnim=ARun.Object;
  AttackAnim=AAttack.Object;GuardAnim=AGuard.Object;DodgeAnim=ADodge.Object;
  HitAnim=AHit.Object;
 }
}
ULLGameInstance* ALLHero::Game() const
{return GetGameInstance<ULLGameInstance>();}
void ALLHero::SetVisualClip(UAnimSequence* Clip,bool bLoop)
{
 if(!Clip||!GetMesh()->GetSkeletalMeshAsset())return;
 const FString Label=Clip->GetPathName();
 if(CurrentVisualClip==Label)return;
 CurrentVisualClip=Label;
 GetMesh()->PlayAnimation(Clip,bLoop);
}
void ALLHero::UpdateVisualAnimation()
{
 if(!IdleAnim)return;
 ULLGameInstance* GI=Game();
 if(GI&&GI->bInBattle)
 {SetVisualClip(GI->Phase==ELLPhase::EnemyTiming?GuardAnim:IdleAnim,true);return;}
 const float Speed=GetVelocity().Size2D();
 if(Speed>560.f)SetVisualClip(RunAnim,true);
 else if(Speed>65.f)SetVisualClip(WalkAnim,true);
 else SetVisualClip(IdleAnim,true);
}

void ALLHero::BeginPlay()
{
 Super::BeginPlay();
 SetVisualClip(IdleAnim,true);
 if(APlayerController* PC=Cast<APlayerController>(GetController()))
 {
  PC->bShowMouseCursor=true;
  PC->bEnableClickEvents=true;
  PC->SetInputMode(FInputModeGameAndUI());
 }
}
void ALLHero::Tick(float DeltaSeconds)
{
 Super::Tick(DeltaSeconds);
 ActionVisualTime=FMath::Max(0.f,ActionVisualTime-DeltaSeconds);
 if(ActionVisualTime<=0.f)UpdateVisualAnimation();
 if(ULLGameInstance* GI=Game())
 {
  if(GI->Phase==ELLPhase::Exploration&&!GI->bInBattle)
  {
   GI->SavedWorldLocation=GetActorLocation();
   GI->CurrentBiome=FMath::Clamp(FMath::FloorToInt(
    GetActorLocation().X/2000.f),0,4);
  }
 }
}
void ALLHero::SetupPlayerInputComponent(UInputComponent* Input)
{
 Super::SetupPlayerInputComponent(Input);
 Input->BindAxis(TEXT("MoveForward"),this,&ALLHero::MoveForward);
 Input->BindAxis(TEXT("MoveRight"),this,&ALLHero::MoveRight);
 Input->BindAxis(TEXT("Turn"),this,&ALLHero::OrbitYaw);
 Input->BindAxis(TEXT("LookUp"),this,&ALLHero::OrbitPitch);
 Input->BindAction(TEXT("Jump"),IE_Pressed,this,&ALLHero::JumpPressed);
 Input->BindAction(TEXT("Sprint"),IE_Pressed,this,&ALLHero::SprintBegin);
 Input->BindAction(TEXT("Sprint"),IE_Released,this,&ALLHero::SprintEnd);
 Input->BindAction(TEXT("OrbitStart"),IE_Pressed,this,&ALLHero::OrbitBegin);
 Input->BindAction(TEXT("OrbitStart"),IE_Released,this,&ALLHero::OrbitEnd);
 Input->BindAction(TEXT("ZoomIn"),IE_Pressed,this,&ALLHero::ZoomIn);
 Input->BindAction(TEXT("ZoomOut"),IE_Pressed,this,&ALLHero::ZoomOut);
 Input->BindAction(TEXT("Engage"),IE_Pressed,this,&ALLHero::EngagePressed);
 Input->BindAction(TEXT("Parry"),IE_Pressed,this,&ALLHero::ParryPressed);
 Input->BindAction(TEXT("Defend"),IE_Pressed,this,&ALLHero::DodgePressed);
 Input->BindAction(TEXT("AttackBeat"),IE_Pressed,this,&ALLHero::AttackPressed);
 Input->BindAction(TEXT("NormalHit"),IE_Pressed,this,&ALLHero::NormalPressed);
 Input->BindAction(TEXT("Skill1"),IE_Pressed,this,&ALLHero::UseOne);
 Input->BindAction(TEXT("Skill2"),IE_Pressed,this,&ALLHero::UseTwo);
 Input->BindAction(TEXT("Skill3"),IE_Pressed,this,&ALLHero::UseThree);
 Input->BindAction(TEXT("Skill4"),IE_Pressed,this,&ALLHero::UseFour);
 Input->BindAction(TEXT("Skill5"),IE_Pressed,this,&ALLHero::UseFive);
 Input->BindAction(TEXT("Skill6"),IE_Pressed,this,&ALLHero::UseSix);
 Input->BindAction(TEXT("Skill7"),IE_Pressed,this,&ALLHero::UseSeven);
 Input->BindAction(TEXT("NextPage"),IE_Pressed,this,&ALLHero::NextPage);
 Input->BindAction(TEXT("EndTurn"),IE_Pressed,this,&ALLHero::EndTurn);
 Input->BindAction(TEXT("SaveGame"),IE_Pressed,this,&ALLHero::SavePressed);
 Input->BindAction(TEXT("LoadGame"),IE_Pressed,this,&ALLHero::LoadPressed);
 Input->BindAction(TEXT("OpenInventory"),IE_Pressed,this,&ALLHero::InventoryPressed);
 Input->BindAction(TEXT("OpenForge"),IE_Pressed,this,&ALLHero::ForgePressed);
 Input->BindAction(TEXT("OpenEvolution"),IE_Pressed,this,&ALLHero::EvolutionPressed);
 Input->BindAction(TEXT("OpenMap"),IE_Pressed,this,&ALLHero::MapPressed);
 Input->BindAction(TEXT("RandomBuild"),IE_Pressed,this,&ALLHero::RandomPressed);
 Input->BindAction(TEXT("Back"),IE_Pressed,this,&ALLHero::BackPressed);
 Input->BindAction(TEXT("SelectNext"),IE_Pressed,this,&ALLHero::SelectNext);
 Input->BindAction(TEXT("SelectPrev"),IE_Pressed,this,&ALLHero::SelectPrevious);
 Input->BindAction(TEXT("MouseClick"),IE_Pressed,this,&ALLHero::MousePressed);
 Input->BindKey(EKeys::B,IE_Pressed,this,&ALLHero::BossPressed);
 Input->BindKey(EKeys::K,IE_Pressed,this,&ALLHero::FinalPressed);
 Input->BindKey(EKeys::H,IE_Pressed,this,&ALLHero::HelpPressed);
 Input->BindKey(EKeys::F2,IE_Pressed,this,&ALLHero::AssistPressed);
}
void ALLHero::MoveForward(float V)
{
 if(FMath::Abs(V)<.01f)return;
 if(ULLGameInstance* GI=Game())
 {
  if(GI->Phase!=ELLPhase::Exploration||GI->bInBattle)return;
  const FRotator Yaw(0,Controller?Controller->GetControlRotation().Yaw:0,0);
  AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::X),V);
 }
}
void ALLHero::MoveRight(float V)
{
 if(FMath::Abs(V)<.01f)return;
 if(ULLGameInstance* GI=Game())
 {
  if(GI->Phase!=ELLPhase::Exploration||GI->bInBattle)return;
  const FRotator Yaw(0,Controller?Controller->GetControlRotation().Yaw:0,0);
  AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y),V);
 }
}
void ALLHero::OrbitYaw(float V){if(bOrbiting&&FMath::Abs(V)>.001f)AddControllerYawInput(V);}
void ALLHero::OrbitPitch(float V){if(bOrbiting&&FMath::Abs(V)>.001f)AddControllerPitchInput(V);}
void ALLHero::OrbitBegin(){bOrbiting=true;}
void ALLHero::OrbitEnd(){bOrbiting=false;}
void ALLHero::ZoomIn(){CameraBoom->TargetArmLength=FMath::Clamp(CameraBoom->TargetArmLength-90.f,350.f,1350.f);}
void ALLHero::ZoomOut(){CameraBoom->TargetArmLength=FMath::Clamp(CameraBoom->TargetArmLength+90.f,350.f,1350.f);}
void ALLHero::JumpPressed()
{
 if(ULLGameInstance* GI=Game())if(GI->Phase==ELLPhase::Exploration)Jump();
}
void ALLHero::SprintBegin()
{
 bSprinting=true;
 GetCharacterMovement()->MaxWalkSpeed=700.f;
}
void ALLHero::SprintEnd()
{
 bSprinting=false;
 GetCharacterMovement()->MaxWalkSpeed=440.f;
}
void ALLHero::EngagePressed()
{
 ULLGameInstance* GI=Game();if(!GI)return;
 // E is bound to Engage and Dodge. Only DodgePressed may consume enemy QTE.
 if(GI->Phase!=ELLPhase::Exploration)return;
 ALLStage* Stage=Cast<ALLGameMode>(GetWorld()->GetAuthGameMode())
  ? Cast<ALLGameMode>(GetWorld()->GetAuthGameMode())->GetStage():nullptr;
 int32 Nearby=Stage?Stage->FindNearestEnemy(GetActorLocation()):INDEX_NONE;
 if(Nearby==INDEX_NONE){GI->SetNotice(TEXT("Подойди к врагу или нажми F для быстрого боя"));return;}
 if(GI->DefeatedIds.Contains(GI->Mobs[Nearby].Id))
 {GI->SetNotice(TEXT("Этот противник уже побеждён. Иди дальше."));return;}
 GI->Engage(GetWorld(),Nearby,false,false,false,GetActorLocation());
}
void ALLHero::ParryPressed()
{
 if(ULLGameInstance* GI=Game())if(GI->Phase==ELLPhase::EnemyTiming)
 {GI->Defend(true);SetVisualClip(GuardAnim,false);ActionVisualTime=.48f;}
}
void ALLHero::DodgePressed()
{
 if(ULLGameInstance* GI=Game())if(GI->Phase==ELLPhase::EnemyTiming)
 {GI->Defend(false);SetVisualClip(DodgeAnim,false);ActionVisualTime=.47f;}
}
void ALLHero::AttackPressed()
{
 if(ULLGameInstance* GI=Game())if(GI->Phase==ELLPhase::AttackTiming)
 {GI->CompleteAttack(true);SetVisualClip(AttackAnim,false);ActionVisualTime=.42f;}
}
void ALLHero::NormalPressed()
{
 ULLGameInstance* GI=Game();if(!GI)return;
 if(GI->Phase==ELLPhase::AttackTiming)
 {GI->CompleteAttack(false);SetVisualClip(AttackAnim,false);ActionVisualTime=.42f;return;}
 if(GI->Phase==ELLPhase::Forge && GI->PendingForgeIndex>=0)
 {GI->ConfirmForge();return;}
 if(GI->Phase==ELLPhase::Menu){GI->StartRun(GetWorld());return;}
 if(GI->Phase==ELLPhase::LossChoice){GI->LevelLoss(MenuCursor);return;}
 if(GI->Phase==ELLPhase::Defeat){GI->Retry(GetWorld());return;}
 if(GI->Phase==ELLPhase::FinalVictory){GI->bInBattle=false;GI->Phase=ELLPhase::Menu;
  UGameplayStatics::OpenLevel(GetWorld(),FName(TEXT("/Game/Maps/Explore")));}
}
void ALLHero::Pick(int32 Index)
{
 ULLGameInstance* GI=Game();if(!GI)return;
 if(GI->Phase==ELLPhase::BattlePlayer)GI->UseSkill(
  Index==0?0:(Index==1?1:2+GI->SkillPage*5+Index-2));
 else if(GI->Phase==ELLPhase::Menu)
  GI->ToggleStartSkill(GI->MenuPage*5+Index);
 else if(GI->Phase==ELLPhase::LossChoice)GI->LevelLoss(GI->SkillPage*5+Index);
 else if(GI->Phase==ELLPhase::Inventory)GI->Equip(GI->ItemPage*5+Index);
 else if(GI->Phase==ELLPhase::Forge)
 {
  GI->QueueForge(GI->ItemPage*5+Index); // Second pick is a preview, not irreversible action.
 }
 else if(GI->Phase==ELLPhase::Evolution)
 {
  int32 I=GI->SkillPage*5+Index;
  if(GI->SelectedItem<0){GI->SelectedItem=I;GI->SetNotice(TEXT("Выбери второй навык для слияния"));}
  else {GI->Evolve(GI->SelectedItem,I);GI->SelectedItem=-1;}
 }
}
void ALLHero::UseOne(){Pick(0);}
void ALLHero::UseTwo(){Pick(1);}
void ALLHero::UseThree(){Pick(2);}
void ALLHero::UseFour(){Pick(3);}
void ALLHero::UseFive(){Pick(4);}
void ALLHero::UseSix(){Pick(5);}
void ALLHero::UseSeven(){Pick(6);}
void ALLHero::NextPage()
{
 ULLGameInstance* GI=Game();if(!GI)return;
 if(GI->Phase==ELLPhase::Menu)GI->MenuPage=(GI->MenuPage+1)%20;
 else if(GI->Phase==ELLPhase::Inventory||GI->Phase==ELLPhase::Forge)
  GI->ItemPage=(GI->ItemPage+1)%FMath::Max(1,FMath::CeilToInt(GI->Items.Num()/5.0f));
 else GI->SkillPage=(GI->SkillPage+1)%FMath::Max(1,
  FMath::CeilToInt(GI->GetSkills().Num()/5.0f));
}
void ALLHero::EndTurn()
{
 ULLGameInstance* GI=Game();if(!GI)return;
 if(GI->Phase==ELLPhase::BattlePlayer){GI->EndPlayerTurn();return;}
 if(GI->Phase==ELLPhase::Exploration)
 {
  int32 Index=FMath::Clamp(GI->CurrentBiome*6,0,GI->Mobs.Num()-1);
  GI->Engage(GetWorld(),Index,false,false,false,GetActorLocation());
 }
}
void ALLHero::SavePressed(){if(ULLGameInstance* GI=Game())GI->SetNotice(
 GI->SaveRun()?TEXT("Игра сохранена F5"):TEXT("Сохранение недоступно"));}
void ALLHero::LoadPressed(){if(ULLGameInstance* GI=Game())GI->LoadRun(GetWorld());}
void ALLHero::InventoryPressed(){if(ULLGameInstance* GI=Game())
 if(GI->Phase==ELLPhase::Exploration){GI->Phase=ELLPhase::Inventory;GI->ItemPage=0;}}
void ALLHero::ForgePressed(){if(ULLGameInstance* GI=Game())
 if(GI->Phase==ELLPhase::Exploration){GI->Phase=ELLPhase::Forge;GI->CancelForge();}}
void ALLHero::EvolutionPressed(){if(ULLGameInstance* GI=Game())
 if(GI->Phase==ELLPhase::Exploration){GI->Phase=ELLPhase::Evolution;GI->SelectedItem=-1;}}
void ALLHero::MapPressed(){if(ULLGameInstance* GI=Game())
 if(GI->Phase==ELLPhase::Exploration)GI->Phase=ELLPhase::Map;}
void ALLHero::BossPressed()
{
 ULLGameInstance* GI=Game();
 if(!GI||GI->Phase!=ELLPhase::Exploration)return;
 for(int32 I=0;I<GI->Bosses.Num();++I)if(GI->IsMinibossUnlocked(I))
 {GI->Engage(GetWorld(),I,true,false,false,GetActorLocation());return;}
 GI->SetNotice(TEXT("Новый мини-босс откроется по мере распада уровней."));
}
void ALLHero::FinalPressed()
{
 ULLGameInstance* GI=Game();if(!GI||GI->Phase!=ELLPhase::Exploration)return;
 GI->Engage(GetWorld(),0,false,false,true,GetActorLocation());
}
void ALLHero::RandomPressed(){if(ULLGameInstance* GI=Game())
 if(GI->Phase==ELLPhase::Menu){GI->RandomizeStartingBuild();GI->SetNotice(TEXT("Случайный билд: 50/50"));}}
void ALLHero::BackPressed()
{
 ULLGameInstance* GI=Game();if(!GI)return;
 if(GI->Phase==ELLPhase::Defeat){GI->ReturnToExploration(GetWorld());return;}
 if(GI->Phase==ELLPhase::Forge && GI->SelectedItem>=0)
 {GI->CancelForge();return;}
 if(GI->Phase==ELLPhase::Inventory||GI->Phase==ELLPhase::Forge||
  GI->Phase==ELLPhase::Evolution||GI->Phase==ELLPhase::Map)
 {GI->Phase=ELLPhase::Exploration;GI->CancelForge();}
}
void ALLHero::SelectNext()
{
 ULLGameInstance* GI=Game();if(!GI)return;
 const int32 N=GI->Phase==ELLPhase::Menu?50:GI->Innate.Num();
 MenuCursor=FMath::Clamp(MenuCursor+1,0,FMath::Max(0,N-1));
}
void ALLHero::SelectPrevious(){MenuCursor=FMath::Max(0,MenuCursor-1);}
bool ALLHero::HandleHUDClick()
{
 const APlayerController* PC=Cast<APlayerController>(Controller);if(!PC)return false;
 float X=0,Y=0;if(!PC->GetMousePosition(X,Y))return false;
 if(ALLHUD* HUD=Cast<ALLHUD>(PC->GetHUD()))return HUD->HandleClick(X,Y);
 return false;
}
void ALLHero::MousePressed(){HandleHUDClick();}
void ALLHero::HelpPressed(){bHelp=!bHelp;}
void ALLHero::AssistPressed(){if(ULLGameInstance* GI=Game())GI->CycleTimingAssist();}
