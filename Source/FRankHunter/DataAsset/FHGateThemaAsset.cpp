// Copyright F Rank Hunter. All Rights Reserved.


#include "DataAsset/FHGateThemaAsset.h"
#include "Algo/RandomShuffle.h"

TSoftObjectPtr<UTexture2D> UFHGateThemaAsset::GetThemaPreviewIcon(FName Key)
{
	FFHGateThemaIcons* IconArray = ThemaPreviewMap.Find(Key);
	if (!IconArray)
	{
		return nullptr;
	}
	return IconArray->GetRandomThemaPreviewIcon();
}

TSoftObjectPtr<UTexture2D> FFHGateThemaIcons::GetRandomThemaPreviewIcon()
{
	if (ThemaPreviewIcons.IsEmpty())
	{
		return nullptr;
	}
	Algo::RandomShuffle(ThemaPreviewIcons);

	return TSoftObjectPtr<UTexture2D>(ThemaPreviewIcons[0].ToSoftObjectPath());
}
