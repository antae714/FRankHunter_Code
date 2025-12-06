// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FHPaymentBase.generated.h"

class UTextBlock;
class UButton;
class USlider;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHPaymentBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void OpenUI();

	// 현재 보유 금액
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> MoneyCanUse;

	// 목표 할당량
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> PayGoal;

	// 남은 납부 목표
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> RemainingPayment;

	// 납부 할 금액 나타낼 텍스트 위젯
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> PayText;

	// 슬라이더
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<USlider> PaySlider;

	// 슬라이더 최대 범위
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxPlayerMoney;

	// 납부 후 남은 할당량
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> RemainingQuota;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UButton> Pay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UButton> Cancel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Payment")
	int32 NeedToPay;
};
