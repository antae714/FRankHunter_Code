// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "Widgets/GameSettingScreen.h"
#include "FHSettingScreen.generated.h"

class UFHTabListWidget;
/**
 * 
 */
UCLASS(Abstract, meta = (Category = "Settings", DisableNativeTick))
class FRANKHUNTER_API UFHSettingScreen : public UGameSettingScreen
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual UGameSettingRegistry* CreateRegistry() override;

	virtual void OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty) override;

	void HandleCloseAction();
	void HandleApplyAction();
	void HandleCancelChangesAction();

protected:
	UPROPERTY(BlueprintReadOnly, Category = Input, meta = (BindWidget, OptionalWidget = true, AllowPrivateAccess = true))
	TObjectPtr<UFHTabListWidget> TabListWidget;

	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle CloseInputActionData;
	FUIActionBindingHandle CloseHandle;

	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle ApplyInputActionData;
	FUIActionBindingHandle ApplyHandle;

	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle CancelChangesInputActionData;
	FUIActionBindingHandle CancelChangesHandle;
};
