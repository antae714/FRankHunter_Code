// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FHSellCountBase.generated.h"

enum class EItemMoveType : uint8;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHSellCountBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<class UImage> ItemIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<class UTextBlock> SellCount;

	UFUNCTION(BlueprintCallable)
	void OnIncreaseButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnDecreaseButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnOkButtonClicked();

	void OnSellCountChanged();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SellCount")
	int32 SellCountValue = 1;

	FName ItemID;
	int32 InventoryIndex;
	int32 MaxItemStack = 0;

	EItemMoveType ItemMoveType;
};
