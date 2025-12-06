// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/FHRPCComponent.h"
#include "FHGateEnterRequestRPCComponent.generated.h"

enum class EGateRank : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRANKHUNTER_API UFHGateEnterRequestRPCComponent : public UFHRPCComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFHGateEnterRequestRPCComponent();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OnAnswerEnterRequest(bool Answer);
	void OnAnswerEnterRequest_Implementation(bool Answer);

	// if player answer ok then call this.
	UFUNCTION(Client, Reliable)
	void OnPlayerAnserRequest(const FString& PlayerName);
	void OnPlayerAnserRequest_Implementation(const FString& PlayerName);

	virtual FName GetComponentName() { return TEXT("UFHGateEnterRequestRPC"); }
};
