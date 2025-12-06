// Copyright F Rank Hunter. All Rights Reserved.


#include "Settings/FHSettingRegistry.h"
#include "Settings/FHPlayerProfileSettings.h"
#include "Settings/FHDeviceSettings.h"
#include "Player/FHLocalPlayer.h"
#include "GameSettingCollection.h"
#include "GameSettingValueDiscreteDynamic.h"
#include "GameSettingValueScalarDynamic.h"

#define LOCTEXT_NAMESPACE "FRankHunter"

void UFHSettingRegistry::InitializeControlSettings()
{
	if (OwningLocalPlayer == nullptr)
	{
		return;
	}

	ControlSettings = NewObject<UGameSettingCollection>();
	ControlSettings->SetDevName(TEXT("ControlSettings"));
	ControlSettings->SetDisplayName(LOCTEXT("ControlSettings_Name", "Control"));
	ControlSettings->Initialize(OwningLocalPlayer);

	/** Mouse Sensitivity */
	{
		UGameSettingCollection* Sensitivity = NewObject<UGameSettingCollection>();
		Sensitivity->SetDevName(TEXT("MouseSensitivity"));
		Sensitivity->SetDisplayName(LOCTEXT("MouseSensitivity_Name", "MouseSensitivity"));
		ControlSettings->AddSetting(Sensitivity);

		/** Mouse Sensitivity Yaw */
		{
			UGameSettingValueScalarDynamic* SettingValue = NewObject<UGameSettingValueScalarDynamic>();
			SettingValue->SetDevName(TEXT("MouseSensitivityYaw"));
			SettingValue->SetDisplayName(LOCTEXT("MouseSensitivityYaw_Name", "X-Axis Sensitivity"));
			SettingValue->SetDescriptionRichText(LOCTEXT("MouseSensitivityYaw_Description", "마우스 움직임에 따른 화면 전환(좌우 회전) 속도를 조절합니다. 값을 높일수록 더 적은 마우스 움직임으로 빠르게 화면을 돌릴 수 있습니다."));
			
			SettingValue->SetDynamicGetter(GET_PLAYERPROFILE_SETTINGS_FUNCTION_PATH(GetMouseSensitivityX));
			SettingValue->SetDynamicSetter(GET_PLAYERPROFILE_SETTINGS_FUNCTION_PATH(SetMouseSensitivityX));
			SettingValue->SetDefaultValue(GetDefault<UFHPlayerProfileSettings>()->GetMouseSensitivityX());
			SettingValue->SetDisplayFormat(UGameSettingValueScalarDynamic::RawTwoDecimals);
			SettingValue->SetSourceRangeAndStep(TRange<double>(0, 10), 0.01);
			SettingValue->SetMinimumLimit(0.01);

			Sensitivity->AddSetting(SettingValue);
		}

		/** Mouse Sensitivity Pitch */
		{
			UGameSettingValueScalarDynamic* SettingValue = NewObject<UGameSettingValueScalarDynamic>();
			SettingValue->SetDevName(TEXT("MouseSensitivityPitch"));
			SettingValue->SetDisplayName(LOCTEXT("MouseSensitivityPitch_Name", "Y-Axis Sensitivity"));
			SettingValue->SetDescriptionRichText(LOCTEXT("MouseSensitivityPitch_Description", "마우스 움직임에 따른 화면 전환(상하 회전) 속도를 조절합니다. 값을 높일수록 더 적은 마우스 움직임으로 빠르게 화면을 돌릴 수 있습니다."));

			SettingValue->SetDynamicGetter(GET_PLAYERPROFILE_SETTINGS_FUNCTION_PATH(GetMouseSensitivityY));
			SettingValue->SetDynamicSetter(GET_PLAYERPROFILE_SETTINGS_FUNCTION_PATH(SetMouseSensitivityY));
			SettingValue->SetDefaultValue(GetDefault<UFHPlayerProfileSettings>()->GetMouseSensitivityY());
			SettingValue->SetDisplayFormat(UGameSettingValueScalarDynamic::RawTwoDecimals);
			SettingValue->SetSourceRangeAndStep(TRange<double>(0, 10), 0.01);
			SettingValue->SetMinimumLimit(0.01);

			Sensitivity->AddSetting(SettingValue);
		}
	}

	/** Key Binding */
	{

	}


}

#undef LOCTEXT_NAMESPACE
