// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "LobbyUIUtils.h"
#include "FHBuyUIBase.generated.h"

class UBorder;
class UFHItemListBase;
class UTextBlock;
enum class EItemType : uint8;
class UFHBuyRPCComponent;
class UFHItemRowSlot;
struct FFHItemData;

USTRUCT()
struct FPurchaseData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FName ItemID;
	UPROPERTY()
	int32 ItemCount = 0;
};

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHBuyUIBase : public UCommonActivatableWidget, public IKioskDelegateClass
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct();

	void InitShopList();
	// Client Only
	void OnShopBuyCountChanged(const TArrayView<int32>& ChangedIndices, int32 FinalSize);
	// Server Only
	void OnShopBuyCountChanged(const FName& ItemID, const int32 BuyCount);
	UFHItemListBase* CreateItemListInstance();
	UFUNCTION()
	void OnPlayerMoneyChanged(int32 Money);

	UFUNCTION(BlueprintCallable)
	void BackToHome();

	UFUNCTION(BlueprintCallable)
	void OpenUI();

	UFUNCTION(BlueprintCallable)
	void ItemTypeTapClicked(int32 TapIndex);
	void GetCachedItemList(EItemType ItemType);
	void AddToShoppingCart(FName ItemID, int32 Price);
	void RemoveFromShoppingCart(FName ItemID, int32 Price);

	UFUNCTION(BlueprintCallable)
	void ResetCart();

	void OnTotalPriceChanged();

	UFUNCTION(BlueprintCallable)
	void OnPurchaseButtonClicked();

	UFUNCTION(BlueprintImplementableEvent)
	void AddItemInGrid(UFHItemListBase* Item);
	UFUNCTION(BlueprintImplementableEvent)
	void ResetGrid();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemListWidget")
	TSubclassOf<UFHItemListBase> ItemListWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemListWidget")
	TSubclassOf<UFHItemRowSlot> ItemRowSlotWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UBorder> Equip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UBorder> Use;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UBorder> Throw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UBorder> Install;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	//TObjectPtr<class UScrollBox> ItemScrollList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> TotalPayment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentMoney;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DataTable")
	TObjectPtr<class UDataTable> ItemTable;

	UPROPERTY()
	TMap<FName, UFHItemListBase*> CachedEquipList;

	UPROPERTY()
	TMap<FName, UFHItemListBase*> CachedUseList;

	UPROPERTY()
	TMap<FName, UFHItemListBase*> CachedThrowList;

	UPROPERTY()
	TMap<FName, UFHItemListBase*> CachedInstallList;

	UPROPERTY()
	TMap<FName, int32> ShoppingCart;
	UPROPERTY()
	TArray<FPurchaseData> PurchaseItems;
	int32 TotalPrice = 0;

	uint32 bIsInitialized : 1 {false};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="RPCComponent")
	TObjectPtr<UFHBuyRPCComponent> RPCComponent;
};
