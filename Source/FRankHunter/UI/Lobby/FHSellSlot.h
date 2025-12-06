// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FHSellSlot.generated.h"

class UBorder;
class UTextBlock;

DECLARE_DELEGATE_OneParam(FIncreaseSellPrice, FName);
DECLARE_DELEGATE_OneParam(FDecreaseSellPrice, FName);

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHSellSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FIncreaseSellPrice IncreaseSellPriceDelegate;
	FDecreaseSellPrice DecreaseSellPriceDelegate;

	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void SetItemStack(int32 Stack);
	void OnOffSelected(bool On);

	/*
		if item is deleted when inventory replicated, call this function
	*/
	void ResetSlot();

	UFUNCTION(BlueprintCallable)
	void OnDecreaseButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnIncreaseButtonClicked();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<UBorder> ItemIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> SellCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemStack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<UBorder> SellCountSlot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data")
	int32 SellCountValue = 0;

	void SetItemIcon();
	void OnSellCountChanged();
	void OnItemStackChanged();

	FSlateBrush DefaultBrush;
	FName ItemID;
	int32 ItemStackValue;
	uint32 bIsFullSelected : 1 {false};
	uint32 bIsSelected : 1 {false};
	uint32 bHasItem : 1 {false};
};
