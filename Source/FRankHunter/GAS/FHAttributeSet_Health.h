// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AttributeSet.h"
#include "FHAttributeHelperMacro.h"

#include "FHAttributeSet_Health.generated.h"

// 다이나믹 델리게이트는 UHT가 관리하므로 헤더로 못 뺌
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFHHealthAttributeEvent);


UCLASS()
class FRANKHUNTER_API UFHAttributeSet_Health : public UAttributeSet
{
	GENERATED_BODY()
	

public:
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Health, Health)
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);
	
	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Health, MaxHealth)
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_Damage)
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Health, Damage)
	UFUNCTION()
	void OnRep_Damage(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_Healing)
	FGameplayAttributeData Healing;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Health, Healing)
	UFUNCTION()
	void OnRep_Healing(const FGameplayAttributeData& OldValue);
	
	UPROPERTY(ReplicatedUsing = OnRep_NaturalRegenInitDelay)
	FGameplayAttributeData NaturalRegenInitDelay;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Health, NaturalRegenInitDelay)
	UFUNCTION()
	void OnRep_NaturalRegenInitDelay(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_NaturalRegenAmountPerSec)
	FGameplayAttributeData NaturalRegenAmountPerSec;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Health, NaturalRegenAmountPerSec)
	UFUNCTION()
	void OnRep_NaturalRegenAmountPerSec(const FGameplayAttributeData& OldValue);
	
	UPROPERTY(EditDefaultsOnly, Category = GAS)
	uint8 bIncreaseCurrentHealthWhenMaxHealthIncreased : 1{ true };

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;


	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintAssignable)
	FFHHealthAttributeEvent OnHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FFHHealthAttributeEvent OnHealthZeroDelegate;
	uint32 bOutOfHealth : 1{ false };
};
