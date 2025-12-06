// Copyright I Love Unity, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameSavable.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "SiInventoryComponent.generated.h"

class USiInventoryComponent;


UCLASS(BlueprintType, Blueprintable)
class SIMPLEINVENTORY_API USiItemInstance : public UObject
{
	GENERATED_BODY()

public:
	USiItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual UWorld* GetWorld() const override;
	virtual bool IsSupportedForNetworking() const override { return true; }
	//virtual void Serialize(FArchive& Ar) override;

#if UE_WITH_IRIS
	/** Register all replication fragments */
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif // UE_WITH_IRIS
	virtual void SetOwnerComp(USiInventoryComponent* NewOwnerComp);
	USiInventoryComponent* GetOwnerComp() const { return OwnerComp.Get(); }
	template <typename T>
	T* GetOwnerComp() const { return Cast<T>(OwnerComp.Get()); }
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SiItemInstance|ItemData")
	int32 ItemMaxStack;

protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<USiInventoryComponent> OwnerComp;
};

USTRUCT(BlueprintType, Blueprintable)
struct SIMPLEINVENTORY_API FSiItemDataElement : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "ItemData")
	int32 ItemStack{ 0 };

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "ItemData")
	uint32 bIsLcok : 1 {false};

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "ItemData")
	TObjectPtr<USiItemInstance> ItemInstance;
};

DECLARE_DELEGATE_OneParam(FOnFItemDataArrayChanged, int32);


USTRUCT(BlueprintType)
struct SIMPLEINVENTORY_API FSiItemDataArray : public FFastArraySerializer
{
	GENERATED_BODY()
public:
	FOnFItemDataArrayChanged OnFItemDataArrayChanged;
	TArray<FSiItemDataElement>& GetItemArray() { return ItemArray; }
	const TArray<FSiItemDataElement>& GetItemArray() const { return ItemArray; }
	void Init(int32 NewSize);

	FSiItemDataElement& GetItemAtIndex(int32 index);
	const FSiItemDataElement& GetItemAtIndex(int32 index) const;
	void SetItemAtIndex(int32 index, FSiItemDataElement& item);
	void Swap(int32 firstIndex, int32 secondIndex);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms);

	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);

	void NotifyItemChanged(int32 index);


private:
	UPROPERTY(VisibleAnywhere, Category = "ItemArray")
	TArray<FSiItemDataElement> ItemArray;
};

template<>
struct TStructOpsTypeTraits<FSiItemDataArray> : public TStructOpsTypeTraitsBase2<FSiItemDataArray>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryChanged,
											 USiInventoryComponent*, InventoryComponent,
											 int32, index);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLEINVENTORY_API USiInventoryComponent : public UActorComponent, public IGameSavable
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USiInventoryComponent();

public:
	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "IASInventoryComponent ")
	FOnInventoryChanged OnInventoryChanged;


	// Begin IGameSavable Implementation
	virtual FString GetSaveSlot() const override;
	virtual bool IsGlobal() const override;
	virtual void SerializeData(FArchive& Ar) override;
	// ~End IGameSavable Implementation

	uint32 bIsDeSerializing : 1;

public:

	/**
	 * @brief Add an item
	 * @param ItemClass Classes of items to add
	 * @param Count Number of items to add
	 * @param index Location of the item to add
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "AddItem"))
	void Server_AddItem(TSubclassOf<USiItemInstance> ItemClass, 
						int32 Count,
						int32 index = -1,
						bool bWillRecurseOnFull = true);

	UFUNCTION(Server, Reliable)
	void Server_AddItemByItemInstance(USiItemInstance* itemInstance, int32 Count, int32 index = -1);


	/**
	 * @brief Remove an item
	 * @param ItemClass Classes of items to remove
	 * @param Count Number of items to remove
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "RemoveItem"))
	void Server_RemoveItem(TSubclassOf<USiItemInstance> ItemClass, int32 Count);
	UFUNCTION(Server, Reliable)
	void Server_RemoveItemInstance(USiItemInstance* itemInstance, int32 Count);

	/**
	 * @brief Remove an item
	 * @param index Location of the item to remove
	 * @param Count Number of items to remove
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "RemoveItemAtIndex"))
	void Server_RemoveItemAtIndex(int32 index, int32 Count);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "SwapItem"))
	void Server_SwapItem(int32 firstIndex, int32 secondIndex);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "SwapItemWithInventory"))
	void Server_SwapItemWithInventory(int32 FromIndex, USiInventoryComponent* ToInventoryComponent, int32 ToIndex);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "SwapItemWithInventory"))
	void Server_SwapItemWithInventory2(USiInventoryComponent* FromInventoryComponent, int32 FromIndex, USiInventoryComponent* ToInventoryComponent, int32 ToIndex);

	/**
	 * @brief 
	 * @param FromIndex 
	 * @param Count 
	 * @param ToInventoryComponent 
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "IASInventoryComponent")
	void Server_GiveItemToInventory(int32 FromIndex, int32 Count, USiInventoryComponent* ToInventoryComponent, int32 ToIndex = -1);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "IASInventoryComponent")
	void Server_GiveItemToInventory2(USiInventoryComponent* FromInventoryComponent, int32 FromIndex, int32 Count, USiInventoryComponent* ToInventoryComponent, int32 ToIndex = -1);

	int32 GetEmptyIndex() const;

	UFUNCTION(BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "GetItemIndex"))
	int32 GetItemIndex(USiItemInstance* ItemClass) const;

	UFUNCTION(BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "GetItemIndexIfAddable"))
	int32 GetItemIndexIfAddable(TSubclassOf<USiItemInstance> ItemClass, int32 Count) const;

	UFUNCTION(BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "GetItemInfo"))
	USiItemInstance* K2_GetItemInstance(int32 index);

	UFUNCTION(BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "GetItemInstance"))
	USiItemInstance* GetItemInstance(int32 index) const;

	UFUNCTION(BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "GetItemInstance"))
	UClass* GetItemClass(int32 index) const;

	template<typename T>
	T* GetItemInstance(int32 index)
	{
		return Cast<T>(GetItemInstance(index));
	}

	UFUNCTION(BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "GetItemNum"))
	int32 GetItemNum() const;

	UFUNCTION(BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "GetItemStack"))
	int32 GetItemStack(int32 index) const;

	UFUNCTION(BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "GetItemStack"))
	bool IsItemLock(int32 index) const;
	
	UFUNCTION(BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "GetItemStack"))
	void ItemLock(int32 index, bool IsLock);

	void SetMaxItemCountBeforePlay(int32 NewMaxItemCount);
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "ItemAbilitySystem|Inventory", meta = (DisplayName = "SetMaxItemCount"))
	void Server_SetMaxItemCount(int32 NewMaxItemCount);

	FSiItemDataArray& GetItemArray() { return ItemArray; }


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnInventoryChangedFunction(int32 index);

private:
	UPROPERTY(Replicated, VisibleAnywhere, Category = "IASInventory")
	FSiItemDataArray ItemArray;

	// Replicated?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemComponent", meta = (ClampMin = "0", AllowPrivateAccess = "true"))
	int32 MaxItemCount;


	mutable bool bISIsValidT;
	mutable FName SystemName;
};
