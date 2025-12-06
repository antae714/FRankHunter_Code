// Copyright I Love Unity, Inc. All Rights Reserved.


#include "SiInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet\KismetSystemLibrary.h"
#include "GameFramework/PlayerState.h"
#include "Kismet\GameplayStatics.h"
#include "SiInventorySaveGame.h"
#include "OnlineSubsystemNames.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItemAbilitySystem, Log, All);

FSiItemDataElement& FSiItemDataArray::GetItemAtIndex(int32 index)
{
	return ItemArray[index];
}

const FSiItemDataElement& FSiItemDataArray::GetItemAtIndex(int32 index) const
{
	return ItemArray[index];
}

void FSiItemDataArray::Init(int32 NewSize)
{
	ItemArray.Init(FSiItemDataElement(), NewSize);

	MarkArrayDirty();
	if (NewSize > 0)
	{
		MarkItemDirty(ItemArray[NewSize - 1]);
		NotifyItemChanged(NewSize - 1);
	}
}

void FSiItemDataArray::SetItemAtIndex(int32 index, FSiItemDataElement& item)
{
	if (ItemArray.IsValidIndex(index))
	{
		ItemArray[index].ItemStack = item.ItemStack;
		ItemArray[index].ItemInstance = item.ItemInstance;


		MarkItemDirty(ItemArray[index]);
		NotifyItemChanged(index);
	}
	else
	{
		UE_LOG(LogItemAbilitySystem, Warning, TEXT("Index Is Not Valid!"));
	}
}

#include "Templates/UnrealTemplate.h"
void FSiItemDataArray::Swap(int32 firstIndex, int32 secondIndex)
{

	::Swap(ItemArray[firstIndex].ItemStack, ItemArray[secondIndex].ItemStack);
	::Swap(ItemArray[firstIndex].ItemInstance, ItemArray[secondIndex].ItemInstance);

	MarkItemDirty(ItemArray[firstIndex]);
	MarkItemDirty(ItemArray[secondIndex]);

	NotifyItemChanged(firstIndex);
	NotifyItemChanged(secondIndex);
}

bool FSiItemDataArray::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FSiItemDataElement, FSiItemDataArray>(ItemArray, DeltaParms, *this);
}

void FSiItemDataArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	PostReplicatedChange(AddedIndices, FinalSize);
}

void FSiItemDataArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (auto& index : ChangedIndices)
	{
		NotifyItemChanged(index);
	}
}

void FSiItemDataArray::NotifyItemChanged(int32 index)
{
	OnFItemDataArrayChanged.ExecuteIfBound(index);
}

// Sets default values for this component's properties
USiInventoryComponent::USiInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
	// ...
}

FString USiInventoryComponent::GetSaveSlot() const
{
	return TEXT("InventoryComponent");
}

bool USiInventoryComponent::IsGlobal() const
{
	return true;
}

