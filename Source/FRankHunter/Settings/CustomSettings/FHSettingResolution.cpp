// Copyright F Rank Hunter. All Rights Reserved.


#include "Settings/CustomSettings/FHSettingResolution.h"
#include "GameFramework/GameUserSettings.h"
#include "RHI.h"

#define LOCTEXT_NAMESPACE "FRankHunter"

UFHSettingResolution::UFHSettingResolution()
{
}

void UFHSettingResolution::SetDiscreteOptionByIndex(int32 Index)
{
	if (ResolutionOptions.IsValidIndex(Index) && ResolutionOptions[Index].IsValid())
	{
		GEngine->GetGameUserSettings()->SetScreenResolution(ResolutionOptions[Index]->GetResolution());

		NotifySettingChanged(EGameSettingChangeReason::Change);
	}
}

int32 UFHSettingResolution::GetDiscreteOptionIndex() const
{
	const UGameUserSettings* UserSettings = CastChecked<const UGameUserSettings>(GEngine->GetGameUserSettings());

	FIntPoint ScreenResolutionPoint = UserSettings->GetScreenResolution();
	int32 Index = INDEX_NONE;
	for (int32 i = 0; i < ResolutionOptions.Num(); ++i)
	{
		if (ResolutionOptions[i]->GetResolution() == ScreenResolutionPoint)
		{
			Index = i;
			break;
		}
	}

	if (Index == INDEX_NONE && ResolutionOptions.Num() > 0)
	{
		Index = ResolutionOptions.Num() - 1;
	}

	return Index;
}

TArray<FText> UFHSettingResolution::GetDiscreteOptions() const
{
	TArray<FText> ResolutionTexts;
	for (int32 i = 0; i < ResolutionOptions.Num(); ++i)
	{
		ResolutionTexts.Add(ResolutionOptions[i]->GetDisplayText());
	}

	return ResolutionTexts;
}

void UFHSettingResolution::OnInitialized()
{
	Super::OnInitialized();
	
	ResolutionOptions.Empty();
	ResolutionFullscreen.Empty();
	ResolutionWindowedFullscreen.Empty();
	ResolutionWindowed.Empty();

	FDisplayMetrics InitialDisplayMetrics;
	FSlateApplication::Get().GetInitialDisplayMetrics(InitialDisplayMetrics);

	FScreenResolutionArray ResolutionArray;
	RHIGetAvailableResolutions(ResolutionArray, true);

	//** Windowed */
	{
		const FIntPoint MinResolution{ 1280, 720 };
		const FIntPoint MaxResolution(InitialDisplayMetrics.PrimaryDisplayWidth - 1, InitialDisplayMetrics.PrimaryDisplayHeight - 1); // If make a window so large that part of the game is off screen, unable to change resolutions back. 
		const float MinAspectRatio = 16.f / 10.f;

		TArray<FIntPoint> WindowedResolutions;

		if (MinResolution.X <= MaxResolution.X && MinResolution.Y <= MaxResolution.Y)
		{
			GetStandardWindowResolutions(MinResolution, MaxResolution, MinAspectRatio, WindowedResolutions);
		}

		if (GSystemResolution.WindowMode == EWindowMode::Windowed)
		{
			WindowedResolutions.AddUnique(FIntPoint(GSystemResolution.ResX, GSystemResolution.ResY));
			WindowedResolutions.Sort([](const FIntPoint& A, const FIntPoint& B)
				{
					return A.X != B.X ? A.X < B.X : A.Y < B.Y;
				});
		}

		// Non-standard device.
		if (WindowedResolutions.Num() == 0)
		{
			WindowedResolutions.Add(FIntPoint(InitialDisplayMetrics.PrimaryDisplayWidth, InitialDisplayMetrics.PrimaryDisplayHeight));
		}

		ResolutionWindowed.Empty(WindowedResolutions.Num());
		for (const FIntPoint& Resolution : WindowedResolutions)
		{
			TSharedRef<FScreenResolutionEntry> ScreenResolutionEntry = MakeShared<FScreenResolutionEntry>();
			ScreenResolutionEntry->Width = Resolution.X;
			ScreenResolutionEntry->Height = Resolution.Y;

			ResolutionWindowed.Add(ScreenResolutionEntry);
		}
	}

	//** Windowed FullScreen */
	{
		FScreenResolutionRHI* InitialResolutionRHI = ResolutionArray.FindByPredicate([InitialDisplayMetrics](const FScreenResolutionRHI& ScreenRes) 
			{
				return ScreenRes.Width == InitialDisplayMetrics.PrimaryDisplayWidth 
					&& ScreenRes.Height == InitialDisplayMetrics.PrimaryDisplayHeight;
			});

		TSharedRef<FScreenResolutionEntry> Entry = MakeShared<FScreenResolutionEntry>();
		if (InitialResolutionRHI)
		{
			Entry->Width = InitialResolutionRHI->Width;
			Entry->Height = InitialResolutionRHI->Height;
			Entry->RefreshRate = InitialResolutionRHI->RefreshRate;
		}
		else
		{
			Entry->Width = InitialDisplayMetrics.PrimaryDisplayWidth;
			Entry->Height = InitialDisplayMetrics.PrimaryDisplayHeight;
			Entry->RefreshRate = FPlatformMisc::GetMaxRefreshRate();
		}

		ResolutionWindowedFullscreen.Add(Entry);
	}

	/** FullScreen */
	{
		if (ResolutionArray.Num() > 0)
		{
			for (int32 FilterThreshold = 0; FilterThreshold < 3; ++FilterThreshold)
			{
				for (int32 ModeIndex = 0; ModeIndex < ResolutionArray.Num(); ModeIndex++)
				{
					const FScreenResolutionRHI& ScreenRes = ResolutionArray[ModeIndex];

					if (ShouldAllowFullScreenResolution(ScreenRes, FilterThreshold))
					{
						TSharedRef<FScreenResolutionEntry> Entry = MakeShared<FScreenResolutionEntry>();
						Entry->Width = ScreenRes.Width;
						Entry->Height = ScreenRes.Height;
						Entry->RefreshRate = ScreenRes.RefreshRate;

						ResolutionFullscreen.Add(Entry);
					}
				}

				if (ResolutionFullscreen.Num())
				{
					break;
				}
			}
		}
	}

	SelectAppropriateResolutions();
}

