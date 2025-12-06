// Copyright F Rank Hunter. All Rights Reserved.


#include "Animation/FHANS_PlayerPunchHitCheck.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/FHPlayerBase.h"
#include "GAS/FHAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/FHGameplayAbilityTargetDatas.h"
#include "FRankHunter.h"
#include "GameFramework/PlayerState.h"

void UFHANS_PlayerPunchHitCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	bEnabled = true;

	AlreadyHitObjects.Empty();
	Player = nullptr;
}

void UFHANS_PlayerPunchHitCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (bEnabled == false) return;

	if (!Player)
	{
		Player = Cast<AFHPlayerBase>(MeshComp->GetOwner());
	}
	if (!Player) return;
	if (Player->IsLocallyControlled() == false) return;

	FVector socketPos = MeshComp->GetSocketLocation(*SocketName);

	TArray<FHitResult> HitResults;
	EDrawDebugTrace::Type DebugDrawType = bDrawDebugSphere ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	bool bIsHit = UKismetSystemLibrary::SphereTraceMultiForObjects(MeshComp, socketPos, socketPos, SphereColliderRadius, CollisionChannels, false, AlreadyHitObjects, DebugDrawType, HitResults, true);

	if (bIsHit)
	{
		for (FHitResult& result : HitResults)
		{
			bool isNoneHitable = result.GetComponent()->ComponentHasTag("NoneHitable");
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

void UFHANS_PlayerPunchHitCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{

}

void UFHANS_PlayerPunchHitCheck::ActivateCheckAttackHit(AActor* OwnerActor, FHitResult& hitResult)
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
	PayloadData.TargetTags.AddTag(GET_GAMEPLAY_TAG("Player.Ability.Punch"));
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Cast<AActor>(ps), TriggeredTag, PayloadData);
}