void USiInventoryComponent::SerializeData(FArchive& Ar)
{
	// 맘에안드는데 더좋은방법이 생각이 안남
	TArray<int32> ItemStackArray;
	TArray<bool> IsLockArray;
	TArray<TObjectPtr<USiItemInstance>> TempItemArray;

	Ar.UsingCustomVersion(FInventoryCustomVersion::GUID);
	
	if (Ar.IsSaving())
	{
		ItemStackArray.SetNum(GetItemNum());
		IsLockArray.SetNum(GetItemNum());
		TempItemArray.SetNum(GetItemNum());

		for (size_t i = 0; i < GetItemNum(); i++)
		{
			ItemStackArray[i] = GetItemStack(i);
			IsLockArray[i] = IsItemLock(i);
			TempItemArray[i] = GetItemInstance(i);
		}
	}

	Ar << ItemStackArray;
	Ar << IsLockArray;
	Ar << TempItemArray;

	if (Ar.IsSaving() || (Ar.IsLoading() && Ar.CustomVer(FInventoryCustomVersion::GUID) >= FInventoryCustomVersion::FirstVersion))
	{
		for (size_t i = 0; i < TempItemArray.Num(); i++)
		{
			UClass* itemClass = TempItemArray[i] ? TempItemArray[i]->GetClass() : nullptr;
			Ar << itemClass;

			if (Ar.IsLoading() && itemClass)
			{
				TempItemArray[i] = NewObject<USiItemInstance>(this, itemClass);
			}
			if (TempItemArray[i])
			{
				TempItemArray[i]->Serialize(Ar);
			}
		}
	}

	if (Ar.IsLoading())
	{
		ItemArray.Init(TempItemArray.Num());

		for (int32 i = 0; i < TempItemArray.Num(); ++i)
		{
			USiItemInstance* originItem = TempItemArray[i];
			if (!originItem)
			{
				continue;
			}
			Server_AddItemByItemInstance(originItem, ItemStackArray[i], i);
			ItemLock(i, IsLockArray[i]);
		}
		bIsDeSerializing = true;
	}
}

// Called when the game starts
void USiInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ItemArray.OnFItemDataArrayChanged = FOnFItemDataArrayChanged::CreateUObject(this, &ThisClass::OnInventoryChangedFunction);
	if (!bIsDeSerializing && GetOwner() && GetOwner()->HasAuthority())
	{
		ItemArray.Init(MaxItemCount);
	}
	ItemArray.MarkArrayDirty();
	for (size_t i = 0; i < GetItemNum(); i++)
	{
		if (GetItemInstance(i))
		{
			ItemArray.MarkItemDirty(ItemArray.GetItemArray()[i]);
			ItemArray.NotifyItemChanged(i);
		}
	}

}

void USiInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	for (auto& item : ItemArray.GetItemArray())
	{
		if (item.ItemInstance)
		{
			item.ItemInstance->SetOwnerComp(nullptr);
		}
	}
}

void USiInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemArray);
}

void USiInventoryComponent::Server_AddItem_Implementation(TSubclassOf<USiItemInstance> ItemClass, int32 Count, int32 index, bool bWillRecurseOnFull = true)
{
	int32 InitIndex = index;
	if (ItemClass == nullptr)
	{
		UE_LOG(LogItemAbilitySystem, Warning, TEXT("Item class is null!"));
		return;
	}

	if (index == INDEX_NONE)
	{
		index = GetItemIndexIfAddable(ItemClass, Count);
	}

	if (!ItemArray.GetItemArray().IsValidIndex(index))
	{
		UE_LOG(LogItemAbilitySystem, Warning, TEXT("Index Is Not Valid!"));
		return;
	}

	if (!IsItemLock(index))
	{
		FSiItemDataElement& itemData = ItemArray.GetItemAtIndex(index);
		if (itemData.ItemInstance == nullptr)
		{
			itemData.ItemInstance = NewObject<USiItemInstance>(GetOwner(), ItemClass, NAME_None, RF_NoFlags, ItemClass->GetDefaultObject());
			bool bIsStackOverflow = itemData.ItemInstance->ItemMaxStack < Count;
			if (!bIsStackOverflow)
			{
				itemData.ItemStack = Count;
				Count = 0;
			}
			else
			{
				Count -= itemData.ItemInstance->ItemMaxStack;
				itemData.ItemStack = itemData.ItemInstance->ItemMaxStack;
			}
			itemData.ItemInstance->SetOwnerComp(this);
		}
		else
		{
			if (itemData.ItemInstance->IsA(ItemClass))
			{
				bool bIsStackOverflow = itemData.ItemInstance->ItemMaxStack - itemData.ItemStack < Count;
				if (!bIsStackOverflow)
				{
					itemData.ItemStack += Count;
					Count = 0;
				}
				else
				{
					Count -= itemData.ItemInstance->ItemMaxStack - itemData.ItemStack;
					itemData.ItemStack = itemData.ItemInstance->ItemMaxStack;
				}
			}
			else
			{
				UE_LOG(LogItemAbilitySystem, Warning, TEXT("Item class mismatch!"));
				return;
			}
		}
		ItemArray.SetItemAtIndex(index, itemData);
	}


	if(bWillRecurseOnFull && InitIndex == INDEX_NONE && Count > 0 && InitIndex != index)
	{
		Server_AddItem(ItemClass, Count, INDEX_NONE, bWillRecurseOnFull);
	}
}

