// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/Foundation/FHButtonBase.h"
#include "CommonActionWidget.h"

void UFHButtonBase::SetButtonText(const FText& InText)
{
	ButtonText = InText;

	RefreshButtonText();
}

void UFHButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	UpdateButtonStyle();
	RefreshButtonText();
}

void UFHButtonBase::UpdateInputActionWidget()
{
	Super::UpdateInputActionWidget();

	UpdateButtonStyle();

	RefreshButtonText();
}

void UFHButtonBase::OnInputMethodChanged(ECommonInputType CurrentInputType)
{
	Super::OnInputMethodChanged(CurrentInputType);

	UpdateButtonStyle();
}

void UFHButtonBase::RefreshButtonText()
{
	/*if (bOverride_ButtonText)
	{
		UpdateButtonText(ButtonText);
		return;
	}

	if (InputActionWidget)
	{
		const FText ActionDisplayText = InputActionWidget->GetDisplayText();
		if (!ActionDisplayText.IsEmpty())
		{
			UpdateButtonText(ActionDisplayText);
			return;
		}
	}*/

	UpdateButtonText(ButtonText);
}