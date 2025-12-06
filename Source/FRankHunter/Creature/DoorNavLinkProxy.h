// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "DoorNavLinkProxy.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API ADoorNavLinkProxy : public ANavLinkProxy
{
	GENERATED_BODY()
	
public:
	ADoorNavLinkProxy();

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	
	//virtual bool IsNavigationRelevant() const override;

public:
	UFUNCTION()
	void SmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint);
	void ResumeActorMove();

private:
	UPROPERTY()
	TObjectPtr<AActor> ActorInUse;

	FTimerHandle ResumeTimer;
};