void USiInventoryComponent::Server_AddItemByItemInstance_Implementation(USiItemInstance* data, int32 Count, int32 index)
{
	if (!data)
	{
		return;
	}
	if (index == INDEX_NONE)
	{
		index = GetItemIndexIfAddable(data->GetClass(), Count);
	}

	if (!ItemArray.GetItemArray().IsValidIndex(index))
	{
		UE_LOG(LogItemAbilitySystem, Warning, TEXT("Index Is Not Valid!"));
		return;
	}

	FSiItemDataElement& itemData = ItemArray.GetItemAtIndex(index);
	if (itemData.ItemInstance == nullptr)
	{
		itemData.ItemInstance = NewObject<USiItemInstance>(GetOwner(), data->GetClass(), NAME_None, RF_NoFlags, data);
		itemData.ItemStack = Count;
		itemData.ItemInstance->SetOwnerComp(this);
	}
	else
	{
		if (itemData.ItemInstance->IsA(data->GetClass()))
		{
			itemData.ItemStack += Count;
		}
		else
		{
			UE_LOG(LogItemAbilitySystem, Warning, TEXT("Item class mismatch!"));
			return;
		}
	}
	ItemArray.SetItemAtIndex(index, itemData);
}

void USiInventoryComponent::Server_RemoveItemInstance_Implementation(USiItemInstance* itemInstance, int32 Count)
{

	if (itemInstance)
	{
		for (int32 i = 0; i < ItemArray.GetItemArray().Num(); ++i)
		{
			FSiItemDataElement& itemData = ItemArray.GetItemAtIndex(i);
			if (itemData.ItemStack > 0 && itemData.ItemInstance == itemInstance)
			{
				Server_RemoveItemAtIndex(i, Count);

				break;
			}
		}
	}
	else
	{
		UE_LOG(LogItemAbilitySystem, Warning, TEXT("Item class is null!"));
	}
}

void USiInventoryComponent::Server_RemoveItem_Implementation(TSubclassOf<USiItemInstance> ItemClass, int32 Count)
{
	if (ItemClass)
	{
		for (int32 i = 0; i < ItemArray.GetItemArray().Num(); ++i)
		{
			FSiItemDataElement& itemData = ItemArray.GetItemAtIndex(i);
			if (itemData.ItemInstance && itemData.ItemInstance.IsA(ItemClass))
			{
				Server_RemoveItemAtIndex(i, Count);
				break;
			}
		}
	}
	else
	{
		UE_LOG(LogItemAbilitySystem, Warning, TEXT("Item class is null!"));
	}
}

void USiInventoryComponent::Server_RemoveItemAtIndex_Implementation(int32 index, int32 Count)
{
	if (Count <= 0)
	{
		return;
	}
	if (!ItemArray.GetItemArray().IsValidIndex(index))
	{
		UE_LOG(LogItemAbilitySystem, Warning, TEXT("Index Is Not Valid!"));
		return;
	}

	FSiItemDataElement& itemData = ItemArray.GetItemAtIndex(index);

	itemData.ItemStack -= Count;
	if (itemData.ItemStack <= 0)
	{
		itemData.ItemInstance->SetOwnerComp(nullptr);
		itemData.ItemInstance = nullptr;
		itemData.ItemStack = 0;
	}

	ItemArray.SetItemAtIndex(index, itemData);
}

