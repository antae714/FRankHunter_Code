// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "AnimNotifyState_CollisionCheck.generated.h"



UENUM(BlueprintType, meta = (Bitflags))
enum class ETeamAttitudeFlags2 : uint8
{
	None = 0 UMETA(Hidden),
	Friendly = 1 << 0,
	Neutral = 1 << 1,
	Hostile = 1 << 2,
};
ENUM_CLASS_FLAGS(ETeamAttitudeFlags2)


/**
 * 
 */
UCLASS(editinlinenew, Blueprintable)
class FRANKHUNTER_API UAnimNotifyState_CollisionCheck : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_CollisionCheck();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (AnimNotifyBoneName = "true"))
	FName FirstSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (AnimNotifyBoneName = "true"))
	FName SecondSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (Bitmask, BitmaskEnum = "ETeamAttitudeFlags2"))
	int32 AttitudeMask;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	uint32 bIsDebugDraw : 1;
#endif
	uint32 bIsHit : 1;


	FVector LastFirstPosition;
	FVector LastSecondPosition;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

#if WITH_EDITOR
	virtual void DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp, const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const override;
#endif
};
