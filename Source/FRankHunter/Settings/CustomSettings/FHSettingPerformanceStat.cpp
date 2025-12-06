// Copyright F Rank Hunter. All Rights Reserved.


#include "Settings/CustomSettings/FHSettingPerformanceStat.h"

#define LOCTEXT_NAMESPACE "FRankHunter"

UFHSettingPerformanceStat::UFHSettingPerformanceStat()
{
}

void UFHSettingPerformanceStat::SetStat(EFHDisplayablePerformanceStat InStat)
{
    StatToDisplay = InStat;
    SetDevName(FName(*FString::Printf(TEXT("PerformanceStat_%d"), (int32)StatToDisplay)));
}

void UFHSettingPerformanceStat::StoreInitial()
{
    const UFHDeviceSettings* Settings = UFHDeviceSettings::Get();
    InitialMode = Settings->GetPerfStatDisplayState(StatToDisplay);
}

void UFHSettingPerformanceStat::ResetToDefault()
{
    UFHDeviceSettings* Settings = UFHDeviceSettings::Get();
    Settings->SetPerfStatDisplayState(StatToDisplay, EFHStatDisplayMode::Hidden);
    NotifySettingChanged(EGameSettingChangeReason::ResetToDefault);
}

void UFHSettingPerformanceStat::RestoreToInitial()
{
    UFHDeviceSettings* Settings = UFHDeviceSettings::Get();
    Settings->SetPerfStatDisplayState(StatToDisplay, InitialMode);
    NotifySettingChanged(EGameSettingChangeReason::RestoreToInitial);
}

void UFHSettingPerformanceStat::SetDiscreteOptionByIndex(int32 Index)
{
    if (DisplayModes.IsValidIndex(Index))
    {
        const EFHStatDisplayMode DisplayMode = DisplayModes[Index];

        UFHDeviceSettings* Settings = UFHDeviceSettings::Get();
        Settings->SetPerfStatDisplayState(StatToDisplay, DisplayMode);
    }

    NotifySettingChanged(EGameSettingChangeReason::Change);
}

int32 UFHSettingPerformanceStat::GetDiscreteOptionIndex() const
{
    const UFHDeviceSettings* Settings = UFHDeviceSettings::Get();
    return DisplayModes.Find(Settings->GetPerfStatDisplayState(StatToDisplay));
}

void UFHSettingPerformanceStat::OnInitialized()
{
    Super::OnInitialized();

    AddMode(LOCTEXT("PerfStatDisplayMode_None", "None"), EFHStatDisplayMode::Hidden);
    AddMode(LOCTEXT("PerfStatDisplayMode_Show", "Show"), EFHStatDisplayMode::Show);
}

void UFHSettingPerformanceStat::AddMode(FText&& Label, EFHStatDisplayMode Mode)
{
    Options.Emplace(MoveTemp(Label));
    DisplayModes.Add(Mode);
}

#undef LOCTEXT_NAMESPACE