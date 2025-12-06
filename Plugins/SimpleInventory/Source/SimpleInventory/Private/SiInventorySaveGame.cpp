// Copyright F Rank Hunter. All Rights Reserved.


#include "SiInventorySaveGame.h"
#include "SiInventoryComponent.h"

const FGuid FInventoryCustomVersion::GUID(0xA1B2C3D4, 0xE5F6A7B8, 0xC9D0E1F2, 0x12345678);
FCustomVersionRegistration GRegisterInventoryCustomVersion(FInventoryCustomVersion::GUID, FInventoryCustomVersion::LatestVersion, TEXT("InventoryVer"));


USiInventorySaveGame::USiInventorySaveGame()
{
}

void USiInventorySaveGame::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	Ar.UsingCustomVersion(FInventoryCustomVersion::GUID);
	
	if (Ar.IsSaving() || (Ar.IsLoading()&& Ar.CustomVer(FInventoryCustomVersion::GUID) >= FInventoryCustomVersion::FirstVersion))
	{
		for (size_t i = 0; i < ItemArray.Num(); i++)
		{
			UClass* itemClass = ItemArray[i] ? ItemArray[i]->GetClass() : nullptr;
			Ar << itemClass;

			if (Ar.IsLoading() && itemClass)
			{
				ItemArray[i] = NewObject<USiItemInstance>(this, itemClass);
			}
			if (ItemArray[i])
			{
				ItemArray[i]->Serialize(Ar);
			}
		}
	}

}
