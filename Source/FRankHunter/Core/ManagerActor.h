// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ManagerActor.generated.h"

UCLASS()
class FRANKHUNTER_API AManagerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AManagerActor();

	virtual FName GetManagerName() const { return NAME_None; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
};
