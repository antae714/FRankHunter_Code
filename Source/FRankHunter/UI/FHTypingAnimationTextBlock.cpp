// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/FHTypingAnimationTextBlock.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHSoundManagerActor.h"

UFHTypingAnimationTextBlock::UFHTypingAnimationTextBlock() : 
	bUseAnimation(false),
	LoopDelay(0.0f),
	SoundManager(nullptr),
	SoundVolume(1.0f)
{

}

void UFHTypingAnimationTextBlock::BeginDestroy()
{
	Super::BeginDestroy();

	if (AnimationHandle.IsValid())
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(AnimationHandle);
		}
	}
}

void UFHTypingAnimationTextBlock::PlayAnimation()
{
	CurrentIndex = 0;
	if (SoundManager == nullptr)
	{
		SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			AnimationHandle,
			FTimerDelegate::CreateUObject(this, &UFHTypingAnimationTextBlock::Animation),
			LoopDelay,
			true
		);
	}

}

void UFHTypingAnimationTextBlock::CancelAnimation()
{
	if (AnimationHandle.IsValid() && GetWorld()->GetTimerManager().IsTimerActive(AnimationHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(AnimationHandle);
		SetText(FText::FromString(OriginText.ToString()));
		OnTextAnimationEnd();
	}
}

void UFHTypingAnimationTextBlock::Animation()
{
	FString TextString = OriginText.ToString();
	FString Substring = TextString.Mid(CurrentIndex, 1);

	FString CurrentText = GetText().ToString();
	CurrentText = CurrentText + Substring;
	SetText(FText::FromString(CurrentText));
	CurrentIndex++;

	SoundManager->PlaySound2DLocallyByTag(SoundTag, SoundVolume);

	if (CurrentIndex >= TextString.Len())
	{
		OnTextAnimationEnd();
		return;
	}
}

void UFHTypingAnimationTextBlock::OnTextAnimationEnd()
{
	if (AnimationHandle.IsValid() && GetWorld()->GetTimerManager().IsTimerActive(AnimationHandle))
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(AnimationHandle);
		}
	}
	OnTextAnimationEndDelegate.Broadcast();
}
