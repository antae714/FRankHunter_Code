// Copyright F Rank Hunter. All Rights Reserved.

#include "Animation/Notify/AnimNotifyState_CollisionCheck.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/FHGameplayTags.h"
#include "GAS/FHAbilityTypes.h"
#include "GenericTeamAgentInterface.h"



UAnimNotifyState_CollisionCheck::UAnimNotifyState_CollisionCheck()
{
	EventTag = GET_GAMEPLAY_TAG_GAMEPLAYEVENT_COLLISIONCHECK;
	AttitudeMask = (int32)ETeamAttitudeFlags2::Friendly;
#if WITH_EDITORONLY_DATA
	bIsDebugDraw = true;
#endif
}

void UAnimNotifyState_CollisionCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	LastFirstPosition = MeshComp->GetSocketLocation(FirstSocketName);
	LastSecondPosition = MeshComp->GetSocketLocation(SecondSocketName);
	bIsHit = false;
}

void UAnimNotifyState_CollisionCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp || !MeshComp->GetWorld())
	{
		return;
	}

	if (bIsHit)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();

	// 현재 위치 계산
	FVector CurrFirst = MeshComp->GetSocketLocation(FirstSocketName);
	FVector CurrSecond = MeshComp->GetSocketLocation(SecondSocketName);

	// 캡슐 정보 (스윕 시작)
	FVector StartCenter = (LastFirstPosition + LastSecondPosition) * 0.5f;
	FVector StartUp = (LastSecondPosition - LastFirstPosition).GetSafeNormal();
	float StartHalfHeight = FVector::Distance(LastFirstPosition, LastSecondPosition) * 0.5f;
	FRotator CapsuleRot = FRotationMatrix::MakeFromZ(StartUp).ToQuat().Rotator();
	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(Radius, StartHalfHeight);

	// 스윕 끝 중심
	FVector EndCenter = (CurrFirst + CurrSecond) * 0.5f;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.bTraceComplex = false;
	Params.AddIgnoredActor(OwnerActor);
	MeshComp->GetWorld()->SweepMultiByObjectType(HitResults,
												  StartCenter,
												  EndCenter,
												  CapsuleRot.Quaternion(),
												  FCollisionObjectQueryParams::InitType::AllObjects,
												  CapsuleShape,
												  Params);

	for (auto& HitResult : HitResults)
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			ETeamAttitudeFlags2 Flags = static_cast<ETeamAttitudeFlags2>(AttitudeMask);
			ETeamAttitudeFlags2 Flag = static_cast<ETeamAttitudeFlags2>(FGenericTeamId::GetAttitude(OwnerActor, HitActor));
			if (!EnumHasAnyFlags(Flags, Flag))
			{
				continue; // 적대적이지 않은 팀은 무시
			}


			FGameplayEventData EventData{};
			EventData.Instigator = OwnerActor;
			EventData.Target = HitActor;
			EventData.TargetData.Add(new FGameplayAbilityTargetData_SingleTargetHit(HitResult));

			UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
			if (AbilitySystemComponent)
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
					OwnerActor,
					EventTag,
					EventData);
				bIsHit = true;
				break;
			}
		}
	}

	// 위치 갱신
	LastFirstPosition = CurrFirst;
	LastSecondPosition = CurrSecond;
}

void UAnimNotifyState_CollisionCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

#if WITH_EDITOR
void UAnimNotifyState_CollisionCheck::DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp, const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const
{
	if (!bIsDebugDraw)
	{
		return;
	}

	// 현재 프리뷰 타임 (SkeletalMeshComponent에 저장된 미리보기 시간)
	const float PreviewTime = MeshComp->GetPosition();

	const float StartTime = NotifyEvent.GetTriggerTime();
	const float EndTime = NotifyEvent.GetEndTriggerTime();

	// 현재 프리뷰 타임이 이 NotifyState의 유효시간 안에 있는 경우만 드로우
	if (PreviewTime >= StartTime && PreviewTime <= EndTime)
	{
		FVector CurrFirst = MeshComp->GetSocketLocation(FirstSocketName);
		FVector CurrSecond = MeshComp->GetSocketLocation(SecondSocketName);

		FVector StartCenter = (CurrFirst + CurrSecond) * 0.5f;
		FVector StartUp = (CurrSecond - CurrFirst).GetSafeNormal();
		float StartHalfHeight = FVector::Distance(CurrFirst, CurrSecond) * 0.5f;
		FRotator CapsuleRot = FRotationMatrix::MakeFromZ(StartUp).ToQuat().Rotator();
		FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(Radius, StartHalfHeight);

		DrawDebugCapsule(MeshComp->GetWorld(), StartCenter, StartHalfHeight + Radius, Radius, CapsuleRot.Quaternion(), FColor::Green, false, 1.f);
		DrawDebugPoint(MeshComp->GetWorld(), CurrFirst, 10, FColor::Blue, false, 1.0f);
		DrawDebugPoint(MeshComp->GetWorld(), CurrSecond, 10, FColor::Red, false, 1.0f);
	}

}

#endif
