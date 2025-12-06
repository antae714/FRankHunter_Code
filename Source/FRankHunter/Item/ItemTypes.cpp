// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/ItemTypes.h"

float FFHItemDrobTableRow::GetWeight(EGateRank GateGrade) const
{
	switch (GateGrade)
	{
	case EGateRank::E:
		return MaxPerGate_E;
	case EGateRank::D:
		return MaxPerGate_D;
	case EGateRank::C:
		return MaxPerGate_C;
	case EGateRank::B:
		return MaxPerGate_B;
	case EGateRank::A:
		return MaxPerGate_A;
	default:
		break;
	}
	return 0.0f;
}

int32 FFHItemDrobTableRow::GetMaxPerGate(EGateRank GateGrade) const
{
	switch (GateGrade)
	{
	case EGateRank::E:
		return MaxPerGate_E;
	case EGateRank::D:
		return MaxPerGate_D;
	case EGateRank::C:
		return MaxPerGate_C;
	case EGateRank::B:
		return MaxPerGate_B;
	case EGateRank::A:
		return MaxPerGate_A;
	default:
		break;
	}
	return 0;
}

float FFHManaStoneDropTableRow::GetWeight(EGateRank GateGrade) const
{
	switch (GateGrade)
	{
	case EGateRank::E:
		return Weight_E;
	case EGateRank::D:
		return Weight_D;
	case EGateRank::C:
		return Weight_C;
	case EGateRank::B:
		return Weight_B;
	case EGateRank::A:
		return Weight_A;
	case EGateRank::S:
		return Weight_S;
	default:
		break;
	}
	return 0.0f;
}

FLinearColor FItemRarityColorArray::GetColor(EItemRarity ItemRarity) const
{
	switch (ItemRarity)
	{
	case EItemRarity::Common:
		return CommonColor;
	case EItemRarity::UnCommon:
		return UncommonColor;
	case EItemRarity::Rare:
		return RareColor;
	case EItemRarity::Epic:
		return EpicColor;
	case EItemRarity::Legendary:
		return LegendaryColor;
	case EItemRarity::Mythic:
		return MythicColor;
	default:
		break;
	}
	return FLinearColor::White;
}

void FFHItemData::OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems)
{
	OnChangeItem(InRowName);
}

void FFHItemData::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	OnChangeItem(InRowName);
}

void FFHItemData::OnChangeItem(const FName InRowName)
{
	ItemID = InRowName;
}
