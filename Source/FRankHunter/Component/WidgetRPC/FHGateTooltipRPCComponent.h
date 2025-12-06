// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/FHRPCComponent.h"
#include "FHGateTooltipRPCComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRANKHUNTER_API UFHGateTooltipRPCComponent : public UFHRPCComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFHGateTooltipRPCComponent();

	virtual FName GetComponentName() { return TEXT("UFHGateTooltipRPC"); }
};