void UFHSettingResolution::OnDependencyChanged()
{
	SelectAppropriateResolutions();

	const FIntPoint CurrentResolution = GEngine->GetGameUserSettings()->GetScreenResolution();
	int32 LastDiff = CurrentResolution.SizeSquared();
	int32 Index = 0;
	for (int32 i = 0; i < ResolutionOptions.Num(); i++)
	{
		int32 Diff = FMath::Abs(CurrentResolution.SizeSquared() - ResolutionOptions[i]->GetResolution().SizeSquared());
		if (Diff <= LastDiff)
		{
			Index = i;
		}

		LastDiff = Diff;
	}

	SetDiscreteOptionByIndex(Index);
}

void UFHSettingResolution::GetStandardWindowResolutions(const FIntPoint& MinResolution, const FIntPoint& MaxResolution, float MinAspectRatio, TArray<FIntPoint>& OutResolutions)
{
	static TArray<FIntPoint> StandardResolutions;
	if (StandardResolutions.Num() == 0)
	{
		// (https://namu.wiki/w/%ED%95%B4%EC%83%81%EB%8F%84/%EB%AA%A9%EB%A1%9D)

		/** XGA(eXtended Graphics Array) */ 
		new(StandardResolutions) FIntPoint{ 1024, 768 }; // XGA 4:3
		new(StandardResolutions) FIntPoint{ 1280, 768 }; // WXGA 5:3
		new(StandardResolutions) FIntPoint{ 1280, 800 }; // WXGA 16:10
		new(StandardResolutions) FIntPoint{ 1366, 768 }; // FWXGA 16:9
		new(StandardResolutions) FIntPoint{ 1152, 864 }; // XGA+ 4:3
		new(StandardResolutions) FIntPoint{ 1440, 900 }; // WXGA+ 16:10
		new(StandardResolutions) FIntPoint{ 1680, 1050 }; // WSXGA 16:10
		new(StandardResolutions) FIntPoint{ 1280, 1024 }; // SXGA 5:4
		new(StandardResolutions) FIntPoint{ 1400, 1050 }; // SXGA+ 4:3
		new(StandardResolutions) FIntPoint{ 1600, 1200 }; // UXGA 4:3
		new(StandardResolutions) FIntPoint{ 1920, 1200 }; // WUXGA 16:10

		/** QXGA(Quad XGA) */
		new(StandardResolutions) FIntPoint{ 2048, 1536 }; // QXGA 4:3
		new(StandardResolutions) FIntPoint{ 2560, 1600 }; // WQXGA 16:10
		new(StandardResolutions) FIntPoint{ 2880, 1800 }; // WQXGA 16:10
		new(StandardResolutions) FIntPoint{ 3072, 1920 }; // WQXGA 16:10
		new(StandardResolutions) FIntPoint{ 2560, 2048 }; // QSXGA 5:4
		new(StandardResolutions) FIntPoint{ 3200, 2048 }; // WQSXGA 25:16
		new(StandardResolutions) FIntPoint{ 3200, 2400 }; // QUXGA 4:3
		new(StandardResolutions) FIntPoint{ 3840, 2400 }; // WQUXGA 16:10

		/** HXGA(Hyper XGA) */
		new(StandardResolutions) FIntPoint{ 4096, 3072 }; // HXGA 4:3
		new(StandardResolutions) FIntPoint{ 5120, 3200 }; // WHXGA 16:10
		new(StandardResolutions) FIntPoint{ 5120, 4096 }; // HSXGA 5:4
		new(StandardResolutions) FIntPoint{ 6400, 4096 }; // WHSXGA 25:16
		new(StandardResolutions) FIntPoint{ 6400, 4800 }; // HUXGA 4:3
		new(StandardResolutions) FIntPoint{ 7680, 4800 }; // WHUXGA 16:10

		/** HD(High Definition), UHD(Ultra HD) */
		new(StandardResolutions) FIntPoint{ 640, 360 }; // nHD 16:9
		new(StandardResolutions) FIntPoint{ 854, 480 }; // SD 16:9
		new(StandardResolutions) FIntPoint{ 960, 540 }; // qHD 16:9
		new(StandardResolutions) FIntPoint{ 1280, 720 }; // HD 16:9
		new(StandardResolutions) FIntPoint{ 1600, 900 }; // HD+ 16:9
		new(StandardResolutions) FIntPoint{ 1920, 1080 }; // FHD 16:9
		new(StandardResolutions) FIntPoint{ 2560, 1440 }; // QHD 16:9
		new(StandardResolutions) FIntPoint{ 2880, 1620 }; // QHD+ 16:9
		new(StandardResolutions) FIntPoint{ 3200, 1800 }; // QHD+ 16:9

		new(StandardResolutions) FIntPoint{ 3840, 2160 }; // UHD 4K 16:9
		new(StandardResolutions) FIntPoint{ 4096, 2160 }; // Digital Cinema Initiatives 4K 16:9
		new(StandardResolutions) FIntPoint{ 7680, 4320 }; // FUHD 16:9
		new(StandardResolutions) FIntPoint{ 5120, 2160 }; // UHD 5K 16:9
		new(StandardResolutions) FIntPoint{ 5120, 2880 }; // UHD+ 16:9
		new(StandardResolutions) FIntPoint{ 15360, 8640 }; // QUHD 16:9

		StandardResolutions.Sort([](const FIntPoint& A, const FIntPoint& B) 
			{ 
				return (A.X * A.Y) < (B.X * B.Y); 
			});

		for (const FIntPoint& Resolution : StandardResolutions)
		{
			if (MinResolution.X <= Resolution.X && Resolution.X <= MaxResolution.X
				&& MinResolution.Y <= Resolution.Y && Resolution.Y <= MaxResolution.Y)
			{
				const float AspectRatio = (float)Resolution.X / (float)Resolution.Y;
				if (AspectRatio > MinAspectRatio || FMath::IsNearlyEqual(AspectRatio, MinAspectRatio))
				{
					OutResolutions.Add(Resolution);
				}
			}
		}
	}
}

