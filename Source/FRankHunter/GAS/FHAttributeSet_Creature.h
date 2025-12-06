// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "FHAttributeHelperMacro.h"
#include "FHAttributeSet_Creature.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHAttributeSet_Creature : public UAttributeSet
{
	GENERATED_BODY()

public:

	// <07-21> UFHAttrbuteSet_Movement·Î ÀÌ°ü
	/*UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Creature, MoveSpeed)
	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_ChaseSpeed)
	FGameplayAttributeData ChaseSpeed;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Creature, ChaseSpeed)
	UFUNCTION()
	void OnRep_ChaseSpeed(const FGameplayAttributeData& OldValue);*/

	UPROPERTY(ReplicatedUsing = OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Creature, AttackPower)
	UFUNCTION()
	void OnRep_AttackPower(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_AttackRange)
	FGameplayAttributeData AttackRange;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Creature, AttackRange)
	UFUNCTION()
	void OnRep_AttackRange(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_AttackSpeed)
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Creature, AttackSpeed)
	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_SightAngle)
	FGameplayAttributeData SightAngle;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Creature, SightAngle)
	UFUNCTION()
	void OnRep_SightAngle(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_SightRange)
	FGameplayAttributeData SightRange;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Creature, SightRange)
	UFUNCTION()
	void OnRep_SightRange(const FGameplayAttributeData& OldValue);

	UPROPERTY(ReplicatedUsing = OnRep_HearingRange)
	FGameplayAttributeData HearingRange;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Creature, HearingRange)
	UFUNCTION()
	void OnRep_HearingRange(const FGameplayAttributeData& OldValue);
	

	UPROPERTY(ReplicatedUsing = OnRep_DoorOpenTime)
	FGameplayAttributeData DoorOpenTime;
	ATTRIBUTE_ACCESSORS(UFHAttributeSet_Creature, DoorOpenTime)
	UFUNCTION()
	void OnRep_DoorOpenTime(const FGameplayAttributeData& OldValue);
	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


};