void USiInventoryComponent::Server_SwapItem_Implementation(int32 firstIndex, int32 secondIndex)
{
	ItemArray.Swap(firstIndex, secondIndex);
}

void USiInventoryComponent::Server_SwapItemWithInventory_Implementation(int32 FromIndex, USiInventoryComponent* ToInventoryComponent, int32 ToIndex)
{
	USiItemInstance* ThisItem = GetItemInstance(FromIndex);
	USiItemInstance* OtherItem = ToInventoryComponent->GetItemInstance(ToIndex);

	int32 ThisItemStack = GetItemStack(FromIndex);
	int32 OtherItemStack = ToInventoryComponent->GetItemStack(ToIndex);

	Server_RemoveItemAtIndex(FromIndex, ThisItemStack);
	ToInventoryComponent->Server_RemoveItemAtIndex(ToIndex, OtherItemStack);

	Server_AddItemByItemInstance(OtherItem, OtherItemStack, FromIndex);
	ToInventoryComponent->Server_AddItemByItemInstance(ThisItem, ThisItemStack, ToIndex);

}

void USiInventoryComponent::Server_SwapItemWithInventory2_Implementation(USiInventoryComponent* FromInventoryComponent, int32 FromIndex, USiInventoryComponent* ToInventoryComponent, int32 ToIndex)
{
	FromInventoryComponent->Server_SwapItemWithInventory_Implementation(FromIndex, ToInventoryComponent, ToIndex);
}

void USiInventoryComponent::Server_GiveItemToInventory2_Implementation(USiInventoryComponent* FromInventoryComponent, int32 FromIndex, int32 Count, USiInventoryComponent* ToInventoryComponent, int32 ToIndex)
{
	FromInventoryComponent->Server_GiveItemToInventory_Implementation(FromIndex, Count, ToInventoryComponent, ToIndex);
}

int32 USiInventoryComponent::GetEmptyIndex() const
{
	int32 index = ItemArray.GetItemArray().IndexOfByPredicate(
		[](const FSiItemDataElement& itemData)
		{
			return itemData.ItemInstance == nullptr;
		});

	return index;
}

int32 USiInventoryComponent::GetItemIndex(USiItemInstance* ItemClass) const
{
	int32 index = ItemArray.GetItemArray().IndexOfByPredicate(
		[ItemClass](const FSiItemDataElement& itemData)
		{
			bool isSame = itemData.ItemInstance == ItemClass;

			return isSame;
		});
	return index;
}

int32 USiInventoryComponent::GetItemIndexIfAddable(TSubclassOf<USiItemInstance> ItemClass, int32 Count) const
{
	int32 maxItem = ItemClass.GetDefaultObject()->ItemMaxStack;

	int32 index = ItemArray.GetItemArray().IndexOfByPredicate(
		[ItemClass, maxItem, Count](const FSiItemDataElement& itemData)
		{
			if (itemData.ItemInstance == nullptr || itemData.bIsLcok)
			{
				return false;
			}
			bool isSame = itemData.ItemInstance.GetClass() == ItemClass;
			bool bIsFull = itemData.ItemStack == maxItem;

			return isSame && !bIsFull;
		});

	if (index == INDEX_NONE)
	{
		index = GetEmptyIndex();
	}
	return index;
}

USiItemInstance* USiInventoryComponent::K2_GetItemInstance(int32 index)
{
	USiItemInstance* result = GetItemInstance(index);
	return result;
}

USiItemInstance* USiInventoryComponent::GetItemInstance(int32 index) const
{
	if (ItemArray.GetItemArray().IsValidIndex(index))
	{
		return ItemArray.GetItemAtIndex(index).ItemInstance;
	}

	return nullptr;
}

UClass* USiInventoryComponent::GetItemClass(int32 index) const
{
	if (ItemArray.GetItemArray().IsValidIndex(index))
	{
		USiItemInstance* ItemInstance = GetItemInstance(index);
		if (ItemInstance)
		{
			return ItemInstance->GetClass();
		}
	}

	return nullptr;
}

