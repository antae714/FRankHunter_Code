// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "InventoryItemBase.generated.h"

class UPanelWidget;
class UTextBlock;
class USizeBox;
class UImage;
class USiInventoryComponent;
class UFHInventoryComponent;
class UFHItemBase;
class UItemDragPayloadBase;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UInventoryItemBase : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	void SetItemInfo(USiInventoryComponent* Inventory, UFHItemBase* itemInstance, int32 stack, int32 itemIndex);
	UFHInventoryComponent* GetInventoryComponent() const;
	UFHItemBase* GetItemInstance() const;
	bool IsItemLock() const;
	int32 GetInventoryIndex() const;

	UFUNCTION(BlueprintCallable)
	void SwapItem(UInventoryItemBase* otherItemUIBase);



public:

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> ItemNoneImage;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemDragPayloadBase> DragPayloadClass;
	
	UPROPERTY()
	TObjectPtr<UItemDragPayloadBase> DragPayloadInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemStack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ItemImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> LockImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TWeakObjectPtr<UFHInventoryComponent> CachedInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TWeakObjectPtr<UFHItemBase> CachedItemInstance;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CachedStack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CachedItemIndex;
};


UCLASS()
class FRANKHUNTER_API UItemDragPayloadBase : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	void SetPayloadWidget(UInventoryItemBase* Owner);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSetPayloadWidget(UInventoryItemBase* Owner);

	virtual void NativeConstruct() override;


protected:

	UPROPERTY()
	TObjectPtr<UInventoryItemBase> OwnerWidget;


};

UCLASS()
class FRANKHUNTER_API UItemDragImagePayloadBase : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetItemIcon(TSoftObjectPtr<UTexture2D> SoftTexture);
	virtual void NativeConstruct() override;

	UPROPERTY()
	TObjectPtr<class UFHItemListBase> OwnerWidget;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USizeBox> IconWidgetSlot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;
};
