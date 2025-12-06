// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FHSaleUIBase.generated.h"

class UFHItemListBase;
enum class EItemMoveType : uint8;

DECLARE_DYNAMIC_DELEGATE_FourParams(FOnItemDropped, FName, ItemID, int32, CurItemStack, int32, InventoryIndex, EItemMoveType, MoveType);

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHSaleUIBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FOnItemDropped OnItemDroppedDelegate;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	void OnSellItemAdded(int32 InventoryIndex);
	void OnSellItemRemoved(const TArray<int32>& Indices);
	void OnSellItemChanged(const TArray<int32>& Indices);

	UFUNCTION(BlueprintCallable)
	void OpenUI();

	UFUNCTION(BlueprintCallable)
	void CloseUI();

	UFUNCTION(BlueprintCallable)
	void OnSaleSuccess();

	void OnTotalPriceChanged();

	UFUNCTION(BlueprintCallable)
	void OnSaleButtonClicked();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<class UTextBlock> TotalPayment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<class UScrollBox> ItemScrollList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DataTable")
	TObjectPtr<class UDataTable> ItemTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemListWidget")
	TSubclassOf<UFHItemListBase> ItemListWidgetClass;

	UPROPERTY()
	TArray<UFHItemListBase*> CachedItemList;

	int32 TotalPrice = 0;
	int32 OpenPlayerCount = 0;
};
