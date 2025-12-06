// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "FHItemToolTip.generated.h"

class UTextBlock;
class UImage;
class UProgressBar;
struct FFHItemData;

UENUM(BlueprintType)
enum class EItemTooltipContext : uint8
{
	Inventory,
	Shop,
	World
};


/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHItemToolTip : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable,
			  meta = (DevelopmentOnly,
			  DisplayName = "Set Item Data (DesignTime Only)",
			  ToolTip = "디자인 타임에서만 사용하세요"))
	void K2_SettingItemData(TSubclassOf<UFHItemBase> itemClass, int32 Condition , EItemTooltipContext context);

	UFUNCTION(BlueprintCallable)
	void SettingItemData(class UFHItemBase* itemInstance, EItemTooltipContext context);

	UFUNCTION(BlueprintCallable)
	void SettingItemDataFromData(FFHItemData ItemData, EItemTooltipContext context);
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> Icon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ReqLevel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Rareity;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> DisplayWeight;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ConditionPercent;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> ConditionProgress;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Description;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> PriceNotify;
};
