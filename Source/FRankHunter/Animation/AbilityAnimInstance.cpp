	// Copyright F Rank Hunter. All Rights Reserved.


#include "Animation/AbilityAnimInstance.h"
#include "GAS/FHAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAbilityAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner());
	if (AbilitySystemComponent)
	{
		AbilityPropertyMap.Initialize(this, AbilitySystemComponent);
		bIsASCInit = true;
	}


}

void UAbilityAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (bIsASCInit)
	{
		AbilityPropertyMap.ApplyCurrentTags();
	}
	
}
