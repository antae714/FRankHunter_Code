// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FHAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAbilitySystemComponentInitializedDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilityGivenDelegate, const FGameplayAbilitySpec&);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRANKHUNTER_API UFHAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	bool IsInitialized = false;

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UFH_ASCDescriptor> InitalizingDescriptor;

	FOnAbilitySystemComponentInitializedDelegate OnAbilitySystemInitializedEvent;
	FOnAbilityGivenDelegate OnAbilityGivenDelegate;
public:
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	virtual void InitializeComponent() override;
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
};
