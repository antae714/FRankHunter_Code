// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTags.h"
#include "GAS/FHAbilityTypes.h"
#include "FHAnimNotify_ActiveAbility.generated.h"

class UGameplayAbility;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHAnimNotify_ActiveAbility : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(Category = Node, EditAnywhere)
	EAbilityActiveType AbilityActiveType;

	UPROPERTY(Category = Node, EditAnywhere, meta = (EditCondition = "AbilityActiveType==EAbilityActiveType::Tag", EditConditionHides))
	FGameplayTag AbilityTag;

	UPROPERTY(Category = Node, EditAnywhere, meta = (EditCondition = "AbilityActiveType==EAbilityActiveType::Class", EditConditionHides))
	TSubclassOf<UGameplayAbility> AbilityClass;
};
