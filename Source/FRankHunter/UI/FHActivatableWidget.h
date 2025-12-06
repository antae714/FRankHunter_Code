// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CommonActivatableWidget.h"
#include "FHActivatableWidget.generated.h"

/**
 * UI 화면 활성/비활성화을 위한 입력을 관리
 */
UCLASS(Abstract, Blueprintable)
class FRANKHUNTER_API UFHActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	UFHActivatableWidget(const FObjectInitializer& ObjectInitializer);

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Input)
	ECommonInputMode InputConfig;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
