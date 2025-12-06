// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/FHObserverHud.h"
#include "Components/TextBlock.h"
#include "Core/FHPlayerController.h"
#include "Player/FHObserverPawn.h"
#include "Core/FHPlayerStateBase.h"

void UFHObserverHud::NativeConstruct()
{
    Super::NativeConstruct();

    AFHPlayerController* Controller = GetOwningPlayer<AFHPlayerController>();
    if (!Controller)
    {
        return;
    }
    AFHPlayerStateBase* PS = Controller->GetPlayerState<AFHPlayerStateBase>();
    if (!PS)
    {
        return;
    }
    PS->OnPlayerGateStateChangedDelegate.BindDynamic(this, &UFHObserverHud::PlayerGateStateChanged);
}

void UFHObserverHud::UpdateTimeText(FText text, FString LeftTime)
{
    FText NewText = FText::Format(text, FText::FromString(LeftTime));
    TimeText->SetText(NewText);
}

void UFHObserverHud::UpdateCoreText(FText text, int32 LeftCore)
{
    FText NewText = FText::Format(text, LeftCore);
    CoreText->SetText(NewText);
}

//void UFHObserverHud::UpdateQuotaText(int32 CurrentMoney, int32 RequiredMoney)
//{
//    FString TimeString = FString::Printf(TEXT("%d / %d"), CurrentMoney, RequiredMoney);
//    FText DisplayText = FText::FromString(FString::Printf(TEXT("할당량 : %s"), *TimeString));
//    QuotaText->SetText(DisplayText);
//}

void UFHObserverHud::UpdateObservingTargetName(APawn* NewTarget)
{
    if (NewTarget)
    {
        AFHPlayerStateBase* TargetPS = NewTarget->GetPlayerState<AFHPlayerStateBase>();
        if (TargetPS)
        {
            FString PlayerName = TargetPS->GetPlayerName();
            FText DisplayText = FText::Format(FText::FromString(TEXT("관전중 : {0}")), FText::FromString(PlayerName));
            ObservingNameText->SetText(DisplayText);
        }
    }
    else
    {
        ObservingNameText->SetText(FText::FromString(TEXT("관전 대상 없음")));
    }
}
