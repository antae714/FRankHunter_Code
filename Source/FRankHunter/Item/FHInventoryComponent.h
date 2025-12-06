// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SiInventoryComponent.h"
#include "Common/CommonItemEnum.h"
#include "FHInventoryComponent.generated.h"

class UFHItemBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangedFocus, int32, index);



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) , EditInlineNew)
class FRANKHUNTER_API UFHInventoryComponent : public USiInventoryComponent
{
	GENERATED_BODY()

public:	
	UFHInventoryComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnInventoryChangedFunction(int32 index) override;

public:
	FOnChangedFocus OnChangedFocus;

	void RefreshASCInit();

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentItemIndex() const { return CurrentItemIndex; }

	UFUNCTION(BlueprintCallable)
	void ClearInventory();

	UFUNCTION(Server, Reliable, BlueprintCallable, meta = (DisplayName = "SetCurrentItemIndex"))
	void Server_SetCurrentItemIndex(int32 NewItemIndex);

	// only use for open terminal
	void SetFocusEmpty();

	UFUNCTION()
	void OnRep_CurrentItemIndex();

	UFUNCTION(BlueprintCallable)
	UFHItemBase* GetCurrentItem();

	void InformPlayerChangeItemHoldingType(UFHItemBase* CurrentItem);
	void InformPlayerEquippedItemDisappeard();

	/*
		Use this function when open backpack.
	*/
	void SetTargetInventory(UFHInventoryComponent* TargetInventory);
	const TWeakObjectPtr<UFHInventoryComponent> GetTargetInventory() const { return TargetInventoryWeak; };
private:

	// ÀÌ¸í. Äü½½·ÔÇöÀç¾ÆÀÌÅÛ
	UPROPERTY(ReplicatedUsing = OnRep_CurrentItemIndex)
	int32 CurrentItemIndex;

	UPROPERTY()
	TObjectPtr<UFHItemBase>  CachedCurrentItem;

protected:
	/*
		Use this when open backpack.
		only use when player use quick move item.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TWeakObjectPtr<UFHInventoryComponent> TargetInventoryWeak;
};

