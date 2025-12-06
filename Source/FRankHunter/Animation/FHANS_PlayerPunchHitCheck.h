// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "FHANS_PlayerPunchHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHANS_PlayerPunchHitCheck : public UAnimNotifyState
{
	GENERATED_BODY()

	bool bEnabled{ true };

	UPROPERTY()
	TArray<AActor*> AlreadyHitObjects;

	UPROPERTY()
	class AFHPlayerBase* Player{ nullptr };

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
	FString SocketName{ "PunchHitCheckPoint" };

	UPROPERTY(EditAnywhere)
	uint8 bDrawDebugSphere : 1{ false };

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	void ActivateCheckAttackHit(AActor* OwnerActor, FHitResult& hitResult);
};
