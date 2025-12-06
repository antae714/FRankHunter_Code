// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/FHActivatableWidget.h"
#include "Editor/WidgetCompilerLog.h"

#define LOCTEXT_NAMESPACE "FHRankHunter"

UFHActivatableWidget::UFHActivatableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TOptional<FUIInputConfig> UFHActivatableWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case ECommonInputMode::All:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case ECommonInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case ECommonInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	default:
		return TOptional<FUIInputConfig>();
	}
}

#undef LOCTEXT_NAMESPACE