// Copyright F Rank Hunter. All Rights Reserved.


#include "Settings/FHDeviceSettings.h"
#include "SoundControlBus.h"
#include "SoundControlBusMix.h"
#include "AudioModulationStatics.h"
#include "Sound/FHSoundSettings.h"
#include "Engine/World.h"
#include "Scalability.h" 
#include "DeviceProfiles/DeviceProfileManager.h" 

#if WITH_EDITOR
static TAutoConsoleVariable<bool> CVarApplyFrameRateSettingsInPIE(TEXT("FRankHunter.Settings.ApplyFrameRateSettingsInPIE"),
	false,
	TEXT("If true, apply frame rate settings (e.g., menu frame rate) when playing in editor."),
	ECVF_Default);

static TAutoConsoleVariable<bool> CVarApplyFrontEndPerformanceOptionsInPIE(TEXT("FRankHunter.Settings.ApplyFrontEndPerformanceOptionsInPIE"),
	false,
	TEXT("If true, apply front-end performance options (e.g., menu frame rate) when playing in editor."),
	ECVF_Default);

static TAutoConsoleVariable<bool> CVarApplyDeviceProfilesInPIE(TEXT("FRankHunter.Settings.ApplyDeviceProfilesInPIE"),
	false,
	TEXT("Should we apply experience/platform emulated device profiles in PIE?"),
	ECVF_Default);
#endif

static TAutoConsoleVariable<int32> CVarDeviceProfileDrivenTargetFps(TEXT("FRankHunter.DeviceProfile.Console.TargetFPS"),
	-1,
	TEXT("Target FPS when being driven by device profile"),
	ECVF_Default | ECVF_Preview);

static TAutoConsoleVariable<int32> CVarDeviceProfileDrivenFrameSyncType(TEXT("FRankHunter.DeviceProfile.Console.FrameSyncType"),
	-1,
	TEXT("Sync type when being driven by device profile. Corresponds to r.GTSyncType"),
	ECVF_Default | ECVF_Preview);

//==================================================================================================
FFHScalabilitySnapshot::FFHScalabilitySnapshot()
{
	Qualities.ResolutionQuality = -1.0f;
	Qualities.ViewDistanceQuality = -1;
	Qualities.AntiAliasingQuality = -1;
	Qualities.ShadowQuality = -1;
	Qualities.GlobalIlluminationQuality = -1;
	Qualities.ReflectionQuality = -1;
	Qualities.PostProcessQuality = -1;
	Qualities.TextureQuality = -1;
	Qualities.EffectsQuality = -1;
	Qualities.FoliageQuality = -1;
	Qualities.ShadingQuality = -1;
}

namespace FHSettingsHelpers
{
	int32 GetHighestLevelOfAnyScalabilityChannel(const Scalability::FQualityLevels& ScalabilityQuality)
	{
		int32 MaxScalability = ScalabilityQuality.ViewDistanceQuality;
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.AntiAliasingQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.ShadowQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.GlobalIlluminationQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.ReflectionQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.PostProcessQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.TextureQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.EffectsQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.FoliageQuality);
		MaxScalability = FMath::Max(MaxScalability, ScalabilityQuality.ShadingQuality);

		return (MaxScalability >= 0) ? MaxScalability : -1;
	}

	void FillScalabilitySettingsFromDeviceProfile(FFHScalabilitySnapshot& Mode, const FString& Suffix = FString())
	{
		Mode = FFHScalabilitySnapshot();

		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.ResolutionQuality%s"), *Suffix), Mode.Qualities.ResolutionQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.ViewDistanceQuality%s"), *Suffix), Mode.Qualities.ViewDistanceQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.AntiAliasingQuality%s"), *Suffix), Mode.Qualities.AntiAliasingQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.ShadowQuality%s"), *Suffix), Mode.Qualities.ShadowQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.GlobalIlluminationQuality%s"), *Suffix), Mode.Qualities.GlobalIlluminationQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.ReflectionQuality%s"), *Suffix), Mode.Qualities.ReflectionQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.PostProcessQuality%s"), *Suffix), Mode.Qualities.PostProcessQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.TextureQuality%s"), *Suffix), Mode.Qualities.TextureQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.EffectsQuality%s"), *Suffix), Mode.Qualities.EffectsQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.FoliageQuality%s"), *Suffix), Mode.Qualities.FoliageQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.ShadingQuality%s"), *Suffix), Mode.Qualities.ShadingQuality);
	}
}

