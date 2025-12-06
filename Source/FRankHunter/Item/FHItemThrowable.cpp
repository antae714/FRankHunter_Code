// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/FHItemThrowable.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"

void UFHItemThrowable::SetItemData(const FFHItemData& data)
{
	Super::SetItemData(data);


	const UFHFRankHunterSettings* ItemSettings = GetDefault<UFHFRankHunterSettings>();
	TObjectPtr<UDataTable> ThrowableItemDataTable = Cast<UDataTable>(ItemSettings->ThrowableItemDataTable.LoadSynchronous());
	if (!ThrowableItemDataTable)
	{
		return;
	}
	UFHItemClassDataAsset* ItemDA = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
	FFHThrowItemData* FindData = ThrowableItemDataTable->FindRow<FFHThrowItemData>(data.ItemID, TEXT("UFHItemInstallable::SetItemData"));

	if (ItemDA && FindData)
	{
		ImpactAbility = ItemDA->GetItemAbility(FindData->ImpactAbilityID);
	}
}
