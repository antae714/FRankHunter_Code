// Copyright F Rank Hunter. All Rights Reserved.


#include "Player/FHLocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Settings/FHPlayerProfileSettings.h"
#include "Settings/FHDeviceSettings.h"

UFHLocalPlayer::UFHLocalPlayer()
{
}

void UFHLocalPlayer::PostInitProperties()
{
	Super::PostInitProperties();

	/** TODO: Setting Dynamic Value */

}

UFHPlayerProfileSettings* UFHLocalPlayer::GetPlayerProfileSettings() const
{
	if (!PlayerProfileSettings)
	{
		PlayerProfileSettings = UFHPlayerProfileSettings::LoadOrCreateSettings(this);
	}

	return PlayerProfileSettings;
}

UFHDeviceSettings* UFHLocalPlayer::GetDeviceSettings() const
{
	return UFHDeviceSettings::Get();
}

void UFHLocalPlayer::LoadPlayerProfileSettings(bool bForceLoad /* = false */)
{
	FUniqueNetIdRepl CurrentNetId = GetCachedUniqueNetId();
	if (!bForceLoad && PlayerProfileSettings && CurrentNetId == UniqueNetId)
	{
		return;
	}

	// Load -> Callback -> Get Ptr and Id
	UFHPlayerProfileSettings::AsyncLoadOrCreateSettings(this, UFHPlayerProfileSettings::FOnSettingsLoadedEvent::CreateUObject(this, &ThisClass::OnPlayerProfileSettingsLoaded));
}

void UFHLocalPlayer::OnPlayerProfileSettingsLoaded(UFHPlayerProfileSettings* LoadedOrCreatedSettings)
{
	if (ensure(LoadedOrCreatedSettings))
	{
		PlayerProfileSettings = LoadedOrCreatedSettings;

		UniqueNetId = GetCachedUniqueNetId();
	}
}