//==================================================================================================

UFHDeviceSettings::UFHDeviceSettings()
{
	if (!HasAnyFlags(RF_ClassDefaultObject) && FSlateApplication::IsInitialized())
	{
		OnApplicationActivationStateChangedHandle = FSlateApplication::Get().OnApplicationActivationStateChanged().AddUObject(this, &ThisClass::OnAppActivationStateChanged);
	}

	SetToDefaults();
}

UFHDeviceSettings* UFHDeviceSettings::Get()
{
	return GEngine ? CastChecked<UFHDeviceSettings>(GEngine->GetGameUserSettings()) : nullptr;
}

void UFHDeviceSettings::SetToDefaults()
{
	Super::SetToDefaults();

	bSoundControlBusMixLoaded = false;

	//
}

void UFHDeviceSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);

	DesiredUserChosenDeviceProfileSuffix = UserChosenDeviceProfileSuffix;

	FHSettingsHelpers::FillScalabilitySettingsFromDeviceProfile(DeviceDefaultScalabilitySettings, DesiredUserChosenDeviceProfileSuffix);

	bDeviceProfileScalabilityInitialized = true;
}

void UFHDeviceSettings::ResetToCurrentSettings()
{
	Super::ResetToCurrentSettings();

	UserChosenDeviceProfileSuffix = DesiredUserChosenDeviceProfileSuffix;

	//
}

void UFHDeviceSettings::BeginDestroy()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().OnApplicationActivationStateChanged().Remove(OnApplicationActivationStateChangedHandle);
	}

	Super::BeginDestroy();
}

float UFHDeviceSettings::GetEffectiveFrameRateLimit()
{
#if WITH_EDITOR
	if (GIsEditor && !CVarApplyFrameRateSettingsInPIE.GetValueOnGameThread())
	{
		return Super::GetEffectiveFrameRateLimit();
	}
#endif

	float EffectiveFrameRateLimit = Super::GetEffectiveFrameRateLimit();
	if (bInFrontEndForPerformancePurposes)
	{
		EffectiveFrameRateLimit = CombineFrameRateLimits(EffectiveFrameRateLimit, FrameRateLimit_InMenu);
	}

	if (FSlateApplication::IsInitialized() && !FSlateApplication::Get().IsActive())
	{
		EffectiveFrameRateLimit = CombineFrameRateLimits(EffectiveFrameRateLimit, FrameRateLimit_WhenBackgrounded);
	}

	return EffectiveFrameRateLimit;
}

void UFHDeviceSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();

	ApplyAllSoundSettings();

}

void UFHDeviceSettings::OnExperienceLoaded()
{
	ApplyNonResolutionSettings();
}

void UFHDeviceSettings::OnHotfixDeviceProfileApplied()
{
	ApplyNonResolutionSettings();
}

FString UFHDeviceSettings::GetDesiredDeviceProfileQualitySuffix() const
{
	return DesiredUserChosenDeviceProfileSuffix;
}

void UFHDeviceSettings::SetDesiredDeviceProfileQualitySuffix(const FString& InDesiredSuffix)
{
	DesiredUserChosenDeviceProfileSuffix = InDesiredSuffix;
}

const FFHScalabilitySnapshot& UFHDeviceSettings::GetDeviceProfileScalability() const
{
	if (!bDeviceProfileScalabilityInitialized)
	{
		FHSettingsHelpers::FillScalabilitySettingsFromDeviceProfile(const_cast<UFHDeviceSettings*>(this)->DeviceDefaultScalabilitySettings, DesiredUserChosenDeviceProfileSuffix);
		const_cast<UFHDeviceSettings*>(this)->bDeviceProfileScalabilityInitialized = true;
	}

	return DeviceDefaultScalabilitySettings;
}

EFHStatDisplayMode UFHDeviceSettings::GetPerfStatDisplayState(EFHDisplayablePerformanceStat Stat) const
{
	if (const EFHStatDisplayMode* pMode = DisplayStatList.Find(Stat))
	{
		return *pMode;
	}
	else
	{
		return EFHStatDisplayMode::Hidden;
	}
}

