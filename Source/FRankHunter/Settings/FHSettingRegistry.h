// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "DataSource/GameSettingDataSourceDynamic.h"
#include "GameSettingRegistry.h"
#include "Settings/FHDeviceSettings.h"
#include "FHSettingRegistry.generated.h"

class UFHLocalPlayer;

/** GameSettingDataSourceDynamic Helper */
#define GET_PLAYERPROFILE_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)							\
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({								\
		GET_FUNCTION_NAME_STRING_CHECKED(UFHLocalPlayer, GetPlayerProfileSettings),			\
		GET_FUNCTION_NAME_STRING_CHECKED(UFHPlayerProfileSettings, FunctionOrPropertyName)	\
	}))

#define GET_DEVICE_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)							\
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({								\
		GET_FUNCTION_NAME_STRING_CHECKED(UFHLocalPlayer, GetDeviceSettings),				\
		GET_FUNCTION_NAME_STRING_CHECKED(UFHDeviceSettings, FunctionOrPropertyName)			\
	}))


UCLASS()
class FRANKHUNTER_API UFHSettingRegistry : public UGameSettingRegistry
{
	GENERATED_BODY()
	
public:
	UFHSettingRegistry();

	static UFHSettingRegistry* Get(UFHLocalPlayer* InLocalPlayer);

	virtual void SaveChanges() override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UGameSettingCollection> ControlSettings;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UGameSettingCollection> GraphicSettings;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UGameSettingCollection> SoundSettings;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UGameSettingCollection> GameplaySettings;

protected:
	virtual void OnInitialize(ULocalPlayer* InLocalPlayer) override;
	virtual bool IsFinishedInitializing() const override;

	void AddPerformanceStatPage(UGameSettingCollection* PerfStatsOuterCategory);

private:
	void InitializeControlSettings();
	void InitializeGraphicSettings();
	void InitializeSoundSettings();
	void InitializeGameplaySettings();
};
