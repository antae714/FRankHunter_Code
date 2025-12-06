// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUIUtils.h"
#include "FHSellUIBase.generated.h"

class UTextBlock;
class UButton;
class UScrollBox;

USTRUCT(BlueprintType)
struct FCachedSellItemInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int32 InventoryIndex = 0;

	UPROPERTY()
	int32 ItemStack = 0;

	UPROPERTY()
	FName ItemID;
};

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHSellUIBase : public UUserWidget, public IKioskDelegateClass
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void OpenUI();

	UFUNCTION(BlueprintCallable)
	void SellAll();

	UFUNCTION(BlueprintCallable)
	void Sell();

	UFUNCTION(BlueprintCallable)
	void BackToHome();

	UFUNCTION()
	void SellItemAdded(FName ItemID);

	UFUNCTION()
	void SellItemRemoved(FName ItemID);

	UFUNCTION()
	void InventoryChanged(class USiInventoryComponent* InventoryComponent, int32 index);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<class UScrollBox> SellInventoryScroll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChildWidget")
	TSubclassOf<class UFHItemRowSlot> SellSlotClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> TotalSellPrice;

	UPROPERTY()
	TSoftObjectPtr<UDataTable> ItemDataTable;

	// if increase sell item count -> add to this
	TArray<FCachedSellItemInfo> SellItemInfo;

	int32 TotalInventorySize;
	int32 TotalSellPriceValue = 0;

	void OnTotalSellPriceChanged();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CountPerRow")
	int32 CountPerRow = 4;

	uint32 bIsScrollListConstructed : 1 {false};
};