void UFHSettingResolution::SelectAppropriateResolutions()
{
	EWindowMode::Type const WindowMode = GEngine->GetGameUserSettings()->GetFullscreenMode();
	if (LastWindowMode != WindowMode)
	{
		LastWindowMode = WindowMode;

		ResolutionOptions.Empty();
		switch (WindowMode)
		{
		case EWindowMode::Fullscreen:
			ResolutionOptions.Append(ResolutionFullscreen);
			break;
		case EWindowMode::WindowedFullscreen:
			ResolutionOptions.Append(ResolutionWindowedFullscreen);
			break;
		case EWindowMode::Windowed:
			ResolutionOptions.Append(ResolutionWindowed);
			break;
		}

		NotifyEditConditionsChanged();
	}
}

bool UFHSettingResolution::ShouldAllowFullScreenResolution(const FScreenResolutionRHI& SrcScreenRes, int32 FilterThreshold) const
{
	FScreenResolutionRHI ScreenRes = SrcScreenRes;

	// expected: 4:3 == 1.333, 16:9 == 1.777, 16:10 == 1.6, multi-monitor-wide: >2
	bool bIsPortrait = ScreenRes.Width < ScreenRes.Height;
	float AspectRatio = (float)ScreenRes.Width / (float)ScreenRes.Height;

	// 아니;; 이런 모니터를 쓰는 사람이 있어?
	if (bIsPortrait)
	{
		AspectRatio = 1.0f / AspectRatio;
		ScreenRes.Width = SrcScreenRes.Height;
		ScreenRes.Height = SrcScreenRes.Width;
	}

	if (FilterThreshold < 1)
	{
		FDisplayMetrics DisplayMetrics;
		FSlateApplication::Get().GetInitialDisplayMetrics(DisplayMetrics);

		float DisplayAspect = AspectRatio;

		for (int32 MonitorIndex = 0; MonitorIndex < DisplayMetrics.MonitorInfo.Num(); ++MonitorIndex)
		{
			FMonitorInfo& MonitorInfo = DisplayMetrics.MonitorInfo[MonitorIndex];

			if (MonitorInfo.bIsPrimary)
			{
				DisplayAspect = (float)MonitorInfo.NativeWidth / (float)MonitorInfo.NativeHeight;
				break;
			}
		}

		if (FMath::IsNearlyEqual(DisplayAspect, AspectRatio))
		{
			return false;
		}
	}

	// MinResolution 1280x720
	if (FilterThreshold < 2 && (ScreenRes.Width < 1280 || ScreenRes.Height < 720))
	{
		return false;
	}

	return true;
}

