// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LobbyUIUtils.generated.h"

UENUM(BlueprintType)
enum class EPurchaseError : uint8
{
	None,
	NoMoney,
	AlreadyOpen,
	LowOnStock,
	UnKnownError,

};

UENUM(BlueprintType)
enum class EUIType : uint8
{
	Buy,
	Sale
};

UENUM(BlueprintType)
enum class ESellItemError : uint8
{
	None,
	NoItem,
	NotEnoughCount,
	NoItemInList,
};

UENUM(BlueprintType)
enum class EItemMoveType : uint8 
{
	InventoryToShop,
	ShopToInventory,

};

USTRUCT(BlueprintType)
struct FSellItemInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FName ItemID;

	UPROPERTY()
	int32 ItemStack{ 0 };

	UPROPERTY()
	int32 InventoryIndex{ 0 };
};

DECLARE_DELEGATE(FOnHomeButtonClicked);


UINTERFACE(MinimalAPI, Blueprintable)
class UKioskDelegateClass : public UInterface
{
	GENERATED_BODY()


};

class IKioskDelegateClass
{
	GENERATED_BODY()

public:
	FOnHomeButtonClicked OnHomeButtonClickedDelegate;
};