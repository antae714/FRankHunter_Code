// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "FHAbilityTask_WaitMeshDontMove.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitMeshDontMoveDelegate, FVector, LastLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitMeshDontMoveDelegate);

UCLASS()
class FRANKHUNTER_API UFHAbilityTask_WaitMeshDontMove : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FWaitMeshDontMoveDelegate OnMeshDontMove;

protected:
	UPROPERTY()
	class USkeletalMeshComponent* TargetMesh{ nullptr };
	UPROPERTY()
	float SpeedThreshold{ 10.0f };
	UPROPERTY()
	FName BoneInstanceName{ TEXT("pelvis") };
	UPROPERTY()
	float ReadyDelay{ 0.5f };
	float ElapsedTime{ 0.0f };


public:
	static UFHAbilityTask_WaitMeshDontMove* WaitForMeshStop(UGameplayAbility* OwningAbility, class USkeletalMeshComponent* TargetMesh, float SpeedThreshold = 10.0f, FName BoneInstanceName = TEXT("pelvis"), float ReadyDelay = 0.5f);
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
};
