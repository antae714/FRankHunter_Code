// Copyright F Rank Hunter. All Rights Reserved.


#include "Animation/FHAnimNotify_CheckAttackHit.h"
#include "AbilitySystemBlueprintLibrary.h"

UFHAnimNotify_CheckAttackHit::UFHAnimNotify_CheckAttackHit()
{
	ComboAttackStep = 1.0f;
}

void UFHAnimNotify_CheckAttackHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor)
		{
			FGameplayEventData PayloadData;
			PayloadData.EventMagnitude = ComboAttackStep;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggeredTag, PayloadData);
		}
	}
}

FString UFHAnimNotify_CheckAttackHit::GetNotifyName_Implementation() const
{
	return TEXT("Notify_CheckAttackHit");
}
