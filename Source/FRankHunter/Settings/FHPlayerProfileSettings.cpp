// Copyright F Rank Hunter. All Rights Reserved.


#include "Settings/FHPlayerProfileSettings.h"
#include "Player/FHLocalPlayer.h"
#include "Internationalization/Culture.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"

static FString PROFILE_SETTINGS_SLOT_NAME = TEXT("FHPlayerProfileSettings");

UFHPlayerProfileSettings::UFHPlayerProfileSettings()
{
    FInternationalization::Get().OnCultureChanged().AddUObject(this, &ThisClass::OnCultureChanged);
}

UFHPlayerProfileSettings* UFHPlayerProfileSettings::LoadOrCreateSettings(const UFHLocalPlayer* LocalPlayer)
{
    UFHPlayerProfileSettings* PlayerProfileSettings = Cast<UFHPlayerProfileSettings>(LoadOrCreateSaveGameForLocalPlayer(ThisClass::StaticClass(), LocalPlayer, PROFILE_SETTINGS_SLOT_NAME));

    PlayerProfileSettings->ApplySettings();

    return PlayerProfileSettings;
}

bool UFHPlayerProfileSettings::AsyncLoadOrCreateSettings(const UFHLocalPlayer* LocalPlayer, FOnSettingsLoadedEvent CallBack)
{
    FOnLocalPlayerSaveGameLoadedNative Lambda = FOnLocalPlayerSaveGameLoadedNative::CreateLambda([CallBack](ULocalPlayerSaveGame* LoadedSaveGame)
        {
            UFHPlayerProfileSettings* LoadedSettings = CastChecked<UFHPlayerProfileSettings>(LoadedSaveGame);

            LoadedSettings->ApplySettings();

            CallBack.ExecuteIfBound(LoadedSettings);
        });

    return ULocalPlayerSaveGame::AsyncLoadOrCreateSaveGameForLocalPlayer(ThisClass::StaticClass(), LocalPlayer, PROFILE_SETTINGS_SLOT_NAME, Lambda);
}

int32 UFHPlayerProfileSettings::GetLatestDataVersion() const
{
    // 0 = before subclassing ULocalPlayerSaveGame
    // 1 = first proper version
    return 1;
}

void UFHPlayerProfileSettings::SaveSettings()
{
    AsyncSaveGameToSlotForLocalPlayer();

    if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwningPlayer))
    {
        if (UEnhancedInputUserSettings* InputSettings = System->GetUserSettings())
        {
            InputSettings->AsyncSaveSettings();
        }
    }
}

void UFHPlayerProfileSettings::ApplySettings()
{
    // ApplySubtitleOptions();
    // ApplyBackgroundAudioSettings();
    ApplyCultureSettings();

    if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwningPlayer))
    {
        if (UEnhancedInputUserSettings* InputSettings = System->GetUserSettings())
        {
            InputSettings->ApplySettings();
        }
    }
}

void UFHPlayerProfileSettings::ApplyInputSensitivity()
{
    // TODO


}

bool UFHPlayerProfileSettings::IsUsingDefaultCulture() const
{
    FString Culture;
    GConfig->GetString(TEXT("Internationalization"), TEXT("Culture"), Culture, GGameUserSettingsIni);

    return Culture.IsEmpty();
}

void UFHPlayerProfileSettings::ApplyCultureSettings()
{
    if (bResetToDefaultCulture)
    {
        const FCulturePtr SystemDefaultCulture = FInternationalization::Get().GetDefaultCulture();
        check(SystemDefaultCulture.IsValid());

        const FString CultureToApply = SystemDefaultCulture->GetName();
        if (FInternationalization::Get().SetCurrentCulture(CultureToApply))
        {
            GConfig->RemoveKey(TEXT("Internationalization"), TEXT("Culture"), GGameUserSettingsIni);
            GConfig->Flush(false, GGameUserSettingsIni);
        }

        bResetToDefaultCulture = false;
    }
    else if (!PendingCulture.IsEmpty())
    {
        const FString CultureToApply = PendingCulture;
        if (FInternationalization::Get().SetCurrentCulture(CultureToApply))
        {
            GConfig->SetString(TEXT("Internationalization"), TEXT("Culture"), *CultureToApply, GGameUserSettingsIni);
            GConfig->Flush(false, GGameUserSettingsIni);
        }

        ClearPendingCulture();
    }
}