//===============================================================================
FText UFHSettingResolution::FScreenResolutionEntry::GetDisplayText() const
{
	if (!OverrideText.IsEmpty())
	{
		return OverrideText;
	}

	FText Aspect = FText::GetEmpty();

	// expected: 4:3 == 1.333, 16:9 == 1.777, 16:10 == 1.6, multi-monitor-wide: >2
	float AspectRatio = (float)Width / (float)Height;

	if (FMath::IsNearlyEqual(AspectRatio, (4.0f / 3.0f)))
	{
		Aspect = LOCTEXT("AspectRatio 4:3", "4:3");
	}
	else if (FMath::IsNearlyEqual(AspectRatio, (16.0f / 9.0f)))
	{
		Aspect = LOCTEXT("AspectRatio 16:9", "16:9");
	}
	else if (FMath::IsNearlyEqual(AspectRatio, (16.0f / 10.0f)))
	{
		Aspect = LOCTEXT("AspectRatio 16:10", "16:10");
	}
	else if (FMath::IsNearlyEqual(AspectRatio, (3.0f / 4.0f)))
	{
		Aspect = LOCTEXT("AspectRatio 3:4", "3:4");
	}
	else if (FMath::IsNearlyEqual(AspectRatio, (9.0f / 16.0f)))
	{
		Aspect = LOCTEXT("AspectRatio 9:16", "9:16");
	}
	else if (FMath::IsNearlyEqual(AspectRatio, (10.0f / 16.0f)))
	{
		Aspect = LOCTEXT("AspectRatio 10:16", "10:16");
	}

	FNumberFormattingOptions Options;
	Options.UseGrouping = false;

	FFormatNamedArguments Args;
	Args.Add(TEXT("X"), FText::AsNumber(Width, &Options));
	Args.Add(TEXT("Y"), FText::AsNumber(Height, &Options));
	Args.Add(TEXT("AspectRatio"), Aspect);
	Args.Add(TEXT("RefreshRate"), RefreshRate);

	return FText::Format(LOCTEXT("AspectRatio", "{X} x {Y}"), Args);
}

#undef LOCTEXT_NAMESPACE