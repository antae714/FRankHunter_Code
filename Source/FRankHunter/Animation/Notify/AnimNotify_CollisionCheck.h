// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_CollisionCheck.generated.h"

UENUM(BlueprintType, meta = (Bitflags))
enum class ETeamAttitudeFlags : uint8
{
	None = 0 UMETA(Hidden),
	Friendly = 1 << 0,
	Neutral = 1 << 1,
	Hostile = 1 << 2,
};
ENUM_CLASS_FLAGS(ETeamAttitudeFlags)


/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UAnimNotify_CollisionCheck : public UAnimNotify
{
	GENERATED_BODY()
	

public:
	UAnimNotify_CollisionCheck();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (AnimNotifyBoneName = "true"))
	FName FirstSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (AnimNotifyBoneName = "true"))
	FName SecondSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (Bitmask, BitmaskEnum = "ETeamAttitudeFlags"))
	int32 AttitudeMask;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	uint32 bIsDebugDraw : 1;
#endif


	FVector LastFirstPosition;
	FVector LastSecondPosition;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
#if WITH_EDITOR
	virtual void DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp, const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const override;
#endif

};
