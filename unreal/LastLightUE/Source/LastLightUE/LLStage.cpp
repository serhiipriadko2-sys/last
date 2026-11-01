#include "LLStage.h"
#include "LLGameInstance.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimSequence.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Materials/Material.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"

static const FLinearColor LLBiomeBase[]={
 FLinearColor(.18f,.21f,.32f), FLinearColor(.10f,.25f,.32f),
 FLinearColor(.23f,.22f,.31f), FLinearColor(.10f,.27f,.35f),
 FLinearColor(.21f,.14f,.29f)
};
static const FLinearColor LLBiomeAccent[]={
 FLinearColor(.99f,.42f,.23f), FLinearColor(.22f,.83f,.91f),
 FLinearColor(.90f,.68f,.41f), FLinearColor(.41f,.78f,.84f),
 FLinearColor(.78f,.48f,.96f)
};
ALLStage::ALLStage()
{
 PrimaryActorTick.bCanEverTick=false;
 StageRoot=CreateDefaultSubobject<USceneComponent>(TEXT("StageRoot"));
 // All procedural world geometry uses Static mobility; its parent must also be Static.
 // Otherwise UE refuses attachment, producing an invisible/collisionless map.
 StageRoot->SetMobility(EComponentMobility::Static);
 RootComponent=StageRoot;
 static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxRef(TEXT("/Engine/BasicShapes/Cube.Cube"));
 static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereRef(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
 static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderRef(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
 static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeRef(TEXT("/Engine/BasicShapes/Cone.Cone"));
 static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialRef(
  TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
 if(BoxRef.Succeeded())Cube=BoxRef.Object;
 if(SphereRef.Succeeded())Sphere=SphereRef.Object;
 if(CylinderRef.Succeeded())Cylinder=CylinderRef.Object;
 if(ConeRef.Succeeded())Cone=ConeRef.Object;
 if(MaterialRef.Succeeded())ShapeMaterial=MaterialRef.Object;
}
void ALLStage::BeginPlay(){Super::BeginPlay();}

UStaticMeshComponent* ALLStage::AddMesh(UStaticMesh* Shape,const FVector& Pos,
 const FVector& Scale,const FLinearColor& Tint,bool bSolid)
{
 if(!Shape)return nullptr;
 UStaticMeshComponent* Part=NewObject<UStaticMeshComponent>(this);
 Part->SetMobility(EComponentMobility::Static);
 Part->SetStaticMesh(Shape);
 Part->SetRelativeLocation(Pos);Part->SetRelativeScale3D(Scale);
 Part->SetupAttachment(StageRoot);
 Part->SetCollisionEnabled(bSolid?ECollisionEnabled::QueryAndPhysics:ECollisionEnabled::NoCollision);
 Part->SetCollisionResponseToAllChannels(bSolid?ECR_Block:ECR_Ignore);
 Part->SetCastShadow(bSolid);
 if(ShapeMaterial)
 {
  UMaterialInstanceDynamic* Dynamic=UMaterialInstanceDynamic::Create(ShapeMaterial,this);
  Dynamic->SetVectorParameterValue(TEXT("Color"),Tint);
  Dynamic->SetVectorParameterValue(TEXT("BaseColor"),Tint);
  Part->SetMaterial(0,Dynamic);
 }
 Part->RegisterComponent();
 ++GeometryCount;
 return Part;
}
void ALLStage::AddBatch(UStaticMesh* Shape,const TArray<FTransform>& Transforms,
 const FLinearColor& Tint)
{
 if(!Shape||Transforms.IsEmpty())return;
 UInstancedStaticMeshComponent* Batch=NewObject<UInstancedStaticMeshComponent>(this);
 Batch->SetStaticMesh(Shape);
 Batch->SetupAttachment(StageRoot);
 Batch->SetMobility(EComponentMobility::Static);
 Batch->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 Batch->SetCastShadow(false);
 if(ShapeMaterial)
 {
  auto* Mat=UMaterialInstanceDynamic::Create(ShapeMaterial,this);
  Mat->SetVectorParameterValue(TEXT("Color"),Tint);
  Mat->SetVectorParameterValue(TEXT("BaseColor"),Tint);
  Batch->SetMaterial(0,Mat);
 }
 Batch->RegisterComponent();
 for(const FTransform& Transform:Transforms)Batch->AddInstance(Transform);
 GeometryCount+=Transforms.Num();
}
void ALLStage::AddLighting(bool bBattle,int32 Biome)
{
 UDirectionalLightComponent* Sun=NewObject<UDirectionalLightComponent>(this);
 Sun->SetupAttachment(StageRoot);
 Sun->SetRelativeRotation(FRotator(-58.f,-44.f,0.f));
 Sun->SetLightColor(FLinearColor(.94f,.85f,.74f));
 Sun->SetIntensity(bBattle?4.0f:3.3f);
 Sun->SetCastShadows(true);Sun->RegisterComponent();
 UPointLightComponent* Key=NewObject<UPointLightComponent>(this);
 Key->SetupAttachment(StageRoot);
 Key->SetRelativeLocation(bBattle?FVector(0,0,540):FVector(3200,0,850));
 Key->SetIntensity(bBattle?3100.0f:800.0f);
 Key->SetAttenuationRadius(bBattle?3000.0f:4400.0f);
 Key->SetLightColor(LLBiomeAccent[FMath::Clamp(Biome,0,4)]);
 Key->SetCastShadows(false);Key->RegisterComponent();
 if(bBattle)
 {
  UPointLightComponent* HeroFill=NewObject<UPointLightComponent>(this);
  HeroFill->SetupAttachment(StageRoot);
  HeroFill->SetRelativeLocation(FVector(-250,-650,460));
  HeroFill->SetIntensity(4800.f);
  HeroFill->SetAttenuationRadius(1700.f);
  HeroFill->SetLightColor(FLinearColor(.66f,.81f,1.f));
  HeroFill->SetCastShadows(false);
  HeroFill->RegisterComponent();
  UPointLightComponent* EnemyRim=NewObject<UPointLightComponent>(this);
  EnemyRim->SetupAttachment(StageRoot);
  EnemyRim->SetRelativeLocation(FVector(360,750,450));
  EnemyRim->SetIntensity(5400.f);
  EnemyRim->SetAttenuationRadius(1800.f);
  EnemyRim->SetLightColor(FLinearColor(1.f,.78f,.55f));
  EnemyRim->SetCastShadows(false);
  EnemyRim->RegisterComponent();
  // Camera-facing background relief is beyond the first key light's attenuation.
  // A bounded, non-shadowing fill prevents the distant wall looking like a black void.
  UPointLightComponent* BackdropFill=NewObject<UPointLightComponent>(this);
  BackdropFill->SetupAttachment(StageRoot);
  BackdropFill->SetRelativeLocation(FVector(0.f,2750.f,900.f));
  BackdropFill->SetIntensity(8000.f);
  BackdropFill->SetAttenuationRadius(2250.f);
  BackdropFill->SetLightColor(FLinearColor(.68f,.80f,1.f));
  BackdropFill->SetCastShadows(false);
  BackdropFill->RegisterComponent();
 }
}

void ALLStage::BuildExplore()
{
 GeometryCount=0;SkinnedEncounterCount=0;EnemyPositions.Empty();SceneVariant=0;
 FRandomStream Rand(44301);
 const float ZoneLength=2000.0f;
 // Traversable roads and optional side paths remain collision-enabled.
 for(int32 Zone=0;Zone<5;++Zone)
 {
  const float X=Zone*ZoneLength+800;
  const FLinearColor Accent=LLBiomeAccent[Zone];
  const FLinearColor Base=LLBiomeBase[Zone];
  AddMesh(Cube,FVector(X,0,-85),FVector(20.0f,10.0f,1.7f),
   FLinearColor(.20f,.29f,.40f),true);
  AddMesh(Cube,FVector(X,-780,-160),FVector(20.0f,4.3f,1.3f),Base,true);
  AddMesh(Cube,FVector(X,780,-160),FVector(20.0f,4.3f,1.3f),Base,true);
  for(int32 I=0;I<10;++I)
  {
   float XX=Zone*ZoneLength+float(I)*200.0f+100;
   AddMesh(Cube,FVector(XX,-430,4),FVector(1.65f,.28f,.14f),Accent);
   AddMesh(Cube,FVector(XX,430,4),FVector(1.65f,.28f,.14f),Accent);
   if(I%2==0)
    AddMesh(Cone,FVector(XX,0,10),FVector(.27f,.27f,.5f),
     FLinearColor(.82f,.68f,.33f));
  }
  // A visible raised optional detour near the middle of each biome.
  AddMesh(Cube,FVector(X,560,230),FVector(8.0f,2.0f,.6f),
   FLinearColor(.38f,.47f,.59f),true);
  AddMesh(Cube,FVector(X-810,560,115),FVector(5.0f,2.0f,.16f),
   FLinearColor(.42f,.47f,.57f),true);
  for(int32 I=0;I<6;++I)
  {
   FVector Spawn(Zone*ZoneLength+300+I*260.f,(I%2==0?260.f:-260.f),120.f);
    EnemyPositions.Add(Spawn);
    // Original enemy species index is stable even when dead enemies disappear.
    AddMesh(Cylinder,Spawn+FVector(0,0,-111),FVector(.72f,.72f,.15f),Accent);
    ULLGameInstance* GI=GetWorld()?GetWorld()->GetGameInstance<ULLGameInstance>():nullptr;
    const int32 MobIndex=Zone*6+I;
    if(GI&&GI->Mobs.IsValidIndex(MobIndex)
      &&GI->DefeatedIds.Contains(GI->Mobs[MobIndex].Id))continue;
    const FString Shape=GI&&GI->Mobs.IsValidIndex(MobIndex)
      ?GI->Mobs[MobIndex].Shape.ToLower():TEXT("wolf");
    FString Model=TEXT("knight");
    if(Shape.Contains(TEXT("wolf"))||Shape.Contains(TEXT("hound")))Model=TEXT("wolf");
    else if(Shape.Contains(TEXT("moth"))||Shape.Contains(TEXT("wasp"))
      ||Shape.Contains(TEXT("bee"))||Shape.Contains(TEXT("insect")))Model=TEXT("wasp");
    else if(Shape.Contains(TEXT("golem"))||Shape.Contains(TEXT("stone"))
      ||Shape.Contains(TEXT("giant")))Model=TEXT("golem");
    else if(Shape.Contains(TEXT("wraith"))||Shape.Contains(TEXT("specter"))
      ||Shape.Contains(TEXT("ghost"))||Shape.Contains(TEXT("shadow")))Model=TEXT("wraith");
    const FString BasePath=FString::Printf(
      TEXT("/Game/Characters/Joined/%s/%s/SkeletalMeshes/%s"),*Model,*Model,*Model);
    USkeletalMesh* Skin=LoadObject<USkeletalMesh>(nullptr,*(BasePath+TEXT(".")+Model));
    if(Skin)
    {
     auto* Mob=NewObject<USkeletalMeshComponent>(this);
     Mob->SetupAttachment(StageRoot);
     Mob->SetMobility(EComponentMobility::Movable);
     Mob->SetSkeletalMeshAsset(Skin);
     Mob->SetRelativeLocation(Spawn+FVector(0,0,-116));
     Mob->SetRelativeRotation(FRotator(0,Spawn.Y>0?-90.f:90.f,0));
     Mob->SetRelativeScale3D(Model==TEXT("wasp")?FVector(.47):
       (Model==TEXT("wolf")?FVector(.52):FVector(.63)));
     Mob->SetCollisionEnabled(ECollisionEnabled::NoCollision);
     Mob->SetCastShadow(false);
     Mob->RegisterComponent();
     UAnimSequence* Idle=LoadObject<UAnimSequence>(nullptr,
       *(BasePath+TEXT("Idle.")+Model+TEXT("Idle")));
     if(Idle)Mob->PlayAnimation(Idle,true);
     ++SkinnedEncounterCount;
    }
    else
    {
     AddMesh(Cube,Spawn+FVector(0,0,18),FVector(.48f,.42f,.80f),Base);
     AddMesh(Sphere,Spawn+FVector(0,0,96),FVector(.32f,.32f,.32f),
       FLinearColor(.83f,.87f,.91f));
    }
    // LL_WORLD_RIG_ACTOR
  }
  TArray<FTransform> Cliffs,Spikes;
  for(int32 I=0;I<68;++I)
  {
   float RX=Zone*ZoneLength+Rand.FRandRange(10,1900);
   float Side=I%2==0?1.f:-1.f;
   float RY=Side*Rand.FRandRange(1250,2800);
   float H=Rand.FRandRange(3.f,14.f);
   Cliffs.Add(FTransform(FRotator(0,Rand.FRandRange(-180,180),0),
    FVector(RX,RY,H*50-180),FVector(Rand.FRandRange(1.3,4.0),Rand.FRandRange(1,3),H)));
  }
  for(int32 I=0;I<30;++I)
  {
   float RX=Zone*ZoneLength+Rand.FRandRange(30,1900);
   float RY=(I%2==0?1.f:-1.f)*Rand.FRandRange(1000,1600);
   float H=Rand.FRandRange(1.0,3.0);
   Spikes.Add(FTransform(FRotator::ZeroRotator,FVector(RX,RY,50*H),
    FVector(.42f,.42f,H)));
  }
  AddBatch(Cube,Cliffs,Base);
  AddBatch(Cone,Spikes,Accent);
  AddMesh(Cube,FVector(Zone*ZoneLength+1850,0,50),FVector(.85f,12.f,1.4f),Accent);
 }
 AddLighting(false,0);
 UE_LOG(LogTemp,Display,TEXT("LL_EXPLORE_STAGE: 5 zones, 30 encounters, %d mesh instances"),GeometryCount);
 UE_LOG(LogTemp,Display,TEXT("LL_WORLD_RIGS=%d expected_unbeaten=30-minus-defeated"),SkinnedEncounterCount);
}

void ALLStage::BuildArena(int32 Biome,bool bMirror)
{
 GeometryCount=0;EnemyPositions.Empty();SceneVariant=Biome;
 const int32 Z=FMath::Clamp(Biome,0,4);
 const FLinearColor Base=LLBiomeBase[Z],Accent=LLBiomeAccent[Z];
 // Low-profile surrounding ground gives the arena visual depth instead of pure black void.
 AddMesh(Cylinder,FVector(0,0,-265),FVector(89.0f,89.0f,1.4f),
   FLinearColor(.07f,.12f,.19f),false);
 AddMesh(Cylinder,FVector(0,0,-100),FVector(14.0f,14.0f,1.7f),Base,true);
 // Distant stage wall replaces featureless blackness beyond the combat ring.
 // Non-colliding, behind both fighters, does not intersect camera or arena floor.
 AddMesh(Cube,FVector(0,3500,600),FVector(66.f,1.0f,13.f),
   FLinearColor(.13f,.19f,.28f));
 AddMesh(Cube,FVector(0,3450,1320),FVector(66.f,1.15f,1.4f),
   FLinearColor(.23f,.29f,.38f));
 for(int32 Col=0;Col<9;++Col)
 {
  const float X=-3000.f+Col*750.f;
  AddMesh(Cube,FVector(X,3430,500),FVector(.6f,1.2f,9.f),
    FLinearColor(.22f,.28f,.35f));
 }
 AddMesh(Cylinder,FVector(0,0,0),FVector(12.0f,12.0f,.20f),
  FLinearColor(.32f,.36f,.44f),true);
 for(int32 I=0;I<16;++I)
 {
  const float A=2.f*PI*I/16.f;
  // Keep the decorative columns outside the entire combat camera frustum.
  // The previous 1280cm ring included a pillar directly between camera and fighters.
  FVector P(FMath::Cos(A)*1850.f,FMath::Sin(A)*1850.f,0.f);
  AddMesh(Cylinder,P+FVector(0,0,120),FVector(.35f,.35f,2.2f),Base);
  AddMesh(Cone,P+FVector(0,0,285),FVector(.42f,.42f,1.15f),Accent);
  AddMesh(Cube,FVector(FMath::Cos(A)*1030,FMath::Sin(A)*1030,12),
   FVector(1.f,.24f,.10f),Accent);
 }
 // Separate champion and adversary pedestals, not present in exploration.
 AddMesh(Cylinder,FVector(0,-520,48),FVector(1.65f,1.65f,.15f),Accent);
 AddMesh(Cylinder,FVector(0,520,48),FVector(1.65f,1.65f,.15f),Accent);
 if(bMirror)
 for(int32 I=0;I<9;++I)
 {
  float A=2*PI*I/9;
  AddMesh(Cube,FVector(FMath::Cos(A)*1600,FMath::Sin(A)*1600,340),
   FVector(1.2f,.3f,6.0f),Accent);
 }
 AddLighting(true,Z);
 UE_LOG(LogTemp,Display,TEXT("LL_ARENA_STAGE: biome=%d mirror=%d meshes=%d"),Z,bMirror,GeometryCount);
}
int32 ALLStage::FindNearestEnemy(FVector Location,float MaxDistance) const
{
 int32 Best=-1;float BestSq=MaxDistance*MaxDistance;
 const ULLGameInstance* GI=GetWorld()?GetWorld()->GetGameInstance<ULLGameInstance>():nullptr;
 for(int32 I=0;I<EnemyPositions.Num();++I)
 {
  if(GI&&GI->Mobs.IsValidIndex(I)&&GI->DefeatedIds.Contains(GI->Mobs[I].Id))
   continue;
  float Dist=FVector::DistSquared2D(Location,EnemyPositions[I]);
  if(Dist<BestSq){BestSq=Dist;Best=I;}
 }
 return Best;
}
