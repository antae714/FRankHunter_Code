// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/FRankHunterTypes.h"
#include "FHFadeInOut.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFadeInEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFadeOutEnd);

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHFadeInOut : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnFadeInEnd OnFadeInEndDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnFadeOutEnd OnFadeOutEndDelegate;

	void PlayFadeIn();
	void PlayFadeOut();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_PlayFadeIn();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_PlayFadeOut();

	FOnFadeEnd OnFadeEndDelegate;
protected:

	UFUNCTION(BlueprintCallable)
	void OnFadeInEnd();
	UFUNCTION(BlueprintCallable)
	void OnFadeOutEnd();
};
