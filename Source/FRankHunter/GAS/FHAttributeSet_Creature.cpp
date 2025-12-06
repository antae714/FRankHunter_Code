// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/FHAttributeSet_Creature.h"
#include "Net/UnrealNetwork.h"

//void UFHAttributeSet_Creature::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
//{
//	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Creature, MoveSpeed, OldValue);
//}
//
//void UFHAttributeSet_Creature::OnRep_ChaseSpeed(const FGameplayAttributeData& OldValue)
//{
//	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Creature, ChaseSpeed, OldValue);
//}

void UFHAttributeSet_Creature::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Creature, AttackPower, OldValue);
}

void UFHAttributeSet_Creature::OnRep_AttackRange(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Creature, AttackRange, OldValue);
}

void UFHAttributeSet_Creature::OnRep_AttackSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Creature, AttackSpeed, OldValue);
}

void UFHAttributeSet_Creature::OnRep_SightAngle(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Creature, SightAngle, OldValue);
}

void UFHAttributeSet_Creature::OnRep_SightRange(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Creature, SightRange, OldValue);
}

void UFHAttributeSet_Creature::OnRep_HearingRange(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Creature, HearingRange, OldValue);
}

void UFHAttributeSet_Creature::OnRep_DoorOpenTime(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Creature, DoorOpenTime, OldValue);
}

void UFHAttributeSet_Creature::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate the attributes
	//DOREPLIFETIME(UFHAttributeSet_Creature, MoveSpeed);
	//DOREPLIFETIME(UFHAttributeSet_Creature, ChaseSpeed);
	DOREPLIFETIME(UFHAttributeSet_Creature, AttackPower);
	DOREPLIFETIME(UFHAttributeSet_Creature, AttackRange);
	DOREPLIFETIME(UFHAttributeSet_Creature, AttackSpeed);
	DOREPLIFETIME(UFHAttributeSet_Creature, SightAngle);
	DOREPLIFETIME(UFHAttributeSet_Creature, SightRange);
	DOREPLIFETIME(UFHAttributeSet_Creature, HearingRange);
	DOREPLIFETIME(UFHAttributeSet_Creature, DoorOpenTime);
	
}
