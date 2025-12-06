// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Inventory/InventoryItemBase.h"
#include "FHItemQuickSlotElement.generated.h"

class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHItemQuickSlotElement : public UInventoryItemBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct();
	
	void SetFocus(bool bIsFocus);

public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> FocusImage;


};
