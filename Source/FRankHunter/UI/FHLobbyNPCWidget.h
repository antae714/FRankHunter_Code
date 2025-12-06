// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "FHLobbyNPCWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHLobbyNPCWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<class UButton> PayFeeBtn;
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<class UButton> WhatBtn;
    
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<class UTextBlock> DialogBox;

    UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UInputAction> CancelAction;
	UPROPERTY()
	TObjectPtr<class UInputMappingContext> InputMappingContext;

    UPROPERTY()
    class AFHLobbyNPC* WidgetMaster{ nullptr };

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widgets", meta = (BindWidget))
	TObjectPtr<class UFHBuyUIBase> BuyUI;
public:
    virtual void NativeConstruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    UFUNCTION(BlueprintImplementableEvent)
    void BackToHome();

    UFUNCTION(BlueprintImplementableEvent)
    void ShowReturnResult(bool IsRankUp);

    UFUNCTION(BlueprintImplementableEvent)
	void HandlePurchaseError(const EPurchaseError& Error);

    UFUNCTION()
    void PayFee();

    UFUNCTION()
    void Sell();

    void OnFocus(class AFHPlayerBase* player, class AFHPlayerController* pc);
    void OnEscapeKeyPressed();
};
