// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHKioskBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"

#include "Item/ItemTypes.h"
#include "Item/FHInventoryComponent.h"
#include "Item/Data/FHItemClassDataAsset.h"

#include "Core/FHUIManager.h"
#include "Core/FHPropManager.h"
#include "Core/ManagerActorRegistrySubsystem.h"
#include "Core/FHPlayerController.h"
#include "Player/FHPlayerBase.h"

#include "UI/Lobby/FHShopBase.h"
#include "UI/Lobby/FHKioskMain.h"
#include "UI/Lobby/FHBuyUIBase.h"
#include "UI/FHLobbyNPCWidget.h"

#include "Props/FHTray.h"

#include "Components/WidgetComponent.h"
#include "Component/FHInteractableComponent.h"

#include "SimpleSaveKitFunctionLibrary.h"
#include "FRankHunter.h"
#include "Lobby/FH_GS_LobbyGameState.h"

// Sets default values
AFHKioskBase::AFHKioskBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	InteractableComponent = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComponent"));
}

// Called when the game starts or when spawned
void AFHKioskBase::BeginPlay()
{
	Super::BeginPlay();

	UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	Subsystem->GetManagerRegistDelegate(TEXT("PropManager")).AddDynamic(this, &AFHKioskBase::NotifyPropManagerRegisted);

	ShopItemList.OnShopListArrayReplicatedDelegate = FOnShopListArrayReplicated::CreateUObject(this, &AFHKioskBase::NotifyShopListReplicated);
	ShopItemList.OnShopBuyCountChangedDelegate = FOnShopBuyCountChanged::CreateUObject(this, &AFHKioskBase::NotifyShopBuyCountChanged);

	bIsInitShopList = false;
}

void AFHKioskBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFHKioskBase, ShopItemList);
}

FString AFHKioskBase::GetSaveSlot() const
{
	return TEXT("Shop");
}

bool AFHKioskBase::IsGlobal() const
{
	return true;
}

void AFHKioskBase::SerializeData(FArchive& Ar)
{
	Ar.UsingCustomVersion(FFRankHunterCustomVersion::GUID);
	if (Ar.CustomVer(FFRankHunterCustomVersion::GUID) >= FFRankHunterCustomVersion::FirstVersion)
	{
		ShopItemList.Serialize(Ar);
	}
}

// Called every frame
void AFHKioskBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Not Use
void AFHKioskBase::Interact_Impl(class AFHPlayerBase* Player)
{
	AFHPlayerController* PC = Cast<AFHPlayerController>(Player->GetController());
	if (PC)
	{
		PC->OpenKiosk();
	}

	//UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	//check(Subsystem);

	//AFHPropManager* PropManager = Subsystem->GetManagerActor<AFHPropManager>(TEXT("PropManager"));
	//check(PropManager);

	//if (AFHTray* SellTray = PropManager->GetProp<AFHTray>(TEXT("SellTray")))
	//{
	//	SellTray->ProcessSellItem();
	//}
}

void AFHKioskBase::NotifyShopListReplicated()
{
	UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	check(Subsystem);

	AFHUIManager* UIManager = Subsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	check(UIManager);

	if (UFHLobbyNPCWidget* Widget = UIManager->GetWidget<UFHLobbyNPCWidget>(TEXT("NpcTalk")))
	{
		Widget->BuyUI->InitShopList();
	}
}

void AFHKioskBase::NotifyShopBuyCountChanged(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	check(Subsystem);

	AFHUIManager* UIManager = Subsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	check(UIManager);

	if (UFHLobbyNPCWidget* Widget = UIManager->GetWidget<UFHLobbyNPCWidget>(TEXT("NpcTalk")))
	{
		Widget->BuyUI->OnShopBuyCountChanged(ChangedIndices, FinalSize);
	}
}

void AFHKioskBase::NotifyShopBuyCountChanged(const struct FPurchaseData& ItemData)
{
	int32 ChangedItemIndex = ShopItemList.Items.IndexOfByPredicate([ItemData](const FItemList& Element) {
		return Element.ItemID == ItemData.ItemID;
		}
	);

	if (ChangedItemIndex == INDEX_NONE)
	{
		return;
	}

	TArray<int32> ChangedIndices = { ChangedItemIndex };

	UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	check(Subsystem);

	AFHUIManager* UIManager = Subsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	check(UIManager);

	if (UFHLobbyNPCWidget* Widget = UIManager->GetWidget<UFHLobbyNPCWidget>(TEXT("NpcTalk")))
	{
		Widget->BuyUI->OnShopBuyCountChanged(ChangedIndices, ItemData.ItemCount);
	}
}