void UFHDeviceSettings::SetPerfStatDisplayState(EFHDisplayablePerformanceStat Stat, EFHStatDisplayMode DisplayMode)
{
	if (DisplayMode == EFHStatDisplayMode::Hidden)
	{
		DisplayStatList.Remove(Stat);
	}
	else
	{
		DisplayStatList.FindOrAdd(Stat) = DisplayMode;
	}

	PerfStatSettingsChangedEvent.Broadcast();
}

void UFHDeviceSettings::SetShouldUseFrontendPerformanceSettings(bool bInFrontEnd)
{
	bInFrontEndForPerformancePurposes = bInFrontEnd;
	UpdateEffectiveFrameRateLimit();
}

void UFHDeviceSettings::SetFrameRateLimit_InMenu(float NewLimitFPS)
{
	FrameRateLimit_InMenu = NewLimitFPS;
	UpdateEffectiveFrameRateLimit();
}

void UFHDeviceSettings::SetFrameRateLimit_WhenBackgrounded(float NewLimitFPS)
{
	FrameRateLimit_WhenBackgrounded = NewLimitFPS;
	UpdateEffectiveFrameRateLimit();
}

void UFHDeviceSettings::SetFrameRateLimit_Always(float NewLimitFPS)
{
	SetFrameRateLimit(NewLimitFPS);
	UpdateEffectiveFrameRateLimit();
}

bool UFHDeviceSettings::ShouldUseFrontendPerformanceSettings() const
{
#if WITH_EDITOR
	if (GIsEditor && !CVarApplyFrontEndPerformanceOptionsInPIE.GetValueOnGameThread())
	{
		return false;
	}
#endif

	return bInFrontEndForPerformancePurposes;
}

float UFHDeviceSettings::CombineFrameRateLimits(float Limit1, float Limit2)
{
	/** Limit <= 0.0f 이면 무제한 */
	if (Limit1 <= 0.0f)
	{
		return Limit2;
	}
	else if (Limit2 <= 0.0f)
	{
		return Limit1;
	}

	return FMath::Min(Limit1, Limit2);
}

void UFHDeviceSettings::UpdateEffectiveFrameRateLimit()
{
	SetFrameRateLimitCVar(GetEffectiveFrameRateLimit());
}

int32 UFHDeviceSettings::GetMaxSupportedOverallQualityLevel() const
{
	// FHSettingsHelpers::GetHighestLevelOfAnyScalabilityChannel(DeviceDefaultScalabilitySettings.Qualities);

	return 3;
}

int32 UFHDeviceSettings::GetHighestLevelOfAnyScalabilityChannel() const
{
	return FHSettingsHelpers::GetHighestLevelOfAnyScalabilityChannel(ScalabilityQuality);
}

void UFHDeviceSettings::OverrideQualityLevelsToScalabilityMode(const FFHScalabilitySnapshot& InMode, Scalability::FQualityLevels& InOutLevels)
{
	InOutLevels.ResolutionQuality = (InMode.Qualities.ResolutionQuality >= 0.f) ? InMode.Qualities.ResolutionQuality : InOutLevels.ResolutionQuality;
	InOutLevels.ViewDistanceQuality = (InMode.Qualities.ViewDistanceQuality >= 0) ? InMode.Qualities.ViewDistanceQuality : InOutLevels.ViewDistanceQuality;
	InOutLevels.AntiAliasingQuality = (InMode.Qualities.AntiAliasingQuality >= 0) ? InMode.Qualities.AntiAliasingQuality : InOutLevels.AntiAliasingQuality;
	InOutLevels.ShadowQuality = (InMode.Qualities.ShadowQuality >= 0) ? InMode.Qualities.ShadowQuality : InOutLevels.ShadowQuality;
	InOutLevels.GlobalIlluminationQuality = (InMode.Qualities.GlobalIlluminationQuality >= 0) ? InMode.Qualities.GlobalIlluminationQuality : InOutLevels.GlobalIlluminationQuality;
	InOutLevels.ReflectionQuality = (InMode.Qualities.ReflectionQuality >= 0) ? InMode.Qualities.ReflectionQuality : InOutLevels.ReflectionQuality;
	InOutLevels.PostProcessQuality = (InMode.Qualities.PostProcessQuality >= 0) ? InMode.Qualities.PostProcessQuality : InOutLevels.PostProcessQuality;
	InOutLevels.TextureQuality = (InMode.Qualities.TextureQuality >= 0) ? InMode.Qualities.TextureQuality : InOutLevels.TextureQuality;
	InOutLevels.EffectsQuality = (InMode.Qualities.EffectsQuality >= 0) ? InMode.Qualities.EffectsQuality : InOutLevels.EffectsQuality;
	InOutLevels.FoliageQuality = (InMode.Qualities.FoliageQuality >= 0) ? InMode.Qualities.FoliageQuality : InOutLevels.FoliageQuality;
	InOutLevels.ShadingQuality = (InMode.Qualities.ShadingQuality >= 0) ? InMode.Qualities.ShadingQuality : InOutLevels.ShadingQuality;
}

void UFHDeviceSettings::ClampQualityLevelsToDeviceProfile(const Scalability::FQualityLevels& ClampLevels, Scalability::FQualityLevels& InOutLevels)
{
	InOutLevels.ResolutionQuality = (ClampLevels.ResolutionQuality >= 0.f) ? FMath::Min(ClampLevels.ResolutionQuality, InOutLevels.ResolutionQuality) : InOutLevels.ResolutionQuality;
	InOutLevels.ViewDistanceQuality = (ClampLevels.ViewDistanceQuality >= 0) ? FMath::Min(ClampLevels.ViewDistanceQuality, InOutLevels.ViewDistanceQuality) : InOutLevels.ViewDistanceQuality;
	InOutLevels.AntiAliasingQuality = (ClampLevels.AntiAliasingQuality >= 0) ? FMath::Min(ClampLevels.AntiAliasingQuality, InOutLevels.AntiAliasingQuality) : InOutLevels.AntiAliasingQuality;
	InOutLevels.ShadowQuality = (ClampLevels.ShadowQuality >= 0) ? FMath::Min(ClampLevels.ShadowQuality, InOutLevels.ShadowQuality) : InOutLevels.ShadowQuality;
	InOutLevels.GlobalIlluminationQuality = (ClampLevels.GlobalIlluminationQuality >= 0) ? FMath::Min(ClampLevels.GlobalIlluminationQuality, InOutLevels.GlobalIlluminationQuality) : InOutLevels.GlobalIlluminationQuality;
	InOutLevels.ReflectionQuality = (ClampLevels.ReflectionQuality >= 0) ? FMath::Min(ClampLevels.ReflectionQuality, InOutLevels.ReflectionQuality) : InOutLevels.ReflectionQuality;
	InOutLevels.PostProcessQuality = (ClampLevels.PostProcessQuality >= 0) ? FMath::Min(ClampLevels.PostProcessQuality, InOutLevels.PostProcessQuality) : InOutLevels.PostProcessQuality;
	InOutLevels.TextureQuality = (ClampLevels.TextureQuality >= 0) ? FMath::Min(ClampLevels.TextureQuality, InOutLevels.TextureQuality) : InOutLevels.TextureQuality;
	InOutLevels.EffectsQuality = (ClampLevels.EffectsQuality >= 0) ? FMath::Min(ClampLevels.EffectsQuality, InOutLevels.EffectsQuality) : InOutLevels.EffectsQuality;
	InOutLevels.FoliageQuality = (ClampLevels.FoliageQuality >= 0) ? FMath::Min(ClampLevels.FoliageQuality, InOutLevels.FoliageQuality) : InOutLevels.FoliageQuality;
	InOutLevels.ShadingQuality = (ClampLevels.ShadingQuality >= 0) ? FMath::Min(ClampLevels.ShadingQuality, InOutLevels.ShadingQuality) : InOutLevels.ShadingQuality;
}

void UFHDeviceSettings::RunAutoBenchmark(bool bSaveImmediately)
{
	RunHardwareBenchmark();

	const int32 OverallQuality = GetOverallScalabilityLevel();

	SetOverallScalabilityLevel(OverallQuality);

	const FFHScalabilitySnapshot& DeviceProfileSnapshot = GetDeviceProfileScalability();
	if (DeviceProfileSnapshot.bHasOverrides)
	{
		OverrideQualityLevelsToScalabilityMode(DeviceProfileSnapshot, ScalabilityQuality);
	}

	ApplyScalabilitySettings();

	if (bSaveImmediately)
	{
		SaveSettings();
	}
}

void UFHDeviceSettings::ApplyScalabilitySettings()
{
	Scalability::SetQualityLevels(ScalabilityQuality);
}

