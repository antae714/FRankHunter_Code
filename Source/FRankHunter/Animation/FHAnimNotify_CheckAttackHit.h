// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"

#include "FHAnimNotify_CheckAttackHit.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHAnimNotify_CheckAttackHit : public UAnimNotify
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere)
	float ComboAttackStep;

	UPROPERTY(EditAnywhere, Meta=(Categories=Event))
	FGameplayTag TriggeredTag;

public:
	UFHAnimNotify_CheckAttackHit();

protected:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
