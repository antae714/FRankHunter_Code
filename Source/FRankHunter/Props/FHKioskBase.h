// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Containers/ArrayView.h"
#include "GameSavable.h"
#include "FHKioskBase.generated.h"

enum class EItemType : uint8;
enum class EPurchaseError : uint8;
struct FItemArray;

DECLARE_DELEGATE(FOnShopListArrayReplicated);
DECLARE_DELEGATE_TwoParams(FOnShopBuyCountChanged, const TArrayView<int32>&, int32);

USTRUCT(BlueprintType)
struct FItemList : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	EItemType ItemType;

	UPROPERTY()
	FName ItemID;

	UPROPERTY()
	int32 MaxBuyCount;

	FItemList();
	void Serialize(FArchive& Ar);
};

USTRUCT(BlueprintType)
struct FItemArray : public FFastArraySerializer
{
	GENERATED_BODY()

	FOnShopListArrayReplicated OnShopListArrayReplicatedDelegate;
	FOnShopBuyCountChanged OnShopBuyCountChangedDelegate;

	// Server Only
	bool CheckPurchasePossible(const FName& ItemID, int32 BuyCount);
	// Server Only
	void SubMaxBuyCount(const FName& ItemID, int32 ItemCount);
	// Server Only
	void AddItem(const EItemType& ItemType, const FName& ItemID, int32 MaxBuyCount);
	// Server Only
	void ResetItemBuyCount(int32 Index, int32 BuyCount);
	// Client Only
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	// Client Only
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FItemList, FItemArray>(Items, DeltaParams, *this);
	}

	void Serialize(FArchive& Ar);

	UPROPERTY(VisibleAnywhere, Category="ShopList")
	TArray<FItemList> Items;
};

template<>
struct TStructOpsTypeTraits<FItemArray> : public TStructOpsTypeTraitsBase2<FItemList>
{
	enum { WithNetDeltaSerializer = true };
};

UCLASS()
class FRANKHUNTER_API AFHKioskBase : public AActor, public IGameSavable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFHKioskBase();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UFHInteractableComponent> InteractableComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	virtual FString GetSaveSlot() const override;
	virtual bool IsGlobal() const override;
	virtual void SerializeData(FArchive& Ar) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void Interact_Impl(class AFHPlayerBase* Player);

	// Client Only
	void NotifyShopListReplicated();
	// Client Only
	void NotifyShopBuyCountChanged(const TArrayView<int32>& ChangedIndices, int32 FinalSize);

	// Server Only
	void NotifyShopBuyCountChanged(const struct FPurchaseData& ItemData);

	void NotifyPurchaseError(const EPurchaseError& Error);

	UFUNCTION()
	void NotifyPropManagerRegisted();

	UFUNCTION()
	void OnInventoryChanged();

	FItemArray& GetShopList() { return ShopItemList; }

	bool bIsLoad = false;
	bool TryLoadShopList();
	void TrySaveShopList();

	// server only
	UFUNCTION(BlueprintCallable, Category = "ItemList")
	void InitShopItemList();
	bool bIsInitShopList = false;
protected:
	UPROPERTY(Replicated, VisibleAnywhere, Category="ShopItemList")
	FItemArray ShopItemList;
};
