// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "FHAnimNotify_SendGameplayEvent.generated.h"


UCLASS(Blueprintable, meta = (DisplayName = "Send Gameplay Event"))
class FRANKHUNTER_API UFHAnimNotify_SendGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag EventTag;

public:
	UFHAnimNotify_SendGameplayEvent();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
