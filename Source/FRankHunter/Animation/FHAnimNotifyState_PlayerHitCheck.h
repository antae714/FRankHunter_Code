// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "FHAnimNotifyState_PlayerHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHAnimNotifyState_PlayerHitCheck : public UAnimNotifyState
{
	GENERATED_BODY()

	bool bEnabled{ true };

	UPROPERTY(Transient)
	TArray<AActor*> AlreadyHitObjects;

	UPROPERTY(Transient)
	class AFHPlayerBase* Player{ nullptr };

	UPROPERTY(Transient)
	class UStaticMeshComponent* Equipment{ nullptr };

public:
	UPROPERTY(EditAnywhere)
	uint8 bAllowMultiHit : 1{ false };
	UPROPERTY(EditAnywhere)
	uint8 bIgnoreNonHitableObject : 1{ true };

	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> CollisionChannels{
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic)
	};
	
	UPROPERTY(EditAnywhere, Meta=(Categories=Event))
	FGameplayTag TriggeredTag;

	UPROPERTY(EditAnywhere)
	float SphereColliderRadius{ 25.0f };

	UPROPERTY(EditAnywhere)
	FString Socket1Name{ "HitCheckPoint1" };
	UPROPERTY(EditAnywhere)
	FString Socket2Name{ "HitCheckPoint2" };

	UPROPERTY(EditAnywhere)
	uint8 bDrawDebugSphere : 1{ false };

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	void ActivateCheckAttackHit(AActor* OwnerActor, FHitResult& hitResult);


};
