// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SiInventoryComponent.h"
#include "SiInventorySaveGame.generated.h"



// Custom serialization version for all packages Inventory
struct FInventoryCustomVersion
{
	enum Type
	{
		// Before any version changes were made in the plugin
		BeforeCustomVersionWasAdded = 0,
		FirstVersion = 1,

		// -----<new versions can be added above this line>-------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};

	// The GUID for this custom version number
	const static FGuid GUID;

private:
	FInventoryCustomVersion() {}
};



/**
 * DEPRECATED
 */
UCLASS()
class SIMPLEINVENTORY_API USiInventorySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	USiInventorySaveGame();

	virtual void Serialize(FArchive& Ar) override;


public:
	UPROPERTY(VisibleInstanceOnly, Category = "ItemData")
	TArray<int32> ItemStackArray;

	UPROPERTY(VisibleInstanceOnly, Category = "ItemData")
	TArray<bool> IsLockArray;

	UPROPERTY(VisibleInstanceOnly, Category = "ItemData")
	TArray<TObjectPtr<USiItemInstance>> ItemArray;
};