void UFHDeviceSettings::OnAppActivationStateChanged(bool bIsActive)
{
	/** 게임 활성/비활성화시 최적화 */
	UpdateEffectiveFrameRateLimit();
}

void UFHDeviceSettings::SetMasterVolume(float InVolume)
{
	MasterVolume = InVolume;
	ApplyAllSoundSettings();
}

void UFHDeviceSettings::SetMusicVolume(float InVolume)
{
	MusicVolume = InVolume;
	ApplyAllSoundSettings();
}

void UFHDeviceSettings::SetSoundFXVolume(float InVolume)
{
	SoundFXVolume = InVolume;
	ApplyAllSoundSettings();
}

void UFHDeviceSettings::SetVoiceChatVolume(float InVolume)
{
	VoiceChatVolume = InVolume;
	ApplyAllSoundSettings();
}

void UFHDeviceSettings::SetVideoVolume(float InVolume)
{
	VideoVolume = InVolume;
	ApplyAllSoundSettings();
}

void UFHDeviceSettings::LoadUserControlBusAndMix()
{
	if (bSoundControlBusMixLoaded)
	{
		return;
	}

	const UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!World)
	{
		return;
	}

	const UFHSoundSettings* FHSoundSettings = GetDefault<UFHSoundSettings>();
	if (!FHSoundSettings)
	{
		return;
	}

	if (UObject* LoadedObject = FHSoundSettings->UserSettingsControlBusMix.TryLoad())
	{
		if (USoundControlBusMix* LoadedMix = Cast<USoundControlBusMix>(LoadedObject))
		{
			ControlBusMix = LoadedMix;
		}
	}

	if (!ControlBusMix)
	{
		ensureMsgf(false, TEXT("User Settings Control Bus Mix reference missing from Sound Settings."));
		return;
	}

	ControlBusMap.Empty();
	auto LoadBus = [&](const FSoftObjectPath& Path, const FName& Name)
		{
			if (UObject* LoadedObject = Path.TryLoad())
			{
				if (USoundControlBus* Bus = Cast<USoundControlBus>(LoadedObject))
				{
					ControlBusMap.Add(Name, Bus);
					return;
				}
			}

			ensureMsgf(false, TEXT("Control Bus '%s' reference missing or failed to load from Sound Settings."), *Name.ToString());
		};


	LoadBus(FHSoundSettings->MasterVolumeControlBus, TEXT("Master"));
	LoadBus(FHSoundSettings->MusicVolumeControlBus, TEXT("Music"));
	LoadBus(FHSoundSettings->SoundFXVolumeControlBus, TEXT("SoundFX"));
	LoadBus(FHSoundSettings->VideoVolumeControlBus, TEXT("Video"));
	LoadBus(FHSoundSettings->VoiceChatVolumeControlBus, TEXT("VoiceChat"));

	bSoundControlBusMixLoaded = true;
}

void UFHDeviceSettings::ApplyAllSoundSettings()
{
	if (!bSoundControlBusMixLoaded)
	{
		LoadUserControlBusAndMix();
	}

	if (!bSoundControlBusMixLoaded || !ControlBusMix)
	{
		return;
	}

	const UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!World)
	{
		return;
	}

	UAudioModulationStatics::ActivateBusMix(World, ControlBusMix);

	TArray<FSoundControlBusMixStage> MixStages;

	auto AddStage = [&](const FName& Name, float Volume)
		{
			if (TObjectPtr<USoundControlBus>* BusPtr = ControlBusMap.Find(Name))
			{
				if (*BusPtr)
				{
					FSoundControlBusMixStage& NewStage = MixStages.AddDefaulted_GetRef();
					NewStage.Bus = *BusPtr;
					NewStage.Value.TargetValue = Volume;
					NewStage.Value.AttackTime = 0.01f;
					NewStage.Value.ReleaseTime = 0.01f;
				}
			}
		};

	AddStage(TEXT("Master"), MasterVolume);
	AddStage(TEXT("Music"), MusicVolume);
	AddStage(TEXT("SoundFX"), SoundFXVolume);
	AddStage(TEXT("Video"), VideoVolume);
	AddStage(TEXT("VoiceChat"), VoiceChatVolume);

	UAudioModulationStatics::UpdateMix(World, ControlBusMix, MixStages);
}
