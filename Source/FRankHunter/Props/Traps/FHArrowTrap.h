// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Props/Traps/FHTrapBase.h"
#include "FHArrowTrap.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHArrowTrap : public AFHTrapBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator ArrowRotation{ 0, 0, 0 };

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> ArrowActorClass;

public:
	AFHArrowTrap();

protected:
	virtual void BeginPlay() override;
	virtual FName GetTrapName() override { return TEXT("ArrowTrap"); }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_ShotArrow();
	void Server_ShotArrow_Implementation();
};