int32 USiInventoryComponent::GetItemNum() const
{
	return ItemArray.GetItemArray().Num();
}

int32 USiInventoryComponent::GetItemStack(int32 index) const
{
	return ItemArray.GetItemAtIndex(index).ItemStack;
}

bool USiInventoryComponent::IsItemLock(int32 index) const
{
	return ItemArray.GetItemAtIndex(index).bIsLcok;
}

void USiInventoryComponent::ItemLock(int32 index, bool IsLock)
{
	if (ItemArray.GetItemArray().IsValidIndex(index))
	{
		FSiItemDataElement& itemData = ItemArray.GetItemAtIndex(index);
		itemData.bIsLcok = IsLock;
		ItemArray.SetItemAtIndex(index, itemData);
	}
	else
	{
		UE_LOG(LogItemAbilitySystem, Warning, TEXT("Index Is Not Valid!"));
	}
}

void USiInventoryComponent::SetMaxItemCountBeforePlay(int32 NewMaxItemCount)
{
	ensureMsgf(!HasBegunPlay(),
			   TEXT("SetMaxItemCount는 게임 시작 전에만 호출되어야 합니다. 현재 HasBegunPlay() == true 입니다."));

	MaxItemCount = NewMaxItemCount;
}

void USiInventoryComponent::Server_SetMaxItemCount_Implementation(int32 NewMaxItemCount)
{
	MaxItemCount = NewMaxItemCount;
	ItemArray.Init(MaxItemCount);
}

void USiInventoryComponent::Server_GiveItemToInventory_Implementation(int32 FromIndex, int32 Count, USiInventoryComponent* ToInventoryComponent, int32 ToIndex)
{
	if (FromIndex == INDEX_NONE || !ToInventoryComponent)
	{
		return;
	}
	USiItemInstance* ItemInstance = GetItemInstance(FromIndex);

	if (!ItemInstance)
	{
		return;
	}
	if (ToIndex == INDEX_NONE)
	{
		ToIndex = ToInventoryComponent->GetItemIndexIfAddable(ItemInstance->GetClass(), Count);
	}
	if (ToIndex == INDEX_NONE)
	{
		return;
	}

	Server_RemoveItemInstance(ItemInstance, Count);
	if (GetItemInstance(FromIndex))
	{
		ItemInstance = NewObject<USiItemInstance>(GetOwner()->GetLevel(), ItemInstance->GetClass(), NAME_None, RF_NoFlags, ItemInstance);
	}

	ToInventoryComponent->Server_AddItemByItemInstance(ItemInstance, Count, ToIndex);
}

void USiInventoryComponent::OnInventoryChangedFunction(int32 index)
{
	OnInventoryChanged.Broadcast(this, index);
}

USiItemInstance::USiItemInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

UWorld* USiItemInstance::GetWorld() const
{
	Super::GetWorld();
	if (!!HasAllFlags(RF_ClassDefaultObject))
	{
		// If we are a CDO, we must return nullptr instead of calling Outer->GetWorld() to fool UObject::ImplementsGetWorld.
		return nullptr;
	}
	return GetOuter()->GetWorld();
}


#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
void USiItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	// Build descriptors and allocate PropertyReplicationFragments for this object
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif // UE_WITH_IRIS

void USiItemInstance::SetOwnerComp(USiInventoryComponent* NewOwnerComp)
{
	if (OwnerComp == NewOwnerComp)
	{
		return;
	}
	USiInventoryComponent* OldOwnerComp = OwnerComp.Get();
	if (OldOwnerComp)
	{
		OldOwnerComp->RemoveReplicatedSubObject(this);
	}
	if (NewOwnerComp)
	{
		NewOwnerComp->AddReplicatedSubObject(this);
	}

	OwnerComp = NewOwnerComp;
}
