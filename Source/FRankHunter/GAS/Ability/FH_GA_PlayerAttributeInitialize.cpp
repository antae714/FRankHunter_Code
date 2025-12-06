// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/FH_GA_PlayerAttributeInitialize.h"
#include "FRankHunter.h"

#include "Engine/DataTable.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GAS/FHAttributeSet_Health.h"
#include "GAS/FHAttributeSet_Stamina.h"
#include "GAS/FHAttributeSet_PlayerStatus.h"


void UFH_GA_PlayerAttributeInitialize::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		PRINT_LOG(TEXT("Failed to find UAbilitySystemComponent"));
		return;
	}

	for (FAttributeInitializeInfo& info : AttributeInitializeInfoArray)
	{
		InitAttribute(ASC, info);
	}

	if (bUseEffect)
	{
		FGameplayEffectSpecHandle effect = ASC->MakeOutgoingSpec(SettingDerivedAttributeEffect, 1.0f, ASC->MakeEffectContext());
		if (effect.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*effect.Data.Get());
		}
	}
}

void UFH_GA_PlayerAttributeInitialize::InitAttribute(class UAbilitySystemComponent* ASC, FAttributeInitializeInfo& info)
{
	switch (info.InitializeType)
	{
	case EAttributeInitializeType::Float:
	{
		ASC->SetNumericAttributeBase(info.Attribute, info.FloatValue);

		break;
	}
	case EAttributeInitializeType::DataTable:
	{
		if (!InitDataTable) return;

		FString AttributeSetName;
		info.Attribute.GetAttributeSetClass()->GetName(AttributeSetName);
		FName rowName = FName(*(AttributeSetName + TEXT(".") + info.Attribute.GetName()));
		FAttributeMetaData* data = InitDataTable->FindRow<FAttributeMetaData>(rowName, FString{}, true);

		if (!data)
		{
			PRINT_LOG(TEXT("Failed to find row %s in DataTable %s"), *rowName.ToString(), *InitDataTable->GetName());
			return;
		}

		ASC->SetNumericAttributeBase(info.Attribute, data->BaseValue);

		break;
	}
	case EAttributeInitializeType::InfiniteEffect:
	{
		bUseEffect = true;

		break;
	}
	}
}
