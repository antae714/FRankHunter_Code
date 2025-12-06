// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "GameFramework/SaveGame.h"
#include "FHPlayerProfileSettings.generated.h"

class UFHLocalPlayer;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHPlayerProfileSettings : public ULocalPlayerSaveGame
{
	GENERATED_BODY()

public:
	UFHPlayerProfileSettings();

	DECLARE_EVENT_OneParam(UFHPlayerProfileSettings, FOnSettingChangedEvent, UFHPlayerProfileSettings*);
	FOnSettingChangedEvent OnSettingChanged;

	static UFHPlayerProfileSettings* LoadOrCreateSettings(const UFHLocalPlayer* LocalPlayer);

	DECLARE_DELEGATE_OneParam(FOnSettingsLoadedEvent, UFHPlayerProfileSettings*);
	static bool AsyncLoadOrCreateSettings(const UFHLocalPlayer* LocalPlayer, FOnSettingsLoadedEvent CallBack);

	virtual int32 GetLatestDataVersion() const override;

	bool IsDirty() const { return bIsDirty; }
	void SetDirty(bool IsDirty = true) { bIsDirty = IsDirty; }

	void SaveSettings();

	void ApplySettings();

#pragma region MouseSensitivity
public:
	UFUNCTION()
	double GetMouseSensitivityX() const { return MouseSensitivityX; }
	UFUNCTION()
	void SetMouseSensitivityX(double NewValue) 
	{ 
		ChangeValue(MouseSensitivityX, NewValue); 
		ApplyInputSensitivity(); 
	}

	UFUNCTION()
	double GetMouseSensitivityY() const { return MouseSensitivityY; }
	UFUNCTION()
	void SetMouseSensitivityY(double NewValue) 
	{
		ChangeValue(MouseSensitivityY, NewValue); 
		ApplyInputSensitivity(); 
	}

private:
	UPROPERTY()
	double MouseSensitivityX = 1.0;

	UPROPERTY()
	double MouseSensitivityY = 1.0;

	void ApplyInputSensitivity();

#pragma endregion
#pragma region Subtitles
public:
	UFUNCTION()
	bool GetSubtitlesEnabled() const { return bEnableSubtitles; }
	UFUNCTION()
	void SetSubtitlesEnabled(bool Value) { ChangeValue(bEnableSubtitles, Value); }

private:
	UPROPERTY()
	bool bEnableSubtitles = true;

#pragma endregion
#pragma region Language
public:
	const FString& GetPendingCulture() const { return PendingCulture; }

	void SetPendingCulture(const FString& NewCulture)
	{
		PendingCulture = NewCulture;
		bResetToDefaultCulture = false;
		bIsDirty = true;
	}

	void ClearPendingCulture() { PendingCulture.Reset(); }

	void OnCultureChanged() 
	{ 
		ClearPendingCulture(); 
		bResetToDefaultCulture = false; 
	}

	void ResetToDefaultCulture()
	{
		ClearPendingCulture();
		bResetToDefaultCulture = true;
		bIsDirty = true;
	}

	bool ShouldResetToDefaultCulture() const { return bResetToDefaultCulture; }

	bool IsUsingDefaultCulture() const;

	void ApplyCultureSettings();

private:
	UPROPERTY(Transient)
	FString PendingCulture;

	bool bResetToDefaultCulture = false;
#pragma endregion


private:
	bool bIsDirty = false;

	template<typename T>
	bool ChangeValue(T& CurrentValue, const T& NewValue)
	{
		if (CurrentValue != NewValue)
		{
			CurrentValue = NewValue;
			bIsDirty = true;
			OnSettingChanged.Broadcast(this);

			return true;
		}

		return false;
	}
};
