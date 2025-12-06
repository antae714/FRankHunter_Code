// Copyright F Rank Hunter. All Rights Reserved.


#include "Settings/FHSettingRegistry.h"
#include "Settings/FHPlayerProfileSettings.h"
#include "Settings/FHDeviceSettings.h"
#include "Settings/CustomSettings/FHSettingLanguage.h"
#include "Player/FHLocalPlayer.h"
#include "GameSettingCollection.h"
#include "GameSettingValueDiscreteDynamic.h"

#define LOCTEXT_NAMESPACE "FRankHunter"

void UFHSettingRegistry::InitializeGameplaySettings()
{
	if (OwningLocalPlayer == nullptr)
	{
		return;
	}

	GameplaySettings = NewObject<UGameSettingCollection>();
	GameplaySettings->SetDevName(TEXT("GameplaySettings"));
	GameplaySettings->SetDisplayName(LOCTEXT("GameplaySettings_Name", "Gameplay"));
	GameplaySettings->Initialize(OwningLocalPlayer);

	/** Language */
	{
		UGameSettingCollection* LanguageCollection = NewObject<UGameSettingCollection>();
		LanguageCollection->SetDevName(TEXT("LanguageCollection"));
		LanguageCollection->SetDisplayName(LOCTEXT("LanguageCollection_Name", "Language"));
		GameplaySettings->AddSetting(LanguageCollection);

		/** Language Discrete */
		{
			UFHSettingLanguage* SettingValue = NewObject<UFHSettingLanguage>();
			SettingValue->SetDevName(TEXT("LanguageValue"));
			SettingValue->SetDisplayName(LOCTEXT("LanguageValue_Name", "LanguageValue"));
			SettingValue->SetDescriptionRichText(LOCTEXT("LanguageValue_Description", "게임 내에 표시되는 모든 텍스트의 언어를 설정합니다. 여기에는 메뉴, 인터페이스(UI), 대화 자막, 아이템 설명 등이 모두 포함됩니다.\n<TextStyle.Warning>참고: 변경 사항을 완전히 적용하려면 게임을 재시작해야 합니다.</>"));

			LanguageCollection->AddSetting(SettingValue);
		}
	}

	/** Subtitles */
	{
		//UGameSettingCollection* SubtitlesCollection = NewObject<UGameSettingCollection>();
		//SubtitlesCollection->SetDevName(TEXT("SubtitlesCollection"));
		//SubtitlesCollection->SetDisplayName(LOCTEXT("SubtitlesCollection_Name", "Subtitles"));
		//GameplaySettings->AddSetting(SubtitlesCollection);

		///** Subtitles Discrete */
		//{
		//	UGameSettingValueDiscreteDynamic_Bool* SettingValue = NewObject<UGameSettingValueDiscreteDynamic_Bool>();
		//	SettingValue->SetDevName(TEXT("Subtitles"));
		//	SettingValue->SetDisplayName(LOCTEXT("Subtitles_Name", "Subtitles"));
		//	SettingValue->SetDescriptionRichText(LOCTEXT("Subtitles_Description", "Turns subtitles on/off."));

		//	SettingValue->SetDynamicGetter(GET_PLAYERPROFILE_SETTINGS_FUNCTION_PATH(GetSubtitlesEnabled));
		//	SettingValue->SetDynamicSetter(GET_PLAYERPROFILE_SETTINGS_FUNCTION_PATH(SetSubtitlesEnabled));

		//	SettingValue->SetDefaultValue(GetDefault<UFHPlayerProfileSettings>()->GetSubtitlesEnabled());

		//	GameplaySettings->AddSetting(SettingValue);
		//}

		/** TextSize Discrete */
		{
			//UGameSettingValueDiscreteDynamic_Enum* SettingValue = NewObject<UGameSettingValueDiscreteDynamic_Enum>();
			//SettingValue->SetDevName(TEXT("SubtitlesTextSize"));
			//SettingValue->SetDisplayName(LOCTEXT("SubtitlesTextSize_Name", "SubtitlesTextSize"));
			//SettingValue->SetDescriptionRichText(LOCTEXT("SubtitlesTextSize_Description", "Choose different size options of the the subtitle text."));

			//SettingValue->SetDynamicGetter(GET_PLAYERPROFILE_SETTINGS_FUNCTION_PATH());
			//SettingValue->SetDynamicSetter(GET_PLAYERPROFILE_SETTINGS_FUNCTION_PATH());

			//SettingValue->SetDefaultValue();

			//SettingValue->AddEnumOption( , LOCTEXT("", ""));
			//SettingValue->AddEnumOption( , LOCTEXT("", ""));
			//SettingValue->AddEnumOption( , LOCTEXT("", ""));

			//GameplaySettings->AddSetting(SettingValue);
		}

		/** TextColor Discrete */
		{
			//UGameSettingValueDiscreteDynamic_Enum* SettingValue = NewObject<UGameSettingValueDiscreteDynamic_Enum>();
			//SettingValue->SetDevName(TEXT("SubtitlesTextColor"));
			//SettingValue->SetDisplayName(LOCTEXT("SubtitlesTextColor_Name", "SubtitlesTextColor"));
			//SettingValue->SetDescriptionRichText(LOCTEXT("SubtitlesTextColor_Description", "Choose different color options for the subtitle text."));

			//SettingValue->SetDynamicGetter(GET_PLAYERPROFILE_SETTINGS_FUNCTION_PATH());
			//SettingValue->SetDynamicSetter(GET_PLAYERPROFILE_SETTINGS_FUNCTION_PATH());
			
			//SettingValue->SetDefaultValue();
			
			//SettingValue->AddEnumOption(, LOCTEXT("", ""));
			//SettingValue->AddEnumOption(, LOCTEXT("", ""));
			//SettingValue->AddEnumOption(, LOCTEXT("", ""));

			//GameplaySettings->AddSetting(SettingValue);
		}

		/** TextBorder Discrete */
		{
			//UGameSettingValueDiscreteDynamic_Enum* SettingValue = NewObject<UGameSettingValueDiscreteDynamic_Enum>();
			//SettingValue->SetDevName(TEXT("SubtitlesTextBorder"));
			//SettingValue->SetDisplayName(LOCTEXT("SubtitlesTextBorder_Name", "SubtitlesTextBorder"));
			//SettingValue->SetDescriptionRichText(LOCTEXT("SubtitlesTextBorder_Description", "Choose different border options for the subtitle text."));

			//SettingValue->SetDynamicGetter(GET_PLAYERPROFILE_SETTINGS_FUNCTION_PATH());
			//SettingValue->SetDynamicSetter(GET_PLAYERPROFILE_SETTINGS_FUNCTION_PATH());
			
			//SettingValue->SetDefaultValue();
			
			//SettingValue->AddEnumOption(, LOCTEXT("", ""));
			//SettingValue->AddEnumOption(, LOCTEXT("", ""));
			//SettingValue->AddEnumOption(, LOCTEXT("", ""));

			//GameplaySettings->AddSetting(SettingValue);
		}

		/** TextBorderOpacity Discrete */
		{
			//UGameSettingValueDiscreteDynamic_Enum* SettingValue = NewObject<UGameSettingValueDiscreteDynamic_Enum>();
			//SettingValue->SetDevName(TEXT("SubtitlesTextBorderOpacity"));
			//SettingValue->SetDisplayName(LOCTEXT("SubtitlesTextBorderOpacity_Name", "SubtitlesTextBorderOpacity"));
			//SettingValue->SetDescriptionRichText(LOCTEXT("SubtitlesTextBorderOpacity_Description", "Choose different background opacity options for the subtitles text."));

			//SettingValue->SetDynamicGetter(GET_PLAYERPROFILE_SETTINGS_FUNCTION_PATH());
			//SettingValue->SetDynamicSetter(GET_PLAYERPROFILE_SETTINGS_FUNCTION_PATH());
			
			//SettingValue->SetDefaultValue();
			
			//SettingValue->AddEnumOption(, LOCTEXT("", ""));
			//SettingValue->AddEnumOption(, LOCTEXT("", ""));
			//SettingValue->AddEnumOption(, LOCTEXT("", ""));

			//GameplaySettings->AddSetting(SettingValue);
		}
	}

	/** Performance stats */
	{

	}
}

#undef LOCTEXT_NAMESPACE