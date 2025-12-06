// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemDrag.generated.h"


class UInventoryItemBase;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UItemDrag : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UInventoryItemBase> WidgetReference;
};

UCLASS()
class FRANKHUNTER_API UItemDragImage : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName ItemID;

	UPROPERTY()
	int32 ItemStack;

	class FOnItemDropped* OnItemDroppedDelegatePtr;
};