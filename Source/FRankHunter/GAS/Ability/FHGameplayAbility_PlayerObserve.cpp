// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/FHGameplayAbility_PlayerObserve.h"
#include "GAS/FHGameplayTags.h"
#include "Core/FHGateGameStateBase.h"
#include "Core/FHPlayerController.h"
#include "Player/FHObserverPawn.h"

UFHGameplayAbility_PlayerObserve::UFHGameplayAbility_PlayerObserve()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// Observe Prev or Next
	//AbilityInputID = EFHPlayerAbilityInputID::ObserveNext;

	ActivationRequiredTags.AddTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISOBSERVING);
}

bool UFHGameplayAbility_PlayerObserve::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	AFHGateGameStateBase* GameState = GetWorld()->GetGameState<AFHGateGameStateBase>();
	TArray<AFHPlayerStateBase*> AlivePlayers;
	GameState->GetAlivePlayerList(AlivePlayers);
	if (GameState == nullptr || AlivePlayers.Num() <= 0)
	{
		return false;
	}

	return true;
}

void UFHGameplayAbility_PlayerObserve::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);

	AFHPlayerController* FHPC = Cast<AFHPlayerController>(OwnerInfo->PlayerController.Get());
	AFHObserverPawn* ObserverPawn = Cast<AFHObserverPawn>(OwnerInfo->AvatarActor);
	if (FHPC && ObserverPawn)
	{
		FHPC->RequestChangeViewTarget(ObserverPawn, bIsObserveNext);
	}
	
	EndAbility(Handle, OwnerInfo, ActivationInfo, true, false);
}