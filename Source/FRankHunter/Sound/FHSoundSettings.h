// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FHSoundSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, Defaultconfig, meta = (DisplayName = "FHSoundSettings"))
class FRANKHUNTER_API UFHSoundSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = MixSettings, meta = (AllowedClasses = "/Script/AudioModulation.SoundControlBusMix"))
	FSoftObjectPath DefaultControlBusMix;

	UPROPERTY(Config, EditAnywhere, Category = MixSettings, meta = (AllowedClasses = "/Script/AudioModulation.SoundControlBusMix"))
	FSoftObjectPath UserSettingsControlBusMix;

	UPROPERTY(Config, EditAnywhere, Category = BusSettings, meta = (AllowedClasses = "/Script/AudioModulation.SoundControlBus"))
	FSoftObjectPath MasterVolumeControlBus;

	UPROPERTY(Config, EditAnywhere, Category = BusSettings, meta = (AllowedClasses = "/Script/AudioModulation.SoundControlBus"))
	FSoftObjectPath MusicVolumeControlBus;

	UPROPERTY(Config, EditAnywhere, Category = BusSettings, meta = (AllowedClasses = "/Script/AudioModulation.SoundControlBus"))
	FSoftObjectPath SoundFXVolumeControlBus;

	UPROPERTY(Config, EditAnywhere, Category = BusSettings, meta = (AllowedClasses = "/Script/AudioModulation.SoundControlBus"))
	FSoftObjectPath VoiceChatVolumeControlBus;

	UPROPERTY(Config, EditAnywhere, Category = BusSettings, meta = (AllowedClasses = "/Script/AudioModulation.SoundControlBus"))
	FSoftObjectPath VideoVolumeControlBus;
};
