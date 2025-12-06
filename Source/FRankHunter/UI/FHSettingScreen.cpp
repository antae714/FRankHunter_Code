// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/FHSettingScreen.h"
#include "Settings/FHSettingRegistry.h"
#include "Player/FHLocalPlayer.h"
#include "Input/CommonUIInputTypes.h"
#include "UI/Foundation/FHTabListWidget.h"
#include "Core/FHPlayerController.h"

void UFHSettingScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CloseHandle = RegisterUIActionBinding(FBindUIActionArgs(CloseInputActionData, false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleCloseAction)));

	ApplyHandle = RegisterUIActionBinding(FBindUIActionArgs(ApplyInputActionData, false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleApplyAction)));

	CancelChangesHandle = RegisterUIActionBinding(FBindUIActionArgs(CancelChangesInputActionData, false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleCancelChangesAction)));
}

UGameSettingRegistry* UFHSettingScreen::CreateRegistry()
{
	UFHSettingRegistry* NewRegistry = NewObject<UFHSettingRegistry>();

	if (UFHLocalPlayer* LocalPlayer = CastChecked<UFHLocalPlayer>(GetOwningLocalPlayer()))
	{
		NewRegistry->Initialize(LocalPlayer);
	}

	return NewRegistry;
}

void UFHSettingScreen::OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty)
{
	if (bSettingsDirty)
	{
		if (!GetActionBindings().Contains(ApplyHandle))
		{
			AddActionBinding(ApplyHandle);
		}
		if (!GetActionBindings().Contains(CancelChangesHandle))
		{
			AddActionBinding(CancelChangesHandle);
		}
	}
	else
	{
		RemoveActionBinding(ApplyHandle);

		RemoveActionBinding(CancelChangesHandle);
	}
}

void UFHSettingScreen::HandleCloseAction()
{
	if (AttemptToPopNavigation())
	{
		return;
	}

	ApplyChanges();

	DeactivateWidget();

	if (AFHPlayerController* PC = GetOwningPlayer<AFHPlayerController>())
	{
		PC->ToggleGameMenu();
	}
}

void UFHSettingScreen::HandleApplyAction()
{
	ApplyChanges();
}

void UFHSettingScreen::HandleCancelChangesAction()
{
	CancelChanges();
}


