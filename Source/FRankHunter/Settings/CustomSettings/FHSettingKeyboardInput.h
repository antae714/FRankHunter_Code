// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameSettingValue.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "FHSettingKeyboardInput.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHSettingKeyboardInput : public UGameSettingValue
{
	GENERATED_BODY()

public:
	UFHSettingKeyboardInput();

	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	void ChangeBinding(int32 InKeyBindSlot, FKey NewKey);

	UEnhancedPlayerMappableKeyProfile* FindMappableKeyProfile() const;
	
	const FKeyMappingRow* FindKeyMappingRow() const;

	UEnhancedInputUserSettings* GetInputUserSettings() const;

protected:
	virtual void OnInitialized() override;

	FName ActionMappingName;

	FPlayerMappableKeyQueryOptions QueryOptions;

	FGameplayTag ProfileIdentifier;

	TMap<EPlayerMappableKeySlot, FKey> InitialKeyMappings;
};
