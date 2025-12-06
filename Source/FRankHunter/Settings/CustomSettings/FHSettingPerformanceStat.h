// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingValueDiscrete.h"
#include "Settings/FHDeviceSettings.h"
#include "FHSettingPerformanceStat.generated.h"

/**
 *
 */
UCLASS()
class FRANKHUNTER_API UFHSettingPerformanceStat : public UGameSettingValueDiscrete
{
	GENERATED_BODY()

public:
	UFHSettingPerformanceStat();

	void SetStat(EFHDisplayablePerformanceStat InStat);

	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	virtual void SetDiscreteOptionByIndex(int32 Index) override;
	virtual int32 GetDiscreteOptionIndex() const override;
	virtual TArray<FText> GetDiscreteOptions() const override { return Options; }

protected:
	virtual void OnInitialized() override;

	void AddMode(FText&& Label, EFHStatDisplayMode Mode);

	TArray<FText> Options;
	TArray<EFHStatDisplayMode> DisplayModes;

	EFHDisplayablePerformanceStat StatToDisplay;
	EFHStatDisplayMode InitialMode;
};