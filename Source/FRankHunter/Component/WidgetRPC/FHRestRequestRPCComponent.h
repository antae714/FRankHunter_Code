// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/FHRPCComponent.h"
#include "FHRestRequestRPCComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRANKHUNTER_API UFHRestRequestRPCComponent : public UFHRPCComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFHRestRequestRPCComponent();
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void AnswerRest(bool Result);
	void AnswerRest_Implementation(bool Result);

	virtual FName GetComponentName() { return TEXT("FHRestRequestRPC"); }
};
