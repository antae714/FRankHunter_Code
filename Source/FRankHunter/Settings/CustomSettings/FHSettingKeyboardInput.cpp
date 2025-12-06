// Copyright F Rank Hunter. All Rights Reserved.


#include "Settings/CustomSettings/FHSettingKeyboardInput.h"
#include "Settings/FHDeviceSettings.h"
#include "Player/FHLocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerMappableInputConfig.h"

#define LOCTEXT_NAMESPACE "FRankHunter"

UFHSettingKeyboardInput::UFHSettingKeyboardInput()
{
	bReportAnalytics = false;
}

void UFHSettingKeyboardInput::StoreInitial()
{
	if (const UEnhancedPlayerMappableKeyProfile* MappableKeyProfile = FindMappableKeyProfile())
	{
		if (const FKeyMappingRow* KeyMappingRow = FindKeyMappingRow())
		{
			for (const FPlayerKeyMapping& PlayerKeyMapping : KeyMappingRow->Mappings)
			{
				if (MappableKeyProfile->DoesMappingPassQueryOptions(PlayerKeyMapping, QueryOptions))
				{
					ActionMappingName = PlayerKeyMapping.GetMappingName();
					InitialKeyMappings.Add(PlayerKeyMapping.GetSlot(), PlayerKeyMapping.GetCurrentKey());
				}
			}
		}
	}
}

void UFHSettingKeyboardInput::ResetToDefault()
{
	if (UEnhancedInputUserSettings* InputUserSettings = GetInputUserSettings())
	{
		FMapPlayerKeyArgs Args = {};
		Args.MappingName = ActionMappingName;

		FGameplayTagContainer FailureReason;
		InputUserSettings->ResetAllPlayerKeysInRow(Args, FailureReason);

		NotifySettingChanged(EGameSettingChangeReason::Change);
	}
}

void UFHSettingKeyboardInput::RestoreToInitial()
{
	for (TPair<EPlayerMappableKeySlot, FKey> Pair : InitialKeyMappings)
	{
		ChangeBinding((int32)Pair.Key, Pair.Value);
	}
}

void UFHSettingKeyboardInput::ChangeBinding(int32 InKeyBindSlot, FKey NewKey)
{
	FMapPlayerKeyArgs Args = {};
	Args.MappingName = ActionMappingName;
	Args.Slot = (EPlayerMappableKeySlot)(static_cast<uint8>(InKeyBindSlot));
	Args.NewKey = NewKey;

	if (UEnhancedInputUserSettings* InputUserSettings = GetInputUserSettings())
	{
		FGameplayTagContainer FailureReason;
		InputUserSettings->MapPlayerKey(Args, FailureReason);

		NotifySettingChanged(EGameSettingChangeReason::Change);
	}
}

UEnhancedPlayerMappableKeyProfile* UFHSettingKeyboardInput::FindMappableKeyProfile() const
{
	if (UEnhancedInputUserSettings* InputUserSettings = GetInputUserSettings())
	{
		return InputUserSettings->GetKeyProfileWithIdentifier(ProfileIdentifier);
	}

	return nullptr;
}

const FKeyMappingRow* UFHSettingKeyboardInput::FindKeyMappingRow() const
{
	if (const UEnhancedPlayerMappableKeyProfile* MappableKeyProfile = FindMappableKeyProfile())
	{
		return MappableKeyProfile->FindKeyMappingRow(ActionMappingName);
	}

	return nullptr;
}

void UFHSettingKeyboardInput::OnInitialized()
{
	DynamicDetails = FGetGameSettingsDetails::CreateLambda([this](ULocalPlayer&)
		{
			if (const FKeyMappingRow* KeyMappingRow = FindKeyMappingRow())
			{
				if (KeyMappingRow->HasAnyMappings())
				{
					return FText::Format(LOCTEXT("KeyboardInputAction", "Bindings for {0}"), KeyMappingRow->Mappings.begin()->GetDisplayName());
				}
			}
			return FText::GetEmpty();
		});

	Super::OnInitialized();
}

UEnhancedInputUserSettings* UFHSettingKeyboardInput::GetInputUserSettings() const
{
	if (UFHLocalPlayer* FHLocalPlayer = Cast<UFHLocalPlayer>(LocalPlayer))
	{
		if (UEnhancedInputLocalPlayerSubsystem* System = FHLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			return System->GetUserSettings();
		}
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE