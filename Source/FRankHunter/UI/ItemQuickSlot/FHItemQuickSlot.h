// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "FHItemQuickSlot.generated.h"

class UHorizontalBox;

class UFHItemQuickSlotElement;
class USiInventoryComponent;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHItemQuickSlot : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnInventoryChanged(USiInventoryComponent* InventoryComponent, int32 index);

	UFUNCTION()
	void ChageFocusItem(int32 index);

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> ItemsParent;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UFHItemQuickSlotElement>> ItemSlots;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentFocusIndex;
};


