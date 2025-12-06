// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SiInventoryComponent.h"
#include "GameFramework/SaveGame.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "Item\ItemTypes.h"
#include "FHItemBase.generated.h"

class UGameplayAbility;
class UFHGameplayAbility;
class UAbilitySystemComponent;
class UGameplayEffect;


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class FRANKHUNTER_API UFHItemBase : public USiItemInstance
{
	GENERATED_BODY()

public:
	UFHItemBase();

public:
	virtual void PostInitProperties() override;
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void SetOwnerComp(USiInventoryComponent* NewOwnerComp) override;
	void ChangeStackCount(int NewStack);

	UFUNCTION(Server, Reliable)
	void Server_SetEquipped(bool bIsEquipped);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FFHItemData& GetItemData() { return ItemData; }
	const FFHItemData& GetItemData() const { return ItemData; }
	virtual void SetItemData(const FFHItemData& data);

	bool CanUse();
	void RefreshCooldown();
	void ConsumeCondition();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetConditionPercent();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCoolTime();
	
    UPROPERTY(SaveGame, Category = "Item|Data", VisibleAnywhere, BlueprintReadWrite, Replicated)
	int32 CurrentCondition;

	UPROPERTY(Category = "Item|Data", VisibleAnywhere, BlueprintReadOnly)
	FName ItemID;

protected:
	UPROPERTY(Category = "Item|Ability", VisibleAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> PassiveEffect;

    UPROPERTY(Category = "Item|Ability", VisibleAnywhere, BlueprintReadWrite)
    TSubclassOf<UFHGameplayAbility> CurrentHoldAbility;
    
    UPROPERTY(Category = "Item|Ability", VisibleAnywhere, BlueprintReadWrite)
	TSubclassOf<UFHGameplayAbility> PrePareUseAbility;

    UPROPERTY(Category = "Item|Ability", VisibleAnywhere, BlueprintReadWrite)
	TSubclassOf<UFHGameplayAbility> UseAbility;

protected:

	UPROPERTY()
    TWeakObjectPtr<UAbilitySystemComponent> ASC;

	FActiveGameplayEffectHandle PassiveEffectHandle;
	FGameplayAbilitySpecHandle CurrentHoldAbilitySpecHandle;
	FGameplayAbilitySpecHandle PrePareUseAbilitySpecHandle;
    FGameplayAbilitySpecHandle UseAbilitySpecHandle;


private:
	UPROPERTY(Category = "Item|Data", VisibleAnywhere)
	FFHItemData ItemData;

	UPROPERTY(Replicated)
	double lastUseTime;
};

