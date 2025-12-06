// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTags.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "GameplayAbilitySpecHandle.h"
#include "GAS/FHAbilityTypes.h"
#include "BTTask_ActivateAbility.generated.h"

class UGameplayAbility;
struct FAbilityEndedData;


USTRUCT()
struct FAbilityTaskMemory
{
	GENERATED_BODY()

	uint32 bIsAbilitEventActive : 1;
};


/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UBTTask_ActivateAbility : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ActivateAbility();

public:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void OnAbilityEnded(const FAbilityEndedData& EndData);

	UPROPERTY(Category = Node, EditAnywhere)
	EAbilityActive AbilityActiveType;

	UPROPERTY(Category = Node, EditAnywhere, meta = (EditCondition = "AbilityActiveType==EAbilityActive::Tag", EditConditionHides))
	FGameplayTag AbilityTag;

	UPROPERTY(Category = Node, EditAnywhere, meta = (EditCondition = "AbilityActiveType==EAbilityActive::Class", EditConditionHides))
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(Category = Node, EditAnywhere)
	uint32 bIsSkipWaitForAbilityEnd : 1;

	UPROPERTY(Category = Node, EditAnywhere)
	uint32 bIsTaskEndCancleAbility : 1;

	TArray<FGameplayTag> AbilityTags;
};
