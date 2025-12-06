// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/FHAttributeSet_Health.h"
#include "FRankHunter.h"
#include "Net/UnrealNetwork.h"

#include "Player/FHPlayerBase.h"
#include "GameplayEffectExtension.h"


void UFHAttributeSet_Health::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Health, Health, OldValue);

	OnHealthChanged.Broadcast();
	if (GetHealth() <= 0.0f)
	{
		OnHealthZeroDelegate.Broadcast();
	}

	if (bOutOfHealth && (GetHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}
}

void UFHAttributeSet_Health::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Health, MaxHealth, OldValue);
}

void UFHAttributeSet_Health::OnRep_Damage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Health, Damage, OldValue);
}

void UFHAttributeSet_Health::OnRep_Healing(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Health, Healing, OldValue);
}

void UFHAttributeSet_Health::OnRep_NaturalRegenInitDelay(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Health, NaturalRegenInitDelay, OldValue);
}

void UFHAttributeSet_Health::OnRep_NaturalRegenAmountPerSec(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Health, NaturalRegenAmountPerSec, OldValue);
}

void UFHAttributeSet_Health::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFHAttributeSet_Health, Health);
	DOREPLIFETIME(UFHAttributeSet_Health, MaxHealth);
	DOREPLIFETIME(UFHAttributeSet_Health, Damage);
	DOREPLIFETIME(UFHAttributeSet_Health, Healing);
	DOREPLIFETIME(UFHAttributeSet_Health, NaturalRegenInitDelay);
	DOREPLIFETIME(UFHAttributeSet_Health, NaturalRegenAmountPerSec);
}

void UFHAttributeSet_Health::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		if (GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISIMMORTAL))
		{
			float OldHealth = GetHealth();
			NewValue = OldHealth > NewValue ? OldHealth : NewValue;
		}
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
	}
}

void UFHAttributeSet_Health::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		OnHealthChanged.Broadcast();
		if (NewValue <= 0.0f)
		{
			OnHealthZeroDelegate.Broadcast();
		}
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		if (bIncreaseCurrentHealthWhenMaxHealthIncreased)
		{
			float gap = NewValue - OldValue;
			if (gap > 0.0f)
			{
				SetHealth(GetHealth() + gap);
			}
		}

		if (GetHealth() > NewValue)
		{
			SetHealth(NewValue);
		}

		if (bOutOfHealth && (GetHealth() > 0.0f))
		{
			bOutOfHealth = false;
		}
	}
}

void UFHAttributeSet_Health::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		UAbilitySystemComponent* AbilitySystemComponent = GetOwningAbilitySystemComponent();
		{
			// 라이라LyraHealthComponent cpp 148 참고
			//OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());

			FGameplayEventData Payload;
			Payload.EventTag = GET_GAMEPLAY_TAG_GAMEPLAYEVENT_DEATH;
			Payload.Instigator = Cast<APawn>(GetActorInfo()->AvatarActor);
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = Data.EffectSpec.Def;
			Payload.ContextHandle = Data.EffectSpec.GetEffectContext();
			Payload.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = Data.EvaluatedData.Magnitude;

			FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}
	}

	// Check health again in case an event above changed it.
	bOutOfHealth = (GetHealth() <= 0.0f);

	

}
