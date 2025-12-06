// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sound/FHSoundDataTableRow.h"
#include "FHSoundSystem.generated.h"


// ∞¡ FHBlueprintFunctionLibraryø° ≥÷¿Ω

UCLASS()
class FRANKHUNTER_API UFHSoundSystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	UPROPERTY()
	TMap<FGameplayTag, FFHSoundDataTableRow> TagMap;
	UPROPERTY()
	TMap<FName, FFHSoundDataTableRow> NameMap;

public:
    UFUNCTION(BlueprintCallable)
    FFHSoundDataTableRow GetSoundDataByTag(FGameplayTag SoundTag);
    UFUNCTION(BlueprintCallable)
    FFHSoundDataTableRow GetSoundDataByName(FName SoundName);

    UFUNCTION(BlueprintCallable)
    USoundBase* GetSoundAssetByTag(FGameplayTag SoundTag);
    UFUNCTION(BlueprintCallable)
    USoundBase* GetSoundAssetByName(FName SoundName);

    UFUNCTION(BlueprintCallable)
    USoundBase* GetSoundAssetFromSoundData(const FFHSoundDataTableRow& SoundData);

    UFUNCTION(BlueprintCallable)
    class AFHSoundManagerActor* GetSoundManagerActor();
};