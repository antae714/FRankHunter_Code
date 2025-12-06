// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FHObserverHud.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHObserverHud : public UUserWidget
{
	GENERATED_BODY()
	
public:	
    virtual void NativeConstruct() override;

    void UpdateTimeText(FText text, FString LeftTime);
    void UpdateCoreText(FText text, int32 LeftCore);

    // TODO : Localizing
    UFUNCTION(BlueprintImplementableEvent)
    void UpdateQuotaText(int32 CurrentMoney, int32 RequiredMoney);
    void UpdateObservingTargetName(APawn* NewTarget);

	UFUNCTION(BlueprintImplementableEvent, Category = "ObserverHud")
    void UpdateRescuedInfo();
  
    UFUNCTION(BlueprintImplementableEvent, Category = "ObserverHud")
    void PlayerGateStateChanged(class AFHPlayerStateBase* Player);
protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimeText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CoreText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* QuotaText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ObservingNameText;
};
