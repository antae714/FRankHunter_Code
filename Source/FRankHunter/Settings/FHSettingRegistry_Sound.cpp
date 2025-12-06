// Copyright F Rank Hunter. All Rights Reserved.


#include "Settings/FHSettingRegistry.h"
#include "Settings/FHPlayerProfileSettings.h"
#include "Settings/FHDeviceSettings.h"
#include "Player/FHLocalPlayer.h"
#include "GameSettingCollection.h"
#include "GameSettingValueScalarDynamic.h"

#define LOCTEXT_NAMESPACE "FRankHunter"

void UFHSettingRegistry::InitializeSoundSettings()
{
	if (OwningLocalPlayer == nullptr)
	{
		return;
	}

	SoundSettings = NewObject<UGameSettingCollection>();
	SoundSettings->SetDevName(TEXT("SoundSettings"));
	SoundSettings->SetDisplayName(LOCTEXT("SoundSettings_Name", "Sound"));
	SoundSettings->Initialize(OwningLocalPlayer);
	
	/** Volume */
	{
		UGameSettingCollection* VolumeCollection = NewObject<UGameSettingCollection>();
		VolumeCollection->SetDevName(TEXT("VolumeCollection"));
		VolumeCollection->SetDisplayName(LOCTEXT("VolumeCollection_Name", "Volume"));
		SoundSettings->AddSetting(VolumeCollection);

		/** MasterVolume */
		{
			UGameSettingValueScalarDynamic* SettingValue = NewObject<UGameSettingValueScalarDynamic>();
			SettingValue->SetDevName(TEXT("MasterVolume"));
			SettingValue->SetDisplayName(LOCTEXT("MasterVolume_Name", "MasterVolume"));
			SettingValue->SetDescriptionRichText(LOCTEXT("MasterVolume_Description", "게임에서 재생되는 모든 소리(배경 음악, 효과음, 음성 등)의 크기를 한 번에 조절합니다. 이 볼륨을 기준으로 다른 개별 볼륨들이 상대적으로 조절됩니다."));

			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetMasterVolume));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetMasterVolume));

			SettingValue->SetDefaultValue(GetDefault<UFHDeviceSettings>()->GetMasterVolume());
			SettingValue->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);

			VolumeCollection->AddSetting(SettingValue);
		}

		/** MusicVolume */
		{
			UGameSettingValueScalarDynamic* SettingValue = NewObject<UGameSettingValueScalarDynamic>();
			SettingValue->SetDevName(TEXT("MusicVolume"));
			SettingValue->SetDisplayName(LOCTEXT("MusicVolume_Name", "MusicVolume"));
			SettingValue->SetDescriptionRichText(LOCTEXT("MusicVolume_Description", "게임의 분위기를 조성하는 배경 음악(BGM)의 크기를 조절합니다. 게임에 더 집중하고 싶거나 다른 소리를 더 잘 듣고 싶을 때 음악 볼륨을 낮출 수 있습니다."));
			
			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetMusicVolume));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetMusicVolume));
			
			SettingValue->SetDefaultValue(GetDefault<UFHDeviceSettings>()->GetMusicVolume());
			SettingValue->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);

			VolumeCollection->AddSetting(SettingValue);
		}

		/** SoundFXVolume */
		{
			UGameSettingValueScalarDynamic* SettingValue = NewObject<UGameSettingValueScalarDynamic>();
			SettingValue->SetDevName(TEXT("SoundFXVolume"));
			SettingValue->SetDisplayName(LOCTEXT("SoundFXVolume_Name", "SoundFXVolume"));
			SettingValue->SetDescriptionRichText(LOCTEXT("SoundFXVolume_Description", "캐릭터의 발소리, 무기 소리, 스킬 효과음, 환경음 등 게임 플레이와 직접적으로 관련된 모든 효과음의 크기를 조절합니다. 적의 위치를 파악하거나 전투 상황을 인지하는 데 중요한 역할을 합니다."));
			
			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetSoundFXVolume));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetSoundFXVolume));
			
			SettingValue->SetDefaultValue(GetDefault<UFHDeviceSettings>()->GetSoundFXVolume());
			SettingValue->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);

			VolumeCollection->AddSetting(SettingValue);
		}

		/** VoiceChatVolume */
		{
			UGameSettingValueScalarDynamic* SettingValue = NewObject<UGameSettingValueScalarDynamic>();
			SettingValue->SetDevName(TEXT("VoiceChatVolume"));
			SettingValue->SetDisplayName(LOCTEXT("VoiceChatVolume_Name", "VoiceChatVolume"));
			SettingValue->SetDescriptionRichText(LOCTEXT("VoiceChatVolume_Description", "다른 플레이어와의 음성 채팅(보이스챗) 소리의 크기를 조절합니다. 팀원과의 원활한 소통을 위해 게임 소리와의 균형을 맞추는 데 사용됩니다."));
			
			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetVoiceChatVolume));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetVoiceChatVolume));
			
			SettingValue->SetDefaultValue(GetDefault<UFHDeviceSettings>()->GetVoiceChatVolume());
			SettingValue->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);

			VolumeCollection->AddSetting(SettingValue);
		}

		/** VideoVolume */
		{
			UGameSettingValueScalarDynamic* SettingValue = NewObject<UGameSettingValueScalarDynamic>();
			SettingValue->SetDevName(TEXT("VideoVolume"));
			SettingValue->SetDisplayName(LOCTEXT("VideoVolume_Name", "VideoVolume"));
			SettingValue->SetDescriptionRichText(LOCTEXT("VideoVolume_Description", "TV에서 재생되는 튜토리얼 영상 소리의 크기를 조절합니다."));

			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetVideoVolume));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetVideoVolume));

			SettingValue->SetDefaultValue(GetDefault<UFHDeviceSettings>()->GetVideoVolume());
			SettingValue->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);

			VolumeCollection->AddSetting(SettingValue);
		}
	}

	/** Audio */
	{
		//UGameSettingCollection* AudioCollection = NewObject<UGameSettingCollection>();
		//AudioCollection->SetDevName(TEXT("AudioCollection"));
		//AudioCollection->SetDisplayName(LOCTEXT("AudioCollection_Name", "Audio"));
		//SoundSettings->AddSetting(AudioCollection);

		/** AudioDevice */
		{

		}
		/** Improved AudioMode */
		{

		}
	}
}

#undef LOCTEXT_NAMESPACE