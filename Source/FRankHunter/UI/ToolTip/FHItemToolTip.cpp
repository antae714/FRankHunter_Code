// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/ToolTip/FHItemToolTip.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Item/FHItemBase.h"
#include "Item\ItemTypes.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "Core/FHFRankHunterSettings.h"


#define LOCTEXT_NAMESPACE "FHItemToolTip"


void UFHItemToolTip::K2_SettingItemData(TSubclassOf<UFHItemBase> itemClass, int32 Condition, EItemTooltipContext context)
{
	if (itemClass == nullptr)
	{
		return;
	}
	UFHItemBase* itemInstance = NewObject<UFHItemBase>(this, itemClass);;
	if (itemInstance == nullptr)
	{
		return;
	}
	itemInstance->CurrentCondition = Condition;
	SettingItemData(itemInstance, context);
}

void UFHItemToolTip::SettingItemData(UFHItemBase* itemInstance, EItemTooltipContext context)
{
	if (itemInstance == nullptr)
	{
		return;
	}

	FFHItemData& itemData = itemInstance->GetItemData();
	const UEnum* EnumPtr = StaticEnum<EItemRarity>();
	check(EnumPtr);

	const UFHFRankHunterSettings* ItemSettings = GetDefault<UFHFRankHunterSettings>();
	FName LocalizeName = ItemSettings->GetLocalizeStringTableID();

	UFHItemClassDataAsset* ItemDA = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
	check(ItemDA);

	FText ItemNameText = ItemDA->GetItemName(itemData.ItemNameID);
	FText DescriptionText = ItemDA->GetItemDescription(itemData.DescriptionID);
	FText ReqLevelText = FText::Format(FText::FromStringTable(LocalizeName, TEXT("TOOLTIP_EQUIP_LEVEL")), itemData.ReqLevel);
	FText RareityText = EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(itemData.Rarity));
	FText DisplayWeightText = FText::Format(FText::FromStringTable(LocalizeName, TEXT("TOOLTIP_WEIGHT")), itemData.Weight);
	FText ConditionText = FText::Format(FText::AsCultureInvariant(TEXT("{0}/{1}")), itemInstance->CurrentCondition, itemData.DefaultCondition);
	FText PriceText =
		context == EItemTooltipContext::Shop ?
		FText::Format(FText::FromStringTable(LocalizeName, TEXT("TOOLTIP_BUYPRICE")), itemData.BuyPrice)
		: FText::Format(FText::FromStringTable(LocalizeName, TEXT("TOOLTIP_SELLPRICE")), itemData.SellPrice);

	ItemName->SetText(ItemNameText);
	Description->SetText(DescriptionText);
	Icon->SetBrushFromSoftTexture(ItemDA->GetItemIcon(itemData.IconID));
	ReqLevel->SetText(ReqLevelText);
	Rareity->SetText(RareityText);
	DisplayWeight->SetText(DisplayWeightText);
	ConditionProgress->SetPercent((float)itemInstance->CurrentCondition / (itemData.DefaultCondition ? itemData.DefaultCondition : 100));
	ConditionPercent->SetText(ConditionText);
	PriceNotify->SetText(PriceText);

}

void UFHItemToolTip::SettingItemDataFromData(FFHItemData ItemData, EItemTooltipContext context)
{
	const UEnum* EnumPtr = StaticEnum<EItemRarity>();
	check(EnumPtr);

	const UFHFRankHunterSettings* ItemSettings = GetDefault<UFHFRankHunterSettings>();
	FName LocalizeName = ItemSettings->GetLocalizeStringTableID();

	UFHItemClassDataAsset* ItemDA = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
	check(ItemDA);

	FText ItemNameText = ItemDA->GetItemName(ItemData.ItemNameID);
	FText DescriptionText = ItemDA->GetItemDescription(ItemData.DescriptionID);
	FText ReqLevelText = FText::Format(FText::FromStringTable(LocalizeName, TEXT("TOOLTIP_EQUIP_LEVEL")), ItemData.ReqLevel);
	FText RareityText = EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(ItemData.Rarity));
	FText DisplayWeightText = FText::Format(FText::FromStringTable(LocalizeName, TEXT("TOOLTIP_WEIGHT")), ItemData.Weight);
	FText ConditionText = FText::Format(FText::AsCultureInvariant(TEXT("{0}/{1}")), ItemData.DefaultCondition, ItemData.DefaultCondition);
	FText PriceText =
		context == EItemTooltipContext::Shop ?
		FText::Format(FText::FromStringTable(LocalizeName, TEXT("TOOLTIP_BUYPRICE")), ItemData.BuyPrice)
		: FText::Format(FText::FromStringTable(LocalizeName, TEXT("TOOLTIP_SELLPRICE")), ItemData.SellPrice);

	ItemName->SetText(ItemNameText);
	Description->SetText(DescriptionText);
	Icon->SetBrushFromSoftTexture(ItemDA->GetItemIcon(ItemData.IconID));
	ReqLevel->SetText(ReqLevelText);
	Rareity->SetText(RareityText);
	DisplayWeight->SetText(DisplayWeightText);
	ConditionProgress->SetPercent((float)ItemData.DefaultCondition / (ItemData.DefaultCondition ? ItemData.DefaultCondition : 100));
	ConditionPercent->SetText(ConditionText);
	PriceNotify->SetText(PriceText);




	FLinearColor ItemRarityColor = UFHBlueprintFunctionLibrary::GetItemRarityColor(ItemData.Rarity);
	ItemName->SetColorAndOpacity(ItemRarityColor);
	Rareity->SetColorAndOpacity(ItemRarityColor);
}


#undef LOCTEXT_NAMESPACE