// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/FHItemInstallable.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"

void UFHItemInstallable::SetItemData(const FFHItemData& data)
{
	Super::SetItemData(data);

	const UFHFRankHunterSettings* ItemSettings = GetDefault<UFHFRankHunterSettings>();
	TObjectPtr<UDataTable> DeployItemDataTable = Cast<UDataTable>(ItemSettings->DeployItemDataTable.LoadSynchronous());
	if (!DeployItemDataTable)
	{
		return;
	}
	UFHItemClassDataAsset* ItemDA = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
	FFHDeployItemData* FindData = DeployItemDataTable->FindRow<FFHDeployItemData>(data.ItemID, TEXT("UFHItemInstallable::SetItemData"));
	
	if (ItemDA && FindData)
	{
		InstallAbility = ItemDA->GetItemAbility(FindData->DeployAbilityID);
	}
}
