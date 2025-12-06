// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/Data/FHItemSubsystem.h"
#include "Item\ItemTypes.h"
#include "Item/FHItemBase.h"
#include "Engine/AssetManager.h"
#include "Core/FHFRankHunterSettings.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"

void UFHItemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
    DataSetting();
}

TSubclassOf<UFHItemBase> UFHItemSubsystem::GetItemClass(FName ItemID)
{
	if (ItemClassDataAsset)
	{
		return ItemClassDataAsset->GetItemClass(ItemID);
	}
    else
    {
        check(0);
        return nullptr;
    }
}

void UFHItemSubsystem::DataSetting()
{
    const UFHFRankHunterSettings* ItemSettings = GetDefault<UFHFRankHunterSettings>();
    if (ItemSettings)
    {
        TObjectPtr<UDataTable> CachedItemDataTable = Cast<UDataTable>(ItemSettings->ItemDataTable.LoadSynchronous());
        TObjectPtr<UDataTable> CachedItemDataTable2 = Cast<UDataTable>(ItemSettings->ThrowableItemDataTable.LoadSynchronous());
        TObjectPtr<UDataTable> CachedItemDataTable3 = Cast<UDataTable>(ItemSettings->DeployItemDataTable.LoadSynchronous());
        ItemClassDataAsset = Cast<UFHItemClassDataAsset>(ItemSettings->ItemClassDataAsset.LoadSynchronous());

        check(CachedItemDataTable);
        check(ItemClassDataAsset);
        for (const auto& Row : CachedItemDataTable->GetRowMap())
        {
            FName RowName = Row.Key;
            FFHItemData* ItemData = reinterpret_cast<FFHItemData*>(Row.Value);

            TSubclassOf<UFHItemBase> ItemClass = ItemClassDataAsset->GetItemClass(ItemData->ItemID);
            if (ItemClass)
            {
                UFHItemBase* CDO = ItemClassDataAsset->GetItemClass(ItemData->ItemID).GetDefaultObject();
                if (CDO && ItemData)
                {
                    CDO->SetItemData(*ItemData);
                    CDO->ItemMaxStack = ItemData->MaxStack;
                    CDO->CurrentCondition = ItemData->DefaultCondition;
                    CDO->ItemID = ItemData->ItemID;
                }
            }
        }

        CachedItemDataTable = nullptr;
    }
}
