// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/FHAbilitySystemComponent.h"
#include "FRankHunter.h"
#include "DataAsset/FH_ASCDescriptor.h"
#include "GAS/FHGameplayAbility.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Animation/AbilityAnimInstance.h"
#include "GAS/Attributes/FHAttributeSet.h"


void UFHAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	const TArray<UAttributeSet*>& Attributes = GetSpawnedAttributes();
	for (auto& AttributeSet : Attributes)
	{
		UFHAttributeSet* FHAttributeSet = Cast<UFHAttributeSet>(AttributeSet);
		if (FHAttributeSet)
		{
			FHAttributeSet->OnInitAbilityActorInfo();

		}
	}

}

void UFHAbilitySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (!InitalizingDescriptor) return;
	ensure(GetOwnerActor());
	if (!GetOwnerActor()) return;
	if (!GetOwnerActor()->HasAuthority()) return;

	for (const auto& ability : InitalizingDescriptor->InitialAbilities)
	{
		if (ability)
		{
			TSubclassOf<UGameplayAbility> rawAbility = ability.Get();
			UFHGameplayAbility* FHAbility = Cast<UFHGameplayAbility>(rawAbility.GetDefaultObject());
			FGameplayAbilitySpec spec(rawAbility, 1, static_cast<int32>(FHAbility ? FHAbility->AbilityInputID : EFHPlayerAbilityInputID::None));
			GiveAbility(spec);
		}
	}

	for (const auto& effect : InitalizingDescriptor->InitialEffects)
	{
		TSubclassOf<UGameplayEffect> rawEffect = effect.Get();
		FGameplayEffectSpecHandle effectSpec = MakeOutgoingSpec(rawEffect, 1, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*effectSpec.Data.Get());
	}

	for (const auto& tag : InitalizingDescriptor->InitialTags)
	{
		AddLooseGameplayTag(tag);
	}

	IsInitialized = true;

	PRINT_LOG(TEXT("Initialize ASC which owned by %s"), *GetOwnerActor()->GetName());

	OnAbilitySystemInitializedEvent.Broadcast();
}

void UFHAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	OnAbilityGivenDelegate.Broadcast(AbilitySpec);
}