void AFHKioskBase::NotifyPurchaseError(const EPurchaseError& Error)
{
	UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	check(Subsystem);

	AFHUIManager* UIManager = Subsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	check(UIManager);

	//if (UFHKioskMain* Widget = UIManager->GetWidget<UFHKioskMain>(TEXT("Kiosk")))
	//{
	//	Widget->HandlePurchaseError(Error);
	//}

	if (UFHLobbyNPCWidget* Widget = UIManager->GetWidget<UFHLobbyNPCWidget>(TEXT("NpcTalk")))
	{
		Widget->HandlePurchaseError(Error);
	}

}

void AFHKioskBase::NotifyPropManagerRegisted()
{
	UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	AFHPropManager* PropManager = Subsystem->GetManagerActor<AFHPropManager>(TEXT("PropManager"));
	PropManager->RegistProp(TEXT("Kiosk"), this);
}

void AFHKioskBase::OnInventoryChanged()
{

}

bool AFHKioskBase::TryLoadShopList()
{
	bool Result = false;
	
	// Call GameMode Only
	if (!bIsLoad)
	{
		const FString& GameID = UFHBlueprintFunctionLibrary::GetGameName(this);
		Result = USimpleSaveKitFunctionLibrary::IsExistSaveFile(GameID, this);
		if (Result)
		{
			UFHBlueprintFunctionLibrary::LoadData(this);

			NotifyShopListReplicated();
		}
	}

	return Result;
}

void AFHKioskBase::TrySaveShopList()
{
	UFHBlueprintFunctionLibrary::SaveData(this);
}

FItemList::FItemList()
{
	ItemType = EItemType::None;
	MaxBuyCount = 0;
}

bool FItemArray::CheckPurchasePossible(const FName& ItemID, int32 BuyCount)
{
	FItemList* SearchedList = Items.FindByPredicate(
		[ItemID](const FItemList& List) {
			return List.ItemID == ItemID;
		}
	);

	if (SearchedList)
	{
		return SearchedList->MaxBuyCount >= BuyCount;
	}
	return false;
}

void FItemArray::SubMaxBuyCount(const FName& ItemID, int32 ItemCount)
{
	FItemList* SearchedList = Items.FindByPredicate(
		[ItemID](const FItemList& List) {
			return List.ItemID == ItemID;
		}
	);

	if (SearchedList)
	{
		SearchedList->MaxBuyCount -= ItemCount;
		MarkItemDirty(*SearchedList);
	}
}

void FItemArray::AddItem(const EItemType& ItemType, const FName& ItemID, int32 MaxBuyCount)
{
	FItemList ItemList;
	ItemList.ItemType = ItemType;
	ItemList.ItemID = ItemID;
	ItemList.MaxBuyCount = MaxBuyCount;

	Items.Add(ItemList);
}

void FItemArray::ResetItemBuyCount(int32 Index, int32 BuyCount)
{
	if(Index >= 0 && Index < Items.Num())
	{
		Items[Index].MaxBuyCount = BuyCount;
		MarkItemDirty(Items[Index]);
	}
}


void FItemArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	UE_LOG(LogTemp, Warning, TEXT("FItemList::PostReplicatedAdd() Begin"));

	OnShopListArrayReplicatedDelegate.ExecuteIfBound();

	UE_LOG(LogTemp, Warning, TEXT("FItemList::PostReplicatedAdd() End"));
}

void FItemArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	UE_LOG(LogTemp, Warning, TEXT("FItemArray::PostReplicatedChange() Begin"));

	OnShopBuyCountChangedDelegate.ExecuteIfBound(ChangedIndices, FinalSize);

	UE_LOG(LogTemp, Warning, TEXT("FItemArray::PostReplicatedChange() End"));
}

void FItemList::Serialize(FArchive& Ar)
{
	Ar << ItemType;
	Ar << ItemID;
	Ar << MaxBuyCount;
}

void FItemArray::Serialize(FArchive& Ar)
{
	for (FItemList& Item : Items)
	{
		Item.Serialize(Ar);
	}
}

void AFHKioskBase::InitShopItemList()
{
	if (bIsInitShopList)
	{
		return;
	}

	AFH_GS_LobbyGameState* GameState = Cast<AFH_GS_LobbyGameState>(UGameplayStatics::GetGameState(this));
	if(!GameState)
	{
		return;
	}

	TArray<FFHItemData*> ItemList;
	GameState->ItemTable->GetAllRows(TEXT("AllItemList"), ItemList);

	UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	check(Subsystem);

	AFHUIManager* UIManager = Subsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	check(UIManager);

	UFHLobbyNPCWidget* Widget = UIManager->GetWidget<UFHLobbyNPCWidget>(TEXT("NpcTalk"));
	if (!Widget)
	{
		return;
	}

	for (FFHItemData* ItemData : ItemList)
	{
		EItemType ItemType = ItemData->Type;
		FName ItemID = ItemData->ItemID;
		int32 MaxBuyCount = ItemData->MaxBuyCount;

		GetShopList().AddItem(ItemType, ItemID, MaxBuyCount);
	}

	GetShopList().MarkArrayDirty();
	Widget->BuyUI->InitShopList();

	bIsInitShopList = true;
}