// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/FHRPCComponent.h"
#include "FHReturnCoreRPCComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRANKHUNTER_API UFHReturnCoreRPCComponent : public UFHRPCComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ReturnCore();
	void ReturnCore_Implementation();

	//UFUNCTION(BlueprintCallable, Client, Reliable)
	//void ShowReturnResult();
	//void ShowReturnResult_Implementation();
};
