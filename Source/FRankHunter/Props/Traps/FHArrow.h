// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Props/Traps/FHTrapBase.h"
#include "FHArrow.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHArrow : public AActor
{
	GENERATED_BODY()
	
public:
	AFHArrow();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:
	FVector MoveDir;
	float MoveRange;
	float TotalMoveRange;

	FTimerHandle StuddedTimeHandle;
	uint32 bIsMoving : 1 {true};

	UFUNCTION(BlueprintCallable)
	void OnBeginOverlap(AActor* Actor);
};
