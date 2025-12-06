// Copyright F Rank Hunter. All Rights Reserved.


#include "Animation/Notify/FHAnimNotifyState_NiagaraEffect.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void UFHAnimNotifyState_NiagaraEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp && MeshComp->GetWorld() && NiagaraSystem)
	{
		SpawnedNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			NiagaraSystem, 
			MeshComp, 
			SocketName, 
			FVector::ZeroVector, 
			FRotator::ZeroRotator, 
			EAttachLocation::SnapToTarget, 
			true);
	}
}

void UFHAnimNotifyState_NiagaraEffect::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
}

void UFHAnimNotifyState_NiagaraEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (SpawnedNiagaraComponent)
	{
		SpawnedNiagaraComponent->Deactivate();
	}
}
