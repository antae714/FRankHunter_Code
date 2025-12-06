// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "GameSettingValueDiscrete.h"
#include "FHSettingResolution.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHSettingResolution : public UGameSettingValueDiscrete
{
	GENERATED_BODY()
	
public:
	UFHSettingResolution();

	virtual void StoreInitial() override { }
	virtual void ResetToDefault() override { }
	virtual void RestoreToInitial() override { }

	virtual void SetDiscreteOptionByIndex(int32 Index) override;
	virtual int32 GetDiscreteOptionIndex() const override;
	virtual TArray<FText> GetDiscreteOptions() const override;

protected:
	virtual void OnInitialized() override;
	virtual void OnDependencyChanged() override;

private:
	static void GetStandardWindowResolutions(const FIntPoint& MinResolution, const FIntPoint& MaxResolution, float MinAspectRatio, TArray<FIntPoint>& OutResolutions);

	void SelectAppropriateResolutions();
	bool ShouldAllowFullScreenResolution(const FScreenResolutionRHI& SrcScreenRes, int32 FilterThreshold) const;

private:
	TOptional<EWindowMode::Type> LastWindowMode;

	struct FScreenResolutionEntry
	{
		uint32	Width = 0;
		uint32	Height = 0;
		uint32	RefreshRate = 0;
		FText   OverrideText;

		FIntPoint GetResolution() const { return FIntPoint(Width, Height); }
		FText GetDisplayText() const;
	};

	TArray<TSharedPtr<FScreenResolutionEntry>> ResolutionOptions;

	TArray<TSharedPtr<FScreenResolutionEntry>> ResolutionFullscreen;

	TArray<TSharedPtr<FScreenResolutionEntry>> ResolutionWindowedFullscreen;

	TArray<TSharedPtr<FScreenResolutionEntry>> ResolutionWindowed;
};

