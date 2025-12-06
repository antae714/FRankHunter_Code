// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/Abilities/FH_GA_UseItem.h"
#include "BluePrintFunctions\FHBlueprintFunctionLibrary.h"
#include "Item\FHInventoryComponent.h"
#include "Item/FHItemBase.h"
#include "AbilitySystemComponent.h"
#include "GAS/FHGameplayTags.h"
#include "Kismet\KismetSystemLibrary.h"
#include "Player/FHPlayerBase.h"
#include "Core/FHPlayerStateBase.h"
#include "GAS/Ability/Skill/FHGA_Skill_Tinker.h"

UFH_GA_UseItem::UFH_GA_UseItem()
{
	SetAssetTags(FGameplayTagContainer{ GET_GAMEPLAY_TAG("Player.Ability.UseItem")});
}

bool UFH_GA_UseItem::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) == false)
	{
		return false;
	}
	UFHInventoryComponent* InventoryComponent = UFHBlueprintFunctionLibrary::GetInventoryComponent(ActorInfo->AvatarActor.Get());
	UFHItemBase* ItemInstance = InventoryComponent->GetCurrentItem();
	
	if (ItemInstance && ItemInstance->CanUse())
	{
		return true;
	}


	return false;

}

void UFH_GA_UseItem::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	bIsSucess = false;
}

void UFH_GA_UseItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
									 const FGameplayAbilityActorInfo* ActorInfo, 
									 const FGameplayAbilityActivationInfo ActivationInfo, 
									 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	if (bIsSucess)
	{
	}
}

void UFH_GA_UseItem::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (!bWasCancelled)
	{
		UFHInventoryComponent* InventoryComponent = UFHBlueprintFunctionLibrary::GetInventoryComponent(GetCurrentActorInfo()->AvatarActor.Get());
		if (InventoryComponent)
		{
			UFHItemBase* ItemInstance = InventoryComponent->GetCurrentItem();
			if (ItemInstance)
			{
				ItemInstance->RefreshCooldown();
			}
		}
	}
}


void UFH_GA_UseItem::UseSucess()
{
	bIsSucess = true;

	UFHInventoryComponent* InventoryComponent = UFHBlueprintFunctionLibrary::GetInventoryComponent(GetCurrentActorInfo()->AvatarActor.Get());
	if (!InventoryComponent) return;
	UFHItemBase* ItemInstance = InventoryComponent->GetCurrentItem();
	if (!ItemInstance)
	{
		return;
	}

	SendGameplayEvent(GET_GAMEPLAY_TAG_GAMEPLAYEVENT_CONSUMECONDITION, FGameplayEventData());
}

UFH_GA_ItemConsumeCondition::UFH_GA_ItemConsumeCondition()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = GET_GAMEPLAY_TAG_GAMEPLAYEVENT_CONSUMECONDITION;
	AbilityTriggers.Add(TriggerData);
}

void UFH_GA_ItemConsumeCondition::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AFHPlayerBase* Player = Cast<AFHPlayerBase>(GetCurrentActorInfo()->AvatarActor.Get());

	UFHInventoryComponent* InventoryComponent = UFHBlueprintFunctionLibrary::GetInventoryComponent(Player);
	UFHItemBase* ItemInstance = InventoryComponent->GetCurrentItem();

	if (ItemInstance == nullptr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 애니메이션
	const FFHItemData& data = ItemInstance->GetItemData();
	if (data.bHasUsingMotion == true)
	{
		Player->InformPlayerItemWhichHasUsingMotionUsed();
	}

	// 능력
	bool bTinkerEffectApplied = false;
	if (HasAuthority(&ActivationInfo))
	{
		if (data.Type == EItemType::Tool && ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISTINKERON))
		{
			UFHGamePlaySkillAbility* TinkerAbility = nullptr;
			TArray<FGameplayAbilitySpec*> TinkerAbilitySpecs;
			ActorInfo->AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(FGameplayTagContainer(GET_GAMEPLAY_TAG_SKILL_TINKER), TinkerAbilitySpecs);

			if (TinkerAbilitySpecs.Num() > 0 && TinkerAbilitySpecs[0]->Ability)
			{
				TinkerAbility = Cast<UFHGamePlaySkillAbility>(TinkerAbilitySpecs[0]->Ability);
			}

			if (TinkerAbility)
			{
				if (const FFHSkillTable_Tinker* TinkerData = TinkerAbility->GetSkillData<FFHSkillTable_Tinker>(TEXT("Weight")))
				{
					const int32 CurrentLevel = Player->GetPlayerState<AFHPlayerStateBase>()->GetPlayerLevel();
					const float NoConsumeChance = TinkerData->GetValueByLevel(CurrentLevel) / 100.0f;
					const float RandomValue = FMath::FRand(); // 0.0 ~ 1.0

					UE_LOG(LogTemp, Log, TEXT("Tinker Skill On! : Level=%d, SuccessChance=%.2f, RolledValue=%.2f"), CurrentLevel, NoConsumeChance, RandomValue);

					const bool bSuccess = RandomValue <= NoConsumeChance;
					if (bSuccess)
					{
						UE_LOG(LogTemp, Log, TEXT("Success! No condition consumed."));
					}
					else
					{
						UE_LOG(LogTemp, Log, TEXT("Failed! Consume condition x2."));
						ItemInstance->ConsumeCondition();
						ItemInstance->ConsumeCondition();
					}

					bTinkerEffectApplied = true;

					if (AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(Player->GetPlayerState()))
					{
						PS->Client_OnSkillFeedback(bSuccess);
					}
				}
			}
		}
	}

	if (HasAuthority(&ActivationInfo))
	{
		if (!bTinkerEffectApplied)
		{
			ItemInstance->ConsumeCondition();
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
