// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AttributeSet.h"
#include "FHAttributeHelperMacro.h"

#include "FHAttributeSet_PlayerStatus.generated.h"

UCLASS()
class FRANKHUNTER_API UFHAttributeSet_PlayerStatus : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(ReplicatedUsing = OnRep_Vitality)
	FGameplayAttributeData Vitality;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_PlayerStatus, Vitality)
	UFUNCTION()
	void OnRep_Vitality(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_Strength)
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_PlayerStatus, Strength)
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_Endurance)
	FGameplayAttributeData Endurance;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_PlayerStatus, Endurance)
	UFUNCTION()
	void OnRep_Endurance(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_Agility)
	FGameplayAttributeData Agility;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_PlayerStatus, Agility)
	UFUNCTION()
	void OnRep_Agility(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_Willpower)
	FGameplayAttributeData Willpower;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_PlayerStatus, Willpower)
	UFUNCTION()
	void OnRep_Willpower(const FGameplayAttributeData& OldValue);

	// 이동속도 패널티 / WalkSpeed, RunSpeed 모두 영향을 줘야 해서 따로 뺌
	UPROPERTY(ReplicatedUsing = OnRep_HeavyWeightPenaltyRate)
	FGameplayAttributeData HeavyWeightPenaltyRate;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_PlayerStatus, HeavyWeightPenaltyRate)
	UFUNCTION()
	void OnRep_HeavyWeightPenaltyRate(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_DefensePower)
	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_PlayerStatus, DefensePower)
	UFUNCTION()
	void OnRep_DefensePower(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_MiningPower)
	FGameplayAttributeData MiningPower;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_PlayerStatus, MiningPower)
	UFUNCTION()
	void OnRep_MiningPower(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_PlayerStatus, AttackPower)
	UFUNCTION()
	void OnRep_AttackPower(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_AttackRange)
	FGameplayAttributeData AttackRange;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_PlayerStatus, AttackRange)
	UFUNCTION()
	void OnRep_AttackRange(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_AttackSpeed)
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_PlayerStatus, AttackSpeed)
	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue);

	// 어트리뷰트로 하지 말까?
	UPROPERTY(ReplicatedUsing = OnRep_CarryWeight)
	FGameplayAttributeData CarryWeight;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_PlayerStatus, CarryWeight)
	UFUNCTION()
	void OnRep_CarryWeight(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_MaxCarryWeight)
	FGameplayAttributeData MaxCarryWeight;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_PlayerStatus, MaxCarryWeight)
	UFUNCTION()
	void OnRep_MaxCarryWeight(const FGameplayAttributeData& OldValue);

	float GetVitalityBase() const;
	void SetVitalityBase(float NewVal);

	float GetStrengthBase() const;
	void SetStrengthBase(float NewVal);

	float GetEnduranceBase() const;
	void SetEnduranceBase(float NewVal);

	float GetAgilityBase() const;
	void SetAgilityBase(float NewVal);

	float GetWillpowerBase() const;
	void SetWillpowerBase(float NewVal);

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;



	bool IsExceedCarryWeight(float Weight) const;
	float GetCarryWeightRatio() const;
};
