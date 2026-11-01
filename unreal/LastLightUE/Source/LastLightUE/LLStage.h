#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LLStage.generated.h"
class UStaticMesh;
class UStaticMeshComponent;
class USceneComponent;
class UMaterialInterface;

UCLASS()
class LASTLIGHTUE_API ALLStage : public AActor
{
 GENERATED_BODY()
public:
 ALLStage();
 virtual void BeginPlay() override;
 void BuildExplore();
 void BuildArena(int32 Biome,bool bMirror);
 int32 FindNearestEnemy(FVector HeroLocation,float MaxDistance=360.0f) const;
 FVector EnemyLocation() const {return FVector(0,470,130);}
 const TArray<FVector>& GetEnemyPositions() const {return EnemyPositions;}
 int32 GeometryCount=0;
 int32 SkinnedEncounterCount=0;
 int32 SceneVariant=0;
private:
 UPROPERTY() TObjectPtr<USceneComponent> StageRoot;
 UPROPERTY() TObjectPtr<UStaticMesh> Cube;
 UPROPERTY() TObjectPtr<UStaticMesh> Sphere;
 UPROPERTY() TObjectPtr<UStaticMesh> Cylinder;
 UPROPERTY() TObjectPtr<UStaticMesh> Cone;
 UPROPERTY() TObjectPtr<UMaterialInterface> ShapeMaterial;
 TArray<FVector> EnemyPositions;
 UStaticMeshComponent* AddMesh(UStaticMesh* Shape,const FVector& Pos,
  const FVector& Scale,const FLinearColor& Tint,bool bSolid=false);
 void AddBatch(UStaticMesh* Shape,const TArray<FTransform>& Transforms,
  const FLinearColor& Tint);
 void AddLighting(bool bBattle,int32 Biome);
};
