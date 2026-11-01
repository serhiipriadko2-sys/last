#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LLHUD.generated.h"
class UFont;
class ULLGameInstance;
class SWidget;
struct FLLClickZone
{
 FBox2D Box;
 FString Command;
 int32 Index=-1;
};
UCLASS()
class LASTLIGHTUE_API ALLHUD : public AHUD
{
 GENERATED_BODY()
public:
 virtual void DrawHUD() override;
 virtual void EndPlay(const EEndPlayReason::Type Reason) override;
 bool HandleClick(float ScreenX,float ScreenY);
 int32 ButtonsThisFrame=0;
private:
 TArray<FLLClickZone> Zones;
 UPROPERTY() TObjectPtr<UFont> DisplayFont;
 float Scale=1.f;
 TSharedPtr<SWidget> SearchWidget;
 float SearchScale=0.f;
 void UpdateSearch(ULLGameInstance* GI);
 void DrawPause(const ULLGameInstance* GI);
 void Fill(float X,float Y,float W,float H,FLinearColor Color);
 void Text(const FString& Str,float X,float Y,
  FLinearColor Color=FLinearColor::White,float Size=1.3f);
 void Panel(float X,float Y,float W,float H,const FString& Label);
 void Bar(float X,float Y,float W,float H,float Value,float MaxValue,FLinearColor Tint);
 void Button(float X,float Y,float W,float H,const FString& Label,const FString& Command,
  int32 Index=-1,bool bHighlight=false);
 void DrawStats(const ULLGameInstance* GI);
 void DrawWorld(const ULLGameInstance* GI);
 void DrawBattle(const ULLGameInstance* GI);
 void DrawTiming(const ULLGameInstance* GI);
 void DrawMainMenu(const ULLGameInstance* GI);
 void DrawSkillLoss(const ULLGameInstance* GI);
 void DrawInventory(const ULLGameInstance* GI);
 void DrawResult(const ULLGameInstance* GI);
 void DrawLog(const ULLGameInstance* GI);
 void DrawControlHelp(const ULLGameInstance* GI);
};
