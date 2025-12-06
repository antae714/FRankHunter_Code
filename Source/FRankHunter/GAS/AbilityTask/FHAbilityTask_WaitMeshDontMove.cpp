// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/AbilityTask/FHAbilityTask_WaitMeshDontMove.h"
#include "Components/SkeletalMeshComponent.h"
#include "FRankHunter.h"

UFHAbilityTask_WaitMeshDontMove::UFHAbilityTask_WaitMeshDontMove(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
}

UFHAbilityTask_WaitMeshDontMove* UFHAbilityTask_WaitMeshDontMove::WaitForMeshStop(UGameplayAbility* OwningAbility, class USkeletalMeshComponent* TargetMesh, float SpeedThreshold /*= 10.0f*/, FName BoneInstanceName, float ReadyDelay)
{
	if (TargetMesh == nullptr)
	{
		return nullptr;
	}

	UFHAbilityTask_WaitMeshDontMove* Task = NewAbilityTask<UFHAbilityTask_WaitMeshDontMove>(OwningAbility);
	Task->TargetMesh = TargetMesh;
	Task->SpeedThreshold = SpeedThreshold;
	Task->BoneInstanceName = BoneInstanceName;
	Task->ReadyDelay = ReadyDelay;

	return Task;
}

void UFHAbilityTask_WaitMeshDontMove::Activate()
{
	SetWaitingOnAvatar();
}

void UFHAbilityTask_WaitMeshDontMove::TickTask(float DeltaTime)
{
	if (ElapsedTime < ReadyDelay)
	{
		ElapsedTime += DeltaTime;
		return;
	}

	// 제미나이가 루트 바디 인스턴스 구하라고 했는데 루트 바디 인스턴스는 속도 무조건 0만 나옴ㅡㅡ
	FBodyInstance* RootBody = TargetMesh->GetBodyInstance(BoneInstanceName);

	FVector velocity = RootBody->GetUnrealWorldVelocity();
	float size = velocity.Size();

	if (size <= SpeedThreshold)
	{
		FVector LastLocation = TargetMesh->GetBoneLocation(BoneInstanceName);
		OnMeshDontMove.Broadcast();
		EndTask();
	}
}
