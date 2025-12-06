// Copyright F Rank Hunter. All Rights Reserved.


#include "Animation/Notify/FHAnimNotify_SendGameplayEvent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UFHAnimNotify_SendGameplayEvent::UFHAnimNotify_SendGameplayEvent()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor{255, 128, 0};
#endif
}

void UFHAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;

    AActor* OwnerActor = MeshComp->GetOwner();
    if (!OwnerActor) return;

    IAbilitySystemInterface* interface = Cast<IAbilitySystemInterface>(OwnerActor);
    if (!interface) return;

    if (!EventTag.IsValid()) return;

    UAbilitySystemComponent* asc = interface->GetAbilitySystemComponent();
    if (!asc) return;

    FGameplayEventData EventData;
    EventData.Instigator = OwnerActor;
    EventData.Target = OwnerActor;

    asc->HandleGameplayEvent(EventTag, &EventData);
}
