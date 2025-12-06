// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/Abilities/FH_GE_ItemPassive.h"
#include "GAS\FHAttributeSet_PlayerStatus.h"
#include "GAS\FHGameplayTags.h"

UFH_GE_ItemPassive::UFH_GE_ItemPassive()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo ModInfo{};
	ModInfo.Attribute = UFHAttributeSet_PlayerStatus::GetCarryWeightAttribute();
	ModInfo.ModifierOp = EGameplayModOp::Additive;
	FSetByCallerFloat SetByCallerValue{};
	SetByCallerValue.DataName = FName(TEXT("Weight Add"));
	SetByCallerValue.DataTag = GET_GAMEPLAY_TAG_PLAYER_STATUS_CARRYWEIGHT;
	ModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerValue);
	Modifiers.Add(ModInfo);
}
