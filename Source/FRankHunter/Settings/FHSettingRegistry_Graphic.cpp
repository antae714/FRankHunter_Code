// Copyright F Rank Hunter. All Rights Reserved.


#include "Settings/FHSettingRegistry.h"
#include "Settings/FHPlayerProfileSettings.h"
#include "Settings/FHDeviceSettings.h"
#include "Player/FHLocalPlayer.h"
#include "GameSettingCollection.h"
#include "GameSettingValueDiscreteDynamic.h"
#include "GameSettingValueScalarDynamic.h"
#include "GameSettingAction.h"
#include "EditCondition/WhenCondition.h"
#include "CustomSettings/FHSettingResolution.h"
#include "CustomSettings/FHSettingVideoQuality.h"

#define LOCTEXT_NAMESPACE "FRankHunter"

void UFHSettingRegistry::InitializeGraphicSettings()
{
	if (OwningLocalPlayer == nullptr)
	{
		return;
	}

	GraphicSettings = NewObject<UGameSettingCollection>();
	GraphicSettings->SetDevName(TEXT("GraphicSettings"));
	GraphicSettings->SetDisplayName(LOCTEXT("GraphicSettings_Name", "Graphic"));
	GraphicSettings->Initialize(OwningLocalPlayer);

	UGameSettingValueDiscreteDynamic_Enum* WindowModeSetting = nullptr;

	/** Display */
	{
		UGameSettingCollection* DisplayCollection = NewObject<UGameSettingCollection>();
		DisplayCollection->SetDevName(TEXT("DisplayCollection"));
		DisplayCollection->SetDisplayName(LOCTEXT("DisplayCollection_Name", "Display"));
		GraphicSettings->AddSetting(DisplayCollection);

		/** DisplayMode Discrete */
		{
			UGameSettingValueDiscreteDynamic_Enum* SettingValue = NewObject<UGameSettingValueDiscreteDynamic_Enum>();
			SettingValue->SetDevName(TEXT("DisplayMode"));
			SettingValue->SetDisplayName(LOCTEXT("DisplayMode_Name", "DisplayMode"));
			SettingValue->SetDescriptionRichText(LOCTEXT("DisplayMode_Discription", "게임 화면을 표시하는 방식을 설정합니다. '전체 화면'은 최고의 성능을 제공하며, '전체 창 모드'는 다른 프로그램과의 빠른 전환에 유리합니다. '창 모드'는 게임을 작은 창에서 실행할 때 사용됩니다."));

			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetFullscreenMode));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetFullscreenMode));

			SettingValue->AddEnumOption(EWindowMode::Fullscreen, LOCTEXT("DisplayModeFullscreen", "Fullscreen"));
			SettingValue->AddEnumOption(EWindowMode::WindowedFullscreen, LOCTEXT("DisplayModeWindowedFullscreen", "Windowed Fullscreen"));
			SettingValue->AddEnumOption(EWindowMode::Windowed, LOCTEXT("DisplayModeWindowed", "Windowed"));

			WindowModeSetting = SettingValue;

			DisplayCollection->AddSetting(SettingValue);
		}

		/** Display Resolution */
		{
			UFHSettingResolution* SettingValue = NewObject<UFHSettingResolution>();
			SettingValue->SetDevName(TEXT("Resolution"));
			SettingValue->SetDisplayName(LOCTEXT("Resolution_Name", "Resolution"));
			SettingValue->SetDescriptionRichText(LOCTEXT("Resolution_Discription", "게임 화면의 선명도를 결정합니다. 현재 모니터의 기본 해상도(Native Resolution)로 설정하는 것을 권장합니다. 해상도를 낮추면 성능이 향상되지만, 화면이 흐릿하게 보일 수 있습니다. "));

			SettingValue->AddEditDependency(WindowModeSetting);
			SettingValue->AddEditCondition(MakeShared<FWhenCondition>([WindowModeSetting](const ULocalPlayer*, FGameSettingEditableState& InOutEditState)
				{
					if (WindowModeSetting->GetValue<EWindowMode::Type>() == EWindowMode::WindowedFullscreen)
					{
						InOutEditState.Disable(LOCTEXT("ResolutionWindowedFullscreen_Disabled", "'전체 창 모드'에서는 이 옵션이 비활성화됩니다."));
					}
				}));

			DisplayCollection->AddSetting(SettingValue);
		}

		/** Performance Stat */
		{
			AddPerformanceStatPage(DisplayCollection);
		}
	}

	/** Graphics Quality */
	{
		UGameSettingCollection* GraphicsQualityCollection = NewObject<UGameSettingCollection>();
		GraphicsQualityCollection->SetDevName(TEXT("GraphicsQualityCollection"));
		GraphicsQualityCollection->SetDisplayName(LOCTEXT("GraphicsQuality_Name", "GraphicsQuality"));
		GraphicSettings->AddSetting(GraphicsQualityCollection);

		UGameSetting* AutoSetQuality = nullptr;
		UGameSetting* GraphicsQualityPresets = nullptr;

		/** Auto Setting */
		{
			UGameSettingAction* SettingValue = NewObject<UGameSettingAction>();
			SettingValue->SetDevName(TEXT("AutoSetQuality"));
			SettingValue->SetDisplayName(LOCTEXT("AutoSetQuality_Name", "AutoSetQuality"));
			SettingValue->SetDescriptionRichText(LOCTEXT("AutoSetQuality_Description", "현재 사용 중인 PC의 하드웨어 사양(CPU, GPU)을 분석하여 최적의 그래픽 품질 설정을 자동으로 구성합니다. 어떤 옵션을 선택해야 할지 모를 때 사용하면 좋습니다."));

			SettingValue->SetDoesActionDirtySettings(true);
			SettingValue->SetActionText(LOCTEXT("AutoSetQuality_Action", "Auto-Set"));
			SettingValue->SetCustomAction([](ULocalPlayer* LocalPlayer)
				{
					const UFHLocalPlayer* LyraLocalPlayer = CastChecked<UFHLocalPlayer>(LocalPlayer);
					constexpr bool bImmediatelySaveState = false;
					LyraLocalPlayer->GetDeviceSettings()->RunAutoBenchmark(bImmediatelySaveState);
				});

			GraphicsQualityCollection->AddSetting(SettingValue);

			AutoSetQuality = SettingValue;
		}

		/** Brightness */
		{
			UGameSettingValueScalarDynamic* SettingValue = NewObject<UGameSettingValueScalarDynamic>();
			SettingValue->SetDevName(TEXT("Brightness"));
			SettingValue->SetDisplayName(LOCTEXT("Brightness_Name", "Brightness"));
			SettingValue->SetDescriptionRichText(LOCTEXT("Brightness_Description", "게임 화면의 전체적인 밝기를 조절합니다. 너무 어둡거나 밝아서 물체가 잘 보이지 않을 때 이 설정을 조정하여 최적의 시야를 확보하세요."));

			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetDisplayGamma));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetDisplayGamma));
			SettingValue->SetDefaultValue(2.2f);
			SettingValue->SetDisplayFormat([](double SourceValue, double NormalizedValue)
				{
					return FText::Format(LOCTEXT("BrightnessFormat", "{0}%"), (int32)FMath::GetMappedRangeValueClamped(FVector2D(0, 1), FVector2D(50, 150), NormalizedValue));
				});

			SettingValue->SetSourceRangeAndStep(TRange<double>(1.7, 2.7), 0.01);

			GraphicsQualityCollection->AddSetting(SettingValue);
		}

		/** Graphics Quality Presets */
		{
			UFHSettingVideoQuality* SettingValue = NewObject<UFHSettingVideoQuality>();
			SettingValue->SetDevName(TEXT("GraphicsQualityPresets"));
			SettingValue->SetDisplayName(LOCTEXT("GraphicsQualityPresets_Name", "QualityPresets"));
			SettingValue->SetDescriptionRichText(LOCTEXT("GraphicsQualityPresets_Description", "모든 개별 그래픽 설정을 '에픽', '높음', '중간', '낮음' 단계로 한 번에 변경하는 종합 설정입니다. 이 설정을 변경하면 아래의 모든 세부 옵션들이 함께 조정됩니다."));

			SettingValue->AddEditDependency(AutoSetQuality);

			GraphicsQualityCollection->AddSetting(SettingValue);

			GraphicsQualityPresets = SettingValue;
		}

		/** ResolutionScale */
		{
			UGameSettingValueScalarDynamic* SettingValue = NewObject<UGameSettingValueScalarDynamic>();
			SettingValue->SetDevName(TEXT("ResolutionScale"));
			SettingValue->SetDisplayName(LOCTEXT("ResolutionScale_Name", "ResolutionScale"));
			SettingValue->SetDescriptionRichText(LOCTEXT("ResolutionScale_Description", "UI를 제외한 3D 렌더링 해상도의 비율을 조절하여 성능을 크게 향상시킬 수 있는 중요한 옵션입니다. 값을 낮추면 프레임 속도가 크게 올라가지만, 게임 화면이 다소 뭉개져 보일 수 있습니다."));
			
			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetResolutionScaleNormalized));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetResolutionScaleNormalized));
			SettingValue->SetDisplayFormat(UGameSettingValueScalarDynamic::ZeroToOnePercent);
			
			SettingValue->AddEditDependency(AutoSetQuality);
			SettingValue->AddEditDependency(GraphicsQualityPresets);

			GraphicsQualityPresets->AddEditDependency(SettingValue);
			GraphicsQualityCollection->AddSetting(SettingValue);
		}

		/** GlobalIlluminationQuality */
		{
			UGameSettingValueDiscreteDynamic_Number* SettingValue = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			SettingValue->SetDevName(TEXT("GlobalIlluminationQuality"));
			SettingValue->SetDisplayName(LOCTEXT("GlobalIlluminationQuality_Name", "GlobalIllumination"));
			SettingValue->SetDescriptionRichText(LOCTEXT("GlobalIlluminationQuality_Description", "빛이 표면에 반사되어 주변을 간접적으로 비추는 효과의 품질을 설정합니다. 높은 설정은 사실적인 조명과 부드러운 음영을 표현하지만, 성능에 큰 영향을 줄 수 있습니다."));
			
			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetGlobalIlluminationQuality));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetGlobalIlluminationQuality));
			SettingValue->AddOption(0, LOCTEXT("VisualEffectQualityLow", "Low"));
			SettingValue->AddOption(1, LOCTEXT("VisualEffectQualityMedium", "Medium"));
			SettingValue->AddOption(2, LOCTEXT("VisualEffectQualityHigh", "High"));
			SettingValue->AddOption(3, LOCTEXT("VisualEffectQualityEpic", "Epic"));
			
			SettingValue->AddEditDependency(AutoSetQuality);
			SettingValue->AddEditDependency(GraphicsQualityPresets);

			GraphicsQualityPresets->AddEditDependency(SettingValue);

			GraphicsQualityCollection->AddSetting(SettingValue);
		}

		/** Shadows */
		{
			UGameSettingValueDiscreteDynamic_Number* SettingValue = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			SettingValue->SetDevName(TEXT("Shadows"));
			SettingValue->SetDisplayName(LOCTEXT("Shadows_Name", "Shadows"));
			SettingValue->SetDescriptionRichText(LOCTEXT("Shadows_Description", "게임 세계의 깊이감과 현실감을 더해주는 그림자의 품질을 조절합니다. 옵션을 낮추면 그림자가 단순해지거나 사라져 성능이 크게 향상됩니다."));
	
			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetShadowQuality));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetShadowQuality));
			SettingValue->AddOption(0, LOCTEXT("ShadowLow", "Off"));
			SettingValue->AddOption(1, LOCTEXT("ShadowMedium", "Medium"));
			SettingValue->AddOption(2, LOCTEXT("ShadowHigh", "High"));
			SettingValue->AddOption(3, LOCTEXT("ShadowEpic", "Epic"));
			
			SettingValue->AddEditDependency(AutoSetQuality);
			SettingValue->AddEditDependency(GraphicsQualityPresets);

			GraphicsQualityPresets->AddEditDependency(SettingValue);

			GraphicsQualityCollection->AddSetting(SettingValue);
		}

		/** AntiAliasing */
		{
			UGameSettingValueDiscreteDynamic_Number* SettingValue = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			SettingValue->SetDevName(TEXT("AntiAliasing"));
			SettingValue->SetDisplayName(LOCTEXT("AntiAliasing_Name", "Anti-Aliasing"));
			SettingValue->SetDescriptionRichText(LOCTEXT("AntiAliasing_Description", "물체의 외곽선에 나타나는 계단 현상(깨진 것처럼 보이는 현상)을 부드럽게 처리하여 깔끔한 화면을 만들어 줍니다. 높은 설정일수록 선명한 이미지를 제공하지만 약간의 성능 저하가 발생할 수 있습니다."));
			
			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetAntiAliasingQuality));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetAntiAliasingQuality));
			SettingValue->AddOption(0, LOCTEXT("AntiAliasingLow", "Off"));
			SettingValue->AddOption(1, LOCTEXT("AntiAliasingMedium", "Medium"));
			SettingValue->AddOption(2, LOCTEXT("AntiAliasingHigh", "High"));
			SettingValue->AddOption(3, LOCTEXT("AntiAliasingEpic", "Epic"));
			
			SettingValue->AddEditDependency(AutoSetQuality);
			SettingValue->AddEditDependency(GraphicsQualityPresets);

			GraphicsQualityPresets->AddEditDependency(SettingValue);

			GraphicsQualityCollection->AddSetting(SettingValue);
		}

		/** ViewDistance */
		{
			UGameSettingValueDiscreteDynamic_Number* SettingValue = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			SettingValue->SetDevName(TEXT("ViewDistance"));
			SettingValue->SetDisplayName(LOCTEXT("ViewDistance_Name", "ViewDistance"));
			SettingValue->SetDescriptionRichText(LOCTEXT("ViewDistance_Description", "멀리 있는 물체가 화면에 표시되는 거리를 설정합니다. 높은 설정은 탁 트인 풍경을 볼 수 있게 해주지만, 더 많은 오브젝트를 그려야 하므로 CPU 성능에 영향을 줄 수 있습니다."));
			
			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetViewDistanceQuality));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetViewDistanceQuality));
			SettingValue->AddOption(0, LOCTEXT("ViewDistanceNear", "Near"));
			SettingValue->AddOption(1, LOCTEXT("ViewDistanceMedium", "Medium"));
			SettingValue->AddOption(2, LOCTEXT("ViewDistanceFar", "Far"));
			SettingValue->AddOption(3, LOCTEXT("ViewDistanceEpic", "Epic"));
			
			SettingValue->AddEditDependency(AutoSetQuality);
			SettingValue->AddEditDependency(GraphicsQualityPresets);

			GraphicsQualityPresets->AddEditDependency(SettingValue);

			GraphicsQualityCollection->AddSetting(SettingValue);
		}

		/** TextureQuality */
		{
			UGameSettingValueDiscreteDynamic_Number* SettingValue = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			SettingValue->SetDevName(TEXT("TextureQuality"));
			SettingValue->SetDisplayName(LOCTEXT("TextureQuality_Name", "Textures"));
			SettingValue->SetDescriptionRichText(LOCTEXT("TextureQuality_Description", "캐릭터, 벽, 바닥 등 모든 물체 표면의 디테일과 선명도를 결정합니다. 높은 설정은 선명하고 사실적인 질감을 표현하지만, 더 많은 비디오 메모리(VRAM)를 필요로 합니다."));
			
			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetTextureQuality));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetTextureQuality));
			SettingValue->AddOption(0, LOCTEXT("TextureQualityLow", "Low"));
			SettingValue->AddOption(1, LOCTEXT("TextureQualityMedium", "Medium"));
			SettingValue->AddOption(2, LOCTEXT("TextureQualityHigh", "High"));
			SettingValue->AddOption(3, LOCTEXT("TextureQualityEpic", "Epic"));
			
			SettingValue->AddEditDependency(AutoSetQuality);
			SettingValue->AddEditDependency(GraphicsQualityPresets);

			GraphicsQualityPresets->AddEditDependency(SettingValue);

			GraphicsQualityCollection->AddSetting(SettingValue);
		}

		/** VisualEffectQuality */
		{
			UGameSettingValueDiscreteDynamic_Number* SettingValue = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			SettingValue->SetDevName(TEXT("VisualEffectQuality"));
			SettingValue->SetDisplayName(LOCTEXT("VisualEffectQuality_Name", "VisualEffects"));
			SettingValue->SetDescriptionRichText(LOCTEXT("VisualEffectQuality_Description", "폭발, 연기, 마법 효과 등 파티클 이펙트의 품질과 개수를 조절합니다. 화려한 전투 장면 등에서 프레임이 저하될 경우 이 옵션을 낮추면 도움이 될 수 있습니다."));
			
			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetVisualEffectQuality));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetVisualEffectQuality));
			SettingValue->AddOption(0, LOCTEXT("VisualEffectQualityLow", "Low"));
			SettingValue->AddOption(1, LOCTEXT("VisualEffectQualityMedium", "Medium"));
			SettingValue->AddOption(2, LOCTEXT("VisualEffectQualityHigh", "High"));
			SettingValue->AddOption(3, LOCTEXT("VisualEffectQualityEpic", "Epic"));
			
			SettingValue->AddEditDependency(AutoSetQuality);
			SettingValue->AddEditDependency(GraphicsQualityPresets);

			GraphicsQualityPresets->AddEditDependency(SettingValue);

			GraphicsQualityCollection->AddSetting(SettingValue);
		}

		/** ReflectionQuality */
		{
			UGameSettingValueDiscreteDynamic_Number* SettingValue = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			SettingValue->SetDevName(TEXT("ReflectionQuality"));
			SettingValue->SetDisplayName(LOCTEXT("ReflectionQuality_Name", "Reflections"));
			SettingValue->SetDescriptionRichText(LOCTEXT("ReflectionQuality_Description", "물이나 금속 표면에 주변 환경이 비치는 효과의 품질을 설정합니다. 높은 설정은 사실적인 반사를 표현하여 그래픽의 현실감을 크게 높여주지만, 성능 소모가 큰 기능 중 하나입니다."));
			
			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetReflectionQuality));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetReflectionQuality));
			SettingValue->AddOption(0, LOCTEXT("VisualEffectQualityLow", "Low"));
			SettingValue->AddOption(1, LOCTEXT("VisualEffectQualityMedium", "Medium"));
			SettingValue->AddOption(2, LOCTEXT("VisualEffectQualityHigh", "High"));
			SettingValue->AddOption(3, LOCTEXT("VisualEffectQualityEpic", "Epic"));
			
			SettingValue->AddEditDependency(AutoSetQuality);
			SettingValue->AddEditDependency(GraphicsQualityPresets);

			GraphicsQualityPresets->AddEditDependency(SettingValue);

			GraphicsQualityCollection->AddSetting(SettingValue);
		}

		/** PostProcessingQuality */
		{
			UGameSettingValueDiscreteDynamic_Number* SettingValue = NewObject<UGameSettingValueDiscreteDynamic_Number>();
			SettingValue->SetDevName(TEXT("PostProcessingQuality"));
			SettingValue->SetDisplayName(LOCTEXT("PostProcessingQuality_Name", "PostProcessing"));
			SettingValue->SetDescriptionRichText(LOCTEXT("PostProcessingQuality_Description", "화면 전체에 적용되는 추가적인 시각 효과(모션 블러, 렌즈 플레어, 색감 보정 등)의 품질을 설정합니다. 게임의 전체적인 분위기와 영화적인 느낌을 결정하지만, 성능에 영향을 줄 수 있습니다."));
			
			SettingValue->SetDynamicGetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(GetPostProcessingQuality));
			SettingValue->SetDynamicSetter(GET_DEVICE_SETTINGS_FUNCTION_PATH(SetPostProcessingQuality));
			SettingValue->AddOption(0, LOCTEXT("PostProcessingQualityLow", "Low"));
			SettingValue->AddOption(1, LOCTEXT("PostProcessingQualityMedium", "Medium"));
			SettingValue->AddOption(2, LOCTEXT("PostProcessingQualityHigh", "High"));
			SettingValue->AddOption(3, LOCTEXT("PostProcessingQualityEpic", "Epic"));
			
			SettingValue->AddEditDependency(AutoSetQuality);
			SettingValue->AddEditDependency(GraphicsQualityPresets);

			GraphicsQualityPresets->AddEditDependency(SettingValue);

			GraphicsQualityCollection->AddSetting(SettingValue);
		}
	}
}

#undef LOCTEXT_NAMESPACE