// Copyright F Rank Hunter. All Rights Reserved.


#include "BTElement/Tasks/BTTask_ActivateAbility.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
//UBTTask_ActivateAbility

UBTTask_ActivateAbility::UBTTask_ActivateAbility()
{
	INIT_TASK_NODE_NOTIFY_FLAGS();

	bIsTaskEndCancleAbility = true;
}

void UBTTask_ActivateAbility::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	//FAbilityTaskMemory* MyMemory = reinterpret_cast<FAbilityTaskMemory*>(NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControllingPawn);
	if (!AbilitySystemComponent)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 실행 중인 어빌리티 목록 가져오기
	bool bHasTaggedAbilityRunning = false;

	switch (AbilityActiveType)
	{
	case EAbilityActive::Tag:
	{
		for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if (!Spec.IsActive())
			{
				if (Spec.Ability && Spec.Ability->GetAssetTags().HasTag(AbilityTag))
				{
					bHasTaggedAbilityRunning = true;
					break;
				}
			}
		}
	}
	break;
	case EAbilityActive::Class:
	{
		const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass);

		if (!Spec || !Spec->IsActive())
		{
			bHasTaggedAbilityRunning = true;
		}
	}
	break;
	default:
		break;
	}

	if (bHasTaggedAbilityRunning)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}

EBTNodeResult::Type UBTTask_ActivateAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//FAbilityTaskMemory* MyMemory = reinterpret_cast<FAbilityTaskMemory*>(NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	bool IsSuccess = false;
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControllingPawn);
	if (ensure(AbilitySystemComponent))
	{
		switch (AbilityActiveType)
		{
		case EAbilityActive::Tag:
		{
			IsSuccess = AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer{ AbilityTag });
		}
		break;
		case EAbilityActive::Class:
		{
			IsSuccess = AbilitySystemComponent->TryActivateAbilityByClass(AbilityClass);
		}
		break;
		default:
			break;
		}
	}

	if (!bIsSkipWaitForAbilityEnd)
	{
		return EBTNodeResult::InProgress;
	}
	else if (!IsSuccess)
	{
		return EBTNodeResult::Failed;
	}
	else
	{
		return EBTNodeResult::Succeeded;
	}
}

EBTNodeResult::Type UBTTask_ActivateAbility::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//FAbilityTaskMemory* MyMemory = reinterpret_cast<FAbilityTaskMemory*>(NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Aborted;
	}

	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControllingPawn);
	if (!AbilitySystemComponent)
	{
		return EBTNodeResult::Aborted;
	}

	if (bIsTaskEndCancleAbility)
	{
		switch (AbilityActiveType)
		{
		case EAbilityActive::Tag:
		{
			TArray<FGameplayAbilitySpecHandle> AbilityHandles;
			AbilitySystemComponent->FindAllAbilitiesWithTags(AbilityHandles, FGameplayTagContainer{ AbilityTag });
			for (const FGameplayAbilitySpecHandle& SpecHandle : AbilityHandles)
			{
				if (SpecHandle.IsValid())
				{

					AbilitySystemComponent->CancelAbilityHandle(SpecHandle);
				}
			}
		}
		break;
		case EAbilityActive::Class:
		{
			const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass);

			if (!Spec || Spec->IsActive())
			{
				AbilitySystemComponent->CancelAbilityHandle(Spec->Handle);
			}
		}
		break;
		default:
			break;
		}


	}
	return EBTNodeResult::Aborted;
}

