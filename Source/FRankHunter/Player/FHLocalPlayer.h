// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "FHLocalPlayer.generated.h"

class UFHPlayerProfileSettings;
class UFHDeviceSettings;
class UInputMappingContext;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()
	
public:
	UFHLocalPlayer();

	virtual void PostInitProperties() override;

	UFUNCTION()
	UFHPlayerProfileSettings* GetPlayerProfileSettings() const;

	UFUNCTION()
	UFHDeviceSettings* GetDeviceSettings() const;

	void LoadPlayerProfileSettings(bool bForceLoad = false);

protected:
	/* @See : LoadPlayerProfileSettings */
	void OnPlayerProfileSettingsLoaded(UFHPlayerProfileSettings* LoadedOrCreatedSettings);

private:
	UPROPERTY(Transient)
	mutable TObjectPtr<UFHPlayerProfileSettings> PlayerProfileSettings;

	FUniqueNetIdRepl UniqueNetId;

	UPROPERTY(Transient)
	TObjectPtr<const UInputMappingContext> InputMappingContext;
};
