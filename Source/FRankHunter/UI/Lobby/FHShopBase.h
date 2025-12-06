// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUIUtils.h"
#include "FHShopBase.generated.h"

enum class EPurchaseError : uint8;
enum class EItemMoveType : uint8;
enum class EUIType : uint8;
enum class EItemType : uint8;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHShopBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	void InitShopList();
	void OnShopBuyCountChanged(const TArrayView<int32>& ChangedIndices, int32 FinalSize);
	void OnShopBuyCountChanged(const FName& ItemID, const int32 BuyCount);

	void OnShopSellItemAdded(int32 InventoryIndex);
	void OnShopSellItemRemoved(const TArray<int32>& Indices);
	void OnShopSellItemChanged(const TArray<int32>& Indices);

	UFUNCTION()
	void OnItemDroppedOnSaleUI(FName ItemID, int32 ItemStack, int32 InventoryIndex, EItemMoveType MoveType = EItemMoveType::InventoryToShop);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OpenUI(const EUIType& UIType);
	void OpenUI_Implementation(const EUIType& UIType);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowPurchaseResultMessage(const EPurchaseError& Result);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UIWidget", meta = (BindWidget))
	TObjectPtr<class UFHBuyUIBase> BuyUI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UIWidget", meta = (BindWidget))
	TObjectPtr<class UFHSaleUIBase> SaleUI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UIWidget", meta = (BindWidget))
	TObjectPtr<class UFHSellCountBase> SellCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UIWidget", meta = (BindWidget))
	TObjectPtr<class UInventoryBase> CartInventoryUI;
};
