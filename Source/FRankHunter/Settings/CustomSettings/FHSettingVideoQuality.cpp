// Copyright F Rank Hunter. All Rights Reserved.


#include "Settings/CustomSettings/FHSettingVideoQuality.h"
#include "Settings/FHDeviceSettings.h"

#define LOCTEXT_NAMESPACE "FRankHunter"

UFHSettingVideoQuality::UFHSettingVideoQuality()
{
}

void UFHSettingVideoQuality::SetDiscreteOptionByIndex(int32 Index)
{
	UGameUserSettings* UserSettings = CastChecked<UGameUserSettings>(GEngine->GetGameUserSettings());
	if (Index == OptionsWithCustom.Num() - 1)
	{
		//
	}
	else
	{
		UserSettings->SetOverallScalabilityLevel(Index);
	}

	NotifySettingChanged(EGameSettingChangeReason::Change);
}

int32 UFHSettingVideoQuality::GetDiscreteOptionIndex() const
{
	const UGameUserSettings* UserSettings = CastChecked<UGameUserSettings>(GEngine->GetGameUserSettings());
	const int32 OverallQualityLevel = UserSettings->GetOverallScalabilityLevel();
	if (OverallQualityLevel == INDEX_NONE)
	{
		return OptionsWithCustom.Num() - 1;
	}

	return OverallQualityLevel;
}

TArray<FText> UFHSettingVideoQuality::GetDiscreteOptions() const
{
	const UGameUserSettings* UserSettings = CastChecked<UGameUserSettings>(GEngine->GetGameUserSettings());
	if (UserSettings->GetOverallScalabilityLevel() == INDEX_NONE)
	{
		return OptionsWithCustom;
	}

	return Options;
}

void UFHSettingVideoQuality::OnInitialized()
{
	Super::OnInitialized();

	UFHDeviceSettings* UserSettings = UFHDeviceSettings::Get();
	const int32 MaxQualityLevel = UserSettings->GetMaxSupportedOverallQualityLevel();

	auto AddOption = [&](int32 Index, FText&& Value)
		{
			if ((MaxQualityLevel < 0) || (MaxQualityLevel >= Index))
			{
				Options.Add(Value);
			}
		};

	AddOption(0, LOCTEXT("VideoQualityOverall_Low", "Low"));
	AddOption(1, LOCTEXT("VideoQualityOverall_Medium", "Medium"));
	AddOption(2, LOCTEXT("VideoQualityOverall_High", "High"));
	AddOption(3, LOCTEXT("VideoQualityOverall_Epic", "Epic"));

	OptionsWithCustom = Options;
	OptionsWithCustom.Add(LOCTEXT("VideoQualityOverall_Custom", "Custom"));
}

#undef LOCTEXT_NAMESPACE