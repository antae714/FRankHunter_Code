// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AttributeSet.h"
//#include "GAS/FHGameplayTags.h"
#include "FHAttributeHelperMacro.h"

#include "FHAttributeSet_Stamina.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHAttributeSet_Stamina : public UAttributeSet
{
	GENERATED_BODY()


protected:
	//uint32 bIsExhausted : 1;
	//FGameplayTag IsExhaustedTag;
	
public:
	UPROPERTY(ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Stamina, Stamina)
	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue);
	
	UPROPERTY(ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Stamina, MaxStamina)
	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_NaturalRegenInitDelay)
	FGameplayAttributeData NaturalRegenInitDelay;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Stamina, NaturalRegenInitDelay)
	UFUNCTION()
	void OnRep_NaturalRegenInitDelay(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_NaturalRegenAmountPerSec)
	FGameplayAttributeData NaturalRegenAmountPerSec;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Stamina, NaturalRegenAmountPerSec)
	UFUNCTION()
	void OnRep_NaturalRegenAmountPerSec(const FGameplayAttributeData& OldValue);


	UPROPERTY(ReplicatedUsing = OnRep_StaminaCost)
	FGameplayAttributeData StaminaCost;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Stamina, StaminaCost)
	UFUNCTION()
	void OnRep_StaminaCost(const FGameplayAttributeData& OldValue);


public:
	UFUNCTION(BlueprintCallable)
	void SetIncreaseCurrentStaminaWhenMaxStaminaIncreased(bool NewValue);

	UFUNCTION(BlueprintCallable)
	void SetUseExhaustedSystem(bool NewValue);

	UFUNCTION(BlueprintCallable)
	void SetRecoveredFromExhaustedStateRatio(float NewValue);


	uint32 bIncreaseCurrentStaminaWhenMaxStaminaIncreased : 1{true};
	uint32 bUseExhaustedSystem : 1{true};
	float RecoveredFromExhaustedStateRatio{ 0.1f };


public:
	UFHAttributeSet_Stamina();


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
};
