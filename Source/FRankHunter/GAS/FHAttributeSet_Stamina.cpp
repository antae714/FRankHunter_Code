// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/FHAttributeSet_Stamina.h"

#include "Net/UnrealNetwork.h"
#include "FRankHunter.h"
#include "Player/FHPlayerBase.h"
#include "GAS/FHGameplayTags.h"

void UFHAttributeSet_Stamina::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Stamina, Stamina, OldValue);
}

void UFHAttributeSet_Stamina::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Stamina, MaxStamina, OldValue);
}

void UFHAttributeSet_Stamina::OnRep_NaturalRegenInitDelay(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Stamina, NaturalRegenInitDelay, OldValue);
}

void UFHAttributeSet_Stamina::OnRep_NaturalRegenAmountPerSec(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Stamina, NaturalRegenAmountPerSec, OldValue);
}

void UFHAttributeSet_Stamina::OnRep_StaminaCost(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Stamina, StaminaCost, OldValue);
}

void UFHAttributeSet_Stamina::SetIncreaseCurrentStaminaWhenMaxStaminaIncreased(bool NewValue)
{
	bIncreaseCurrentStaminaWhenMaxStaminaIncreased = NewValue;
}

void UFHAttributeSet_Stamina::SetUseExhaustedSystem(bool NewValue)
{
	bUseExhaustedSystem = NewValue;
}

void UFHAttributeSet_Stamina::SetRecoveredFromExhaustedStateRatio(float NewValue)
{
	RecoveredFromExhaustedStateRatio = NewValue;
}

UFHAttributeSet_Stamina::UFHAttributeSet_Stamina()
{
	//IsExhaustedTag = GET_GAMEPLAY_TAG_PLAYER_STATE_ISEXHAUSTED;
}

void UFHAttributeSet_Stamina::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFHAttributeSet_Stamina, Stamina);
	DOREPLIFETIME(UFHAttributeSet_Stamina, MaxStamina);
	DOREPLIFETIME(UFHAttributeSet_Stamina, NaturalRegenInitDelay);
	DOREPLIFETIME(UFHAttributeSet_Stamina, NaturalRegenAmountPerSec);
	DOREPLIFETIME(UFHAttributeSet_Stamina, StaminaCost);
}

void UFHAttributeSet_Stamina::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
}

void UFHAttributeSet_Stamina::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetStaminaAttribute())
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		if (ASC)
		{
			FGameplayTag IsExhaustedTag = GET_GAMEPLAY_TAG_PLAYER_STATE_ISEXHAUSTED;
			bool hasIsExhaustedTag = ASC->HasMatchingGameplayTag(IsExhaustedTag);
			if (NewValue == 0.0f && !hasIsExhaustedTag)
			{
				ASC->AddLooseGameplayTag(IsExhaustedTag);
				PRINT_LOG(TEXT("Player is exhausted."));
			}
			else if (NewValue >= GetMaxStamina() * RecoveredFromExhaustedStateRatio && hasIsExhaustedTag)
			{
				ASC->RemoveLooseGameplayTag(IsExhaustedTag);
				PRINT_LOG(TEXT("Player is recovered from exhausted."));
			}
		}
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		if (bIncreaseCurrentStaminaWhenMaxStaminaIncreased)
		{
			float gap = NewValue - OldValue;
			if (gap > 0.0f)
			{
				SetStamina(GetStamina() + gap);
			}
		}

		if (GetStamina() > NewValue)
		{
			SetStamina(NewValue);
		}
	}
}
