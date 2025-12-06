// Copyright F Rank Hunter. All Rights Reserved.


#include "Animation/FHAnimNotifyState_PlayerHitCheck.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/FHPlayerBase.h"
#include "GAS/FHAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/FHGameplayAbilityTargetDatas.h"
#include "FRankHunter.h"
#include "GameFramework/PlayerState.h"


void UFHAnimNotifyState_PlayerHitCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	bEnabled = true;

	AlreadyHitObjects.Empty();
	Player = nullptr;
	Equipment = nullptr;
}

void UFHAnimNotifyState_PlayerHitCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (bEnabled == false) return;

	if (!Player)
	{
		Player = Cast<AFHPlayerBase>(MeshComp->GetOwner());
	}
	if (!Player) return;
	
	if (!Equipment)
	{
		Equipment = Player->GetGrabMesh();
	}
	if (!Equipment || Equipment->GetStaticMesh() == nullptr) return;

	FVector socketPos1 = Equipment->GetSocketLocation(*Socket1Name);
	FVector socketPos2 = Equipment->GetSocketLocation(*Socket2Name);

	TArray<FHitResult> HitResults;
	EDrawDebugTrace::Type DebugDrawType = bDrawDebugSphere ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	bool bIsHit = UKismetSystemLibrary::SphereTraceMultiForObjects(MeshComp, socketPos1, socketPos2, SphereColliderRadius, CollisionChannels, false, AlreadyHitObjects, DebugDrawType, HitResults, true);

	if (bIsHit)
	{
		for (FHitResult& result : HitResults)
		{
			if (AlreadyHitObjects.Contains(result.GetActor()))
			{
				continue;
			}
			bool isNoneHitable{ false };
			isNoneHitable |= result.GetComponent()->ComponentHasTag(TEXT("NoneHitable"));
			isNoneHitable |= result.GetActor()->ActorHasTag(TEXT("NoneHitable"));

			if (isNoneHitable)
			{
				continue;
			}

			IAbilitySystemInterface* interface = Cast<IAbilitySystemInterface>(result.GetActor());

			if (interface == nullptr && bIgnoreNonHitableObject)
			{
				continue;
			}

			ActivateCheckAttackHit(Player, result);
			if (bAllowMultiHit)
			{
				AlreadyHitObjects.Add(result.GetActor());
			}
			else
			{
				bEnabled = false;
				return;
			}

		}
	}
}

void UFHAnimNotifyState_PlayerHitCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{

}

void UFHAnimNotifyState_PlayerHitCheck::ActivateCheckAttackHit(AActor* OwnerActor, FHitResult& hitResult)
{
	AActor* TargetActor = hitResult.GetActor();

	if (!OwnerActor || !TargetActor) return;

	APlayerState* ps = Cast<ACharacter>(OwnerActor)->GetPlayerState();
	if (!ps)
	{
		PRINT_LOG(TEXT("Cannot find PlayerState from this context."));
		return;
	}

	FGameplayAbilityTargetData_SingleTargetHit* targetData = new FGameplayAbilityTargetData_SingleTargetHit;
	targetData->HitResult = hitResult;

	FGameplayEventData PayloadData;
	PayloadData.Target = TargetActor;
	PayloadData.TargetData.Add(targetData);
	PayloadData.Instigator = OwnerActor;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Cast<AActor>(ps), TriggeredTag, PayloadData);
}
