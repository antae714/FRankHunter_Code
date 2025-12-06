// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingValueDiscrete.h"
#include "FHSettingVideoQuality.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHSettingVideoQuality : public UGameSettingValueDiscrete
{
	GENERATED_BODY()
	
public:
	UFHSettingVideoQuality();

	virtual void StoreInitial() override { }
	virtual void ResetToDefault() override { }
	virtual void RestoreToInitial() override { }

	virtual void SetDiscreteOptionByIndex(int32 Index) override;
	virtual int32 GetDiscreteOptionIndex() const override;
	virtual TArray<FText> GetDiscreteOptions() const override;

protected:
	virtual void OnInitialized() override;

	TArray<FText> Options;
	TArray<FText> OptionsWithCustom;
};
