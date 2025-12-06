// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "GameFramework/GameUserSettings.h"
#include "FHDeviceSettings.generated.h"

class USoundControlBus;
class USoundControlBusMix;

UENUM(BlueprintType)
enum class EFHStatDisplayMode : uint8
{
	Hidden,

	Show
};

UENUM(BlueprintType)
enum class EFHDisplayablePerformanceStat : uint8
{
	// stat fps (in Hz)
	ClientFPS,

	// server tick rate (in Hz)
	ServerFPS,

	// idle time spent waiting for vsync or frame rate limit (in seconds)
	IdleTime,

	// Stat unit overall (in seconds)
	FrameTime,

	// Stat unit (game thread, in seconds)
	FrameTime_GameThread,

	// Stat unit (render thread, in seconds)
	FrameTime_RenderThread,

	// Stat unit (RHI thread, in seconds)
	FrameTime_RHIThread,

	// Stat unit (inferred GPU time, in seconds)
	FrameTime_GPU,

	// Network ping (in ms)
	Ping,

	// The incoming packet loss percentage (%)
	PacketLoss_Incoming,

	// The outgoing packet loss percentage (%)
	PacketLoss_Outgoing,

	// The number of packets received in the last second
	PacketRate_Incoming,

	// The number of packets sent in the past second
	PacketRate_Outgoing,

	// The avg. size (in bytes) of packets received
	PacketSize_Incoming,

	// The avg. size (in bytes) of packets sent
	PacketSize_Outgoing,

	// New stats should go above here
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EFHDisplayablePerformanceStat, EFHDisplayablePerformanceStat::Count);

/**
 *
 */
USTRUCT()
struct FFHScalabilitySnapshot
{
	GENERATED_BODY()

	FFHScalabilitySnapshot();

	Scalability::FQualityLevels Qualities;

	bool bHasOverrides = false;
};

UCLASS()
class FRANKHUNTER_API UFHDeviceSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UFHDeviceSettings();

	static UFHDeviceSettings* Get();

	virtual void SetToDefaults() override;

	virtual void LoadSettings(bool bForceReload) override;

	virtual void ResetToCurrentSettings() override;

	virtual void BeginDestroy() override;

	virtual float GetEffectiveFrameRateLimit() override;

	virtual void ApplyNonResolutionSettings() override;

public:
	void OnExperienceLoaded();
	void OnHotfixDeviceProfileApplied();

	UFUNCTION()
	FString GetDesiredDeviceProfileQualitySuffix() const;

	UFUNCTION()
	void SetDesiredDeviceProfileQualitySuffix(const FString& InDesiredSuffix);

	const FFHScalabilitySnapshot& GetDeviceProfileScalability() const;

	DECLARE_EVENT(UFHDeviceSettings, FPerfStatSettingsChanged);
	FPerfStatSettingsChanged& OnPerfStatDisplayStateChanged() { return PerfStatSettingsChangedEvent; }

	EFHStatDisplayMode GetPerfStatDisplayState(EFHDisplayablePerformanceStat Stat) const;

	void SetPerfStatDisplayState(EFHDisplayablePerformanceStat Stat, EFHStatDisplayMode DisplayMode);

private:
	UPROPERTY(Transient)
	FString DesiredUserChosenDeviceProfileSuffix;

	UPROPERTY(Transient)
	FString CurrentAppliedDeviceProfileOverrideSuffix;

	UPROPERTY(config)
	FString UserChosenDeviceProfileSuffix;

	UPROPERTY(Config)
	TMap<EFHDisplayablePerformanceStat, EFHStatDisplayMode> DisplayStatList;

	FPerfStatSettingsChanged PerfStatSettingsChangedEvent;

#pragma region Performance
public:
	void SetShouldUseFrontendPerformanceSettings(bool bInFrontEnd);

	UFUNCTION()
	float GetFrameRateLimit_InMenu() const { return FrameRateLimit_InMenu; }
	UFUNCTION()
	void SetFrameRateLimit_InMenu(float NewLimitFPS);

	UFUNCTION()
	float GetFrameRateLimit_WhenBackgrounded() const { return FrameRateLimit_WhenBackgrounded; }
	UFUNCTION()
	void SetFrameRateLimit_WhenBackgrounded(float NewLimitFPS);

	UFUNCTION()
	float GetFrameRateLimit_Always() const { return GetFrameRateLimit(); }
	UFUNCTION()
	void SetFrameRateLimit_Always(float NewLimitFPS);

protected:
	bool bInFrontEndForPerformancePurposes = false;

	UPROPERTY(Config)
	float FrameRateLimit_InMenu = 144.0f;

	UPROPERTY(Config)
	float FrameRateLimit_WhenBackgrounded = 30.0f;

protected:
	bool ShouldUseFrontendPerformanceSettings() const;

	float CombineFrameRateLimits(float Limit1, float Limit2);

	void UpdateEffectiveFrameRateLimit();

private:
	FDelegateHandle OnApplicationActivationStateChangedHandle;

private:
	void OnAppActivationStateChanged(bool bIsActive);

#pragma endregion
#pragma region KeyBindings
public:

private:

#pragma endregion
#pragma region Display
public:
	int32 GetMaxSupportedOverallQualityLevel() const;

private:
	FFHScalabilitySnapshot DeviceDefaultScalabilitySettings;

	bool bDeviceProfileScalabilityInitialized = false;

	int32 GetHighestLevelOfAnyScalabilityChannel() const;

	void OverrideQualityLevelsToScalabilityMode(const FFHScalabilitySnapshot& InMode, Scalability::FQualityLevels& InOutLevels);

	void ClampQualityLevelsToDeviceProfile(const Scalability::FQualityLevels& ClampLevels, Scalability::FQualityLevels& InOutLevels);


#pragma endregion
#pragma region Graphics
public:
	UFUNCTION(BlueprintCallable, Category = Settings)
	bool ShouldRunAutoBenchmarkAtStartup() const { return LastCPUBenchmarkResult == -1 ? true : false; }

	UFUNCTION(BlueprintCallable, Category = Settings)
	void RunAutoBenchmark(bool bSaveImmediately);

	void ApplyScalabilitySettings();

private:

public:
	UFUNCTION()
	float GetDisplayGamma() const { return DisplayGamma; }
	UFUNCTION()
	void SetDisplayGamma(float InGamma) { DisplayGamma = InGamma; ApplyDisplayGamma(); }

private:
	void ApplyDisplayGamma() { if (GEngine) GEngine->DisplayGamma = DisplayGamma; }

	UPROPERTY(Config)
	float DisplayGamma = 2.2f;

#pragma endregion
#pragma region Sound
public:
	UFUNCTION()
	float GetMasterVolume() const { return MasterVolume; }
	UFUNCTION()
	void SetMasterVolume(float InVolume);

	UFUNCTION()
	float GetMusicVolume() const { return MusicVolume; }
	UFUNCTION()
	void SetMusicVolume(float InVolume);

	UFUNCTION()
	float GetSoundFXVolume() const { return SoundFXVolume; }
	UFUNCTION()
	void SetSoundFXVolume(float InVolume);

	UFUNCTION()
	float GetVoiceChatVolume() const { return VoiceChatVolume; }
	UFUNCTION()
	void SetVoiceChatVolume(float InVolume);

	UFUNCTION()
	float GetVideoVolume() const { return VideoVolume; }
	UFUNCTION()
	void SetVideoVolume(float InVolume);

private:
	UPROPERTY(Config)
	float MasterVolume = 1.0f;

	UPROPERTY(Config)
	float MusicVolume = 1.0f;

	UPROPERTY(Config)
	float SoundFXVolume = 1.0f;

	UPROPERTY(Config)
	float VoiceChatVolume = 1.0f;

	UPROPERTY(Config)
	float VideoVolume = 1.0f;

	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<USoundControlBus>> ControlBusMap;

	UPROPERTY(Transient)
	TObjectPtr<USoundControlBusMix> ControlBusMix = nullptr;

	UPROPERTY(Transient)
	bool bSoundControlBusMixLoaded{ false };

private:
	void LoadUserControlBusAndMix();

	void ApplyAllSoundSettings();

#pragma endregion
#pragma region Custom
public:

private:

#pragma endregion


};
