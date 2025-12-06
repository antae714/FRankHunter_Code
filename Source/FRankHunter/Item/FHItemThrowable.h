// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item/FHItemBase.h"
#include "FHItemThrowable.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHItemThrowable : public UFHItemBase
{
	GENERATED_BODY()

public:
	virtual void SetItemData(const FFHItemData& data) override;


	UPROPERTY(Category = "Item|Ability", VisibleAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> ImpactAbility;

	UPROPERTY(Category = "Item", EditAnywhere, BlueprintReadWrite)
	FVector LaunchVelocity;
};
