// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/FHRPCComponent.h"
#include "FHActorRPCComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRANKHUNTER_API UFHActorRPCComponent : public UFHRPCComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Server, Reliable)
	void SetSkill(FName SkillID);
	void SetSkill_Implementation(FName SkillID);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void EnterReady();
	void EnterReady_Implementation();
};
