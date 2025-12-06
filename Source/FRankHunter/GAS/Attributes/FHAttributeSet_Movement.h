// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AttributeSet.h"
#include "GAS/FHAttributeHelperMacro.h"
#include "GAS/Attributes/FHAttributeSet.h"

#include "FHAttributeSet_Movement.generated.h"


UCLASS()
class FRANKHUNTER_API UFHAttributeSet_Movement : public UFHAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(ReplicatedUsing = OnRep_WalkSpeed)
	FGameplayAttributeData WalkSpeed;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Movement, WalkSpeed)
	UFUNCTION()
	void OnRep_WalkSpeed(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_RunSpeed)
	FGameplayAttributeData RunSpeed;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Movement, RunSpeed)
	UFUNCTION()
	void OnRep_RunSpeed(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_JumpPower)
	FGameplayAttributeData JumpPower;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Movement, JumpPower)
	UFUNCTION()
	void OnRep_JumpPower(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_ActionSpeedMult)
	FGameplayAttributeData ActionSpeedMult;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Movement, ActionSpeedMult)
	UFUNCTION()
	void OnRep_ActionSpeedMult(const FGameplayAttributeData& OldValue);


public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void OnInitAbilityActorInfo() override;
};
