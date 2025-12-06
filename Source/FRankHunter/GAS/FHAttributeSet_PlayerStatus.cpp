// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/FHAttributeSet_PlayerStatus.h"
#include "Net/UnrealNetwork.h"
#include "FRankHunter.h"

#include "Gameframework/Character.h"
#include "Component/FHCharacterMovementComponent.h"

#include "Player/FHPlayerBase.h"

void UFHAttributeSet_PlayerStatus::OnRep_Vitality(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_PlayerStatus, Vitality, OldValue);

	/*auto* asc = GetOwningAbilitySystemComponent();
	float NewValue = GetVitality();*/

	// 클라이언트가 조인할 때 서버 PossessedBy -> OnRep_PlayerState가 호출되기 전에
	// PlayerState, Player 액터가 먼저 복제되어서 ASC, 어트리뷰트 세트 초기화가 먼저 일어날 수 있음
	// 그럼 클라는 ActorInfo 초기화 전에 이 함수가 호출되게 됨. 예외처리.
}

void UFHAttributeSet_PlayerStatus::OnRep_Strength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_PlayerStatus, Strength, OldValue);

	/*auto* asc = GetOwningAbilitySystemComponent();
	float NewValue = GetStrength();*/
}

void UFHAttributeSet_PlayerStatus::OnRep_Endurance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_PlayerStatus, Endurance, OldValue);

	/*auto* asc = GetOwningAbilitySystemComponent();
	float NewValue = GetEndurance();*/
}

void UFHAttributeSet_PlayerStatus::OnRep_Agility(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_PlayerStatus, Agility, OldValue);

	/*auto* asc = GetOwningAbilitySystemComponent();
	float NewValue = GetAgility();*/
}

void UFHAttributeSet_PlayerStatus::OnRep_Willpower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_PlayerStatus, Willpower, OldValue);

	/*auto* asc = GetOwningAbilitySystemComponent();
	float NewValue = GetWillpower();*/
}

void UFHAttributeSet_PlayerStatus::OnRep_HeavyWeightPenaltyRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_PlayerStatus, HeavyWeightPenaltyRate, OldValue);
}

void UFHAttributeSet_PlayerStatus::OnRep_DefensePower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_PlayerStatus, DefensePower, OldValue);
}

void UFHAttributeSet_PlayerStatus::OnRep_MiningPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_PlayerStatus, MiningPower, OldValue);
}

void UFHAttributeSet_PlayerStatus::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_PlayerStatus, AttackPower, OldValue);
}

void UFHAttributeSet_PlayerStatus::OnRep_AttackRange(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_PlayerStatus, AttackRange, OldValue);
}

void UFHAttributeSet_PlayerStatus::OnRep_AttackSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_PlayerStatus, AttackSpeed, OldValue);
}

void UFHAttributeSet_PlayerStatus::OnRep_CarryWeight(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_PlayerStatus, CarryWeight, OldValue);
}

void UFHAttributeSet_PlayerStatus::OnRep_MaxCarryWeight(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_PlayerStatus, MaxCarryWeight, OldValue);
}

float UFHAttributeSet_PlayerStatus::GetVitalityBase() const
{
	return Vitality.GetBaseValue();
}

void UFHAttributeSet_PlayerStatus::SetVitalityBase(float NewVal)
{
	Vitality.SetBaseValue(NewVal);
}

float UFHAttributeSet_PlayerStatus::GetStrengthBase() const
{
	return Strength.GetBaseValue();
}

void UFHAttributeSet_PlayerStatus::SetStrengthBase(float NewVal)
{
	Strength.SetBaseValue(NewVal);
}

float UFHAttributeSet_PlayerStatus::GetEnduranceBase() const
{
	return Endurance.GetBaseValue();
}

void UFHAttributeSet_PlayerStatus::SetEnduranceBase(float NewVal)
{
	Endurance.SetBaseValue(NewVal);
}

float UFHAttributeSet_PlayerStatus::GetAgilityBase() const
{
	return Agility.GetBaseValue();
}

void UFHAttributeSet_PlayerStatus::SetAgilityBase(float NewVal)
{
	Agility.SetBaseValue(NewVal);
}

float UFHAttributeSet_PlayerStatus::GetWillpowerBase() const
{
	return Willpower.GetBaseValue();
}

void UFHAttributeSet_PlayerStatus::SetWillpowerBase(float NewVal)
{
	Willpower.SetBaseValue(NewVal);
}

void UFHAttributeSet_PlayerStatus::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFHAttributeSet_PlayerStatus, Vitality);
	DOREPLIFETIME(UFHAttributeSet_PlayerStatus, Strength);
	DOREPLIFETIME(UFHAttributeSet_PlayerStatus, Endurance);
	DOREPLIFETIME(UFHAttributeSet_PlayerStatus, Agility);
	DOREPLIFETIME(UFHAttributeSet_PlayerStatus, Willpower);
	DOREPLIFETIME(UFHAttributeSet_PlayerStatus, HeavyWeightPenaltyRate);
	DOREPLIFETIME(UFHAttributeSet_PlayerStatus, DefensePower);
	DOREPLIFETIME(UFHAttributeSet_PlayerStatus, MiningPower);
	DOREPLIFETIME(UFHAttributeSet_PlayerStatus, AttackPower);
	DOREPLIFETIME(UFHAttributeSet_PlayerStatus, AttackRange);
	DOREPLIFETIME(UFHAttributeSet_PlayerStatus, AttackSpeed);
	DOREPLIFETIME(UFHAttributeSet_PlayerStatus, CarryWeight);
	DOREPLIFETIME(UFHAttributeSet_PlayerStatus, MaxCarryWeight);
}

void UFHAttributeSet_PlayerStatus::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	//if (Attribute == GetVitalityAttribute())
	//{
	//}
	//else if (Attribute == GetStrengthAttribute())
	//{
	//}
	//else if (Attribute == GetEnduranceAttribute())
	//{
	//}
	//else if (Attribute == GetAgilityAttribute())
	//{
	//}
	//else if (Attribute == GetWillpowerAttribute())
	//{
	//}
	//else if (Attribute == GetCarryWeightAttribute())
	//{
	//}
	//else if (Attribute == GetMaxCarryWeightAttribute())
	//{
	//}
}

bool UFHAttributeSet_PlayerStatus::IsExceedCarryWeight(float Weight) const
{
	return GetCarryWeight() + Weight > MaxCarryWeight.GetCurrentValue();
}

float UFHAttributeSet_PlayerStatus::GetCarryWeightRatio() const
{
	return GetCarryWeight() / GetMaxCarryWeight();
}
