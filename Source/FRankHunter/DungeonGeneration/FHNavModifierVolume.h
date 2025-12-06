// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NavModifierVolume.h"
#include "FHNavModifierVolume.generated.h"

UENUM(BlueprintType)
enum class EFHNavModifierVolumeType : uint8
{
	Default			UMETA(DisplayName = "Default (Prvent spawn and move)"),
	PreventSpawn	UMETA(DisplayName = "Prevent spawn only")
};

UCLASS()
class FRANKHUNTER_API AFHNavModifierVolume : public ANavModifierVolume
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "FRankHunter")
	EFHNavModifierVolumeType VolumeType{ EFHNavModifierVolumeType::PreventSpawn };
};
