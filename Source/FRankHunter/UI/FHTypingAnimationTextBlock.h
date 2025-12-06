// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "GameplayTagContainer.h"
#include "FHTypingAnimationTextBlock.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTextAnimationEnd);

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHTypingAnimationTextBlock : public UTextBlock
{
	GENERATED_BODY()
	
public:
	UFHTypingAnimationTextBlock();
	virtual void BeginDestroy() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	bool bUseAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation", meta = (EditCondition = "bUseAnimation"))
	float LoopDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation", meta = (EditCondition = "bUseAnimation"))
	FText OriginText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation", meta = (EditCondition = "bUseAnimation"))
	FGameplayTag SoundTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (EditCondition = "bUseAnimation"))
	float SoundVolume;

	UFUNCTION(BlueprintCallable)
	void PlayAnimation();
	UFUNCTION(BlueprintCallable)
	void CancelAnimation();

	UPROPERTY(BlueprintAssignable, Category = "AnimationEndCallback")
	FOnTextAnimationEnd OnTextAnimationEndDelegate;
private:
	void Animation();
	void OnTextAnimationEnd();

	FTimerHandle AnimationHandle;
	int32 CurrentIndex;
	int32 LastIndex;

	UPROPERTY()
	class AFHSoundManagerActor* SoundManager;
};
