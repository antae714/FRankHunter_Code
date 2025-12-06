// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISenseConfig.h"
#include "AISenseConfig_SeenBy.generated.h"

class UAISense_SeenBy;
class UAISense;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UAISenseConfig_SeenBy : public UAISenseConfig
{
	GENERATED_BODY()

public:
    UAISenseConfig_SeenBy();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sense", NoClear, config)
    TSubclassOf<UAISense_SeenBy> Implementation;

    // 최대 감지 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
    float SightRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
    float SightRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
    FVector SightStartOffset;
    

    virtual TSubclassOf<UAISense> GetSenseImplementation() const override;
};
