// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingValueDiscrete.h"
#include "FHSettingLanguage.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHSettingLanguage : public UGameSettingValueDiscrete
{
	GENERATED_BODY()
	
public:
	UFHSettingLanguage();

	virtual void StoreInitial() override { }
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	virtual void SetDiscreteOptionByIndex(int32 Index) override;
	virtual int32 GetDiscreteOptionIndex() const override;
	virtual TArray<FText> GetDiscreteOptions() const override;

protected:
	virtual void OnInitialized() override;
	virtual void OnApply() override;

	TArray<FString> AvailableCultureNames;
};
