// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/ItemTypes.h"
#include "FHItemListBase.generated.h"

class UButton;
class UTextBlock;
class UImage;
enum class EUIType : uint8;
class UInventoryItemBase;
class UFHItemBase;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHItemListBase : public UUserWidget
{
	GENERATED_BODY()

private:
	void SetBorder(bool Active);

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintNativeEvent)
	void InitLayOut(const EUIType& UIType);
	void InitLayOut_Implementation(const EUIType& UIType);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<class UBorder> Border;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	//TObjectPtr<UTextBlock> ItemStack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	//TObjectPtr<UTextBlock> ItemDescription;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FFHItemData CachedItemData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> Price;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> CanBuyCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> BuyCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BuyCount")
	int32 Count = 0;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UItemDragImagePayloadBase> DragPayloadClass;

	UPROPERTY()
	UItemDragImagePayloadBase* ItemPayload = nullptr;

	int32 ItemStackValue = 0;
	int32 MaxBuyCount = 0;
	int32 CachedMaxBuyCount = 0;
	int32 Cost;
	FName ItemID;
	bool bIsBuyListUI = false;

	UPROPERTY()
	TObjectPtr<class UFHBuyUIBase> BuyUI;

	UPROPERTY()
	TObjectPtr<class UFHSaleUIBase> SaleUI;

	UFUNCTION(BlueprintCallable)
	void OnCountSubButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnCountAddButtonClicked();

	void OnCountValueChanged();
	void OnMaxBuyCountChanged();

	void ResetCount();

	UFUNCTION(BlueprintImplementableEvent)
	void SetSoldOut();
};
