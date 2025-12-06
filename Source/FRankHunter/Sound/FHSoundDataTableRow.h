// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FHSoundDataTableRow.generated.h"

USTRUCT(BlueprintType)
struct FFHSoundDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FName SoundName{ NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FGameplayTag SoundTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundData")
	TSoftObjectPtr<USoundBase> SoundAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (ClampMin = "0.0"))
	float VolumeMult = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (ClampMin = "0.0"))
	float PitchMult = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (ClampMin = "0.0"))
	float SpeedMult = 1.0f;
};



