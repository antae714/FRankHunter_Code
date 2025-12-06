// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CommonButtonBase.h"
#include "FHButtonBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class FRANKHUNTER_API UFHButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetButtonText(const FText& InText);

protected:
	virtual void NativePreConstruct() override;
	virtual void UpdateInputActionWidget() override;
	virtual void OnInputMethodChanged(ECommonInputType CurrentInputType) override;

	void RefreshButtonText();
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonText(const FText& InText);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonStyle();

private:
	UPROPERTY(EditAnywhere, Category="Button", meta=(InlineEditConditionToggle))
	uint8 bOverride_ButtonText : 1 = false;
	
	UPROPERTY(EditAnywhere, Category="Button", meta=( editcondition="bOverride_ButtonText" ))
	FText ButtonText;
};
