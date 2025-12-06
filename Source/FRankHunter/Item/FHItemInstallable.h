// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item/FHItemBase.h"
#include "FHItemInstallable.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHItemInstallable : public UFHItemBase
{
	GENERATED_BODY()

public:
	virtual void SetItemData(const FFHItemData& data) override;

	UPROPERTY(Category = "Item|Ability", VisibleAnywhere, BlueprintReadWrite)
	TSubclassOf<UFHGameplayAbility> InstallAbility;

	UPROPERTY(Category = "Item|Data", EditAnywhere, BlueprintReadWrite)
	int InstallLength;
};
