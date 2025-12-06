// Copyright F Rank Hunter. All Rights Reserved.


#include "Settings/CustomSettings/FHSettingLanguage.h"
#include "Internationalization/Culture.h"
#include "Internationalization/TextLocalizationManager.h"
#include "Settings/FHPlayerProfileSettings.h"
#include "Player/FHLocalPlayer.h"

#define LOCTEXT_NAMESPACE "FRankHunter"

static const int32 DefaultSettingLanguageIndex = 0;

UFHSettingLanguage::UFHSettingLanguage()
{
}

void UFHSettingLanguage::ResetToDefault()
{
    SetDiscreteOptionByIndex(DefaultSettingLanguageIndex);
}

void UFHSettingLanguage::RestoreToInitial()
{
    if (UFHPlayerProfileSettings* ProfileSettings = CastChecked<UFHLocalPlayer>(LocalPlayer)->GetPlayerProfileSettings())
    {
        ProfileSettings->ClearPendingCulture();
        NotifySettingChanged(EGameSettingChangeReason::RestoreToInitial);
    }
}

void UFHSettingLanguage::SetDiscreteOptionByIndex(int32 Index)
{
    if (UFHPlayerProfileSettings* ProfileSettings = CastChecked<UFHLocalPlayer>(LocalPlayer)->GetPlayerProfileSettings())
    {
        if (Index == DefaultSettingLanguageIndex)
        {
            ProfileSettings->ResetToDefaultCulture();
        }
        else if (AvailableCultureNames.IsValidIndex(Index))
        {
            ProfileSettings->SetPendingCulture(AvailableCultureNames[Index]);
        }

        NotifySettingChanged(EGameSettingChangeReason::Change);
    }
}

int32 UFHSettingLanguage::GetDiscreteOptionIndex() const
{
    if (UFHPlayerProfileSettings* ProfileSettings = CastChecked<UFHLocalPlayer>(LocalPlayer)->GetPlayerProfileSettings())
    {
        if (ProfileSettings->ShouldResetToDefaultCulture())
        {
            return DefaultSettingLanguageIndex;
        }

        FString PendingCulture = ProfileSettings->GetPendingCulture();
        if (PendingCulture.IsEmpty())
        {
            if (ProfileSettings->IsUsingDefaultCulture())
            {
                return DefaultSettingLanguageIndex;
            }

            PendingCulture = FInternationalization::Get().GetCurrentCulture()->GetName();
        }

        const int32 MatchIndex = AvailableCultureNames.IndexOfByKey(PendingCulture);
        if (MatchIndex != INDEX_NONE)
        {
            return MatchIndex;
        }

        /** Try to find a prioritized match(eg, allowing "en-US" to show as "en" in the UI) */
        const TArray<FString> PrioritizedPendingCultures = FInternationalization::Get().GetPrioritizedCultureNames(PendingCulture);
        for (int32 Index = 0; Index < AvailableCultureNames.Num(); ++Index)
        {
            if (PrioritizedPendingCultures.Contains(AvailableCultureNames[Index]))
            {
                return Index;
            }
        }
    }

    return 0;
}

TArray<FText> UFHSettingLanguage::GetDiscreteOptions() const
{
    TArray<FText> Options;

    for (const FString& CultureName : AvailableCultureNames)
    {
        if (CultureName == TEXT(""))
        {
            const FCulturePtr DefaultCulture = FInternationalization::Get().GetDefaultCulture();
            if (ensure(DefaultCulture))
            {
                const FString& DefaultCultureDisplayName = DefaultCulture->GetDisplayName();
                FText LocalizedDefault = FText::Format(LOCTEXT("DefaultLanguage", "Default ({0})"), FText::FromString(DefaultCultureDisplayName));

                Options.Add(MoveTemp(LocalizedDefault));
            }
        }
        else
        {
            FCulturePtr Culture = FInternationalization::Get().GetCulture(CultureName);
            if (ensureMsgf(Culture, TEXT("Unable to find Culture '%s'!"), *CultureName))
            {
                const FString CultureDisplayName = Culture->GetDisplayName();
                const FString CultureNativeName = Culture->GetNativeName();

                FString Entry = (!CultureNativeName.Equals(CultureDisplayName, ESearchCase::CaseSensitive)) ?
                    FString::Printf(TEXT("%s (%s)"), *CultureNativeName, *CultureDisplayName) : CultureNativeName;

                Options.Add(FText::FromString(Entry));
            }
        }
    }

    return Options;
}

void UFHSettingLanguage::OnInitialized()
{
    Super::OnInitialized();

    const TArray<FString> AllCultureNames = FTextLocalizationManager::Get().GetLocalizedCultureNames(ELocalizationLoadFlags::Game);
    for (const FString& CultureName : AllCultureNames)
    {
        if (FInternationalization::Get().IsCultureAllowed(CultureName))
        {
            AvailableCultureNames.Add(CultureName);
        }
    }

    AvailableCultureNames.Insert(TEXT(""), DefaultSettingLanguageIndex);
}

void UFHSettingLanguage::OnApply()
{
    if (UFHPlayerProfileSettings* ProfileSettings = CastChecked<UFHLocalPlayer>(LocalPlayer)->GetPlayerProfileSettings())
    {
        ProfileSettings->ApplyCultureSettings();

        // TODO : 게임 재시작이 필요하다는 UI 호출 필요.
        //

    }
}

#undef LOCTEXT_NAMESPACE
