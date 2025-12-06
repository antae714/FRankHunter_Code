// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Door.h"
#include "FHDoor.generated.h"

class AFHCreatureBase;
class UNavLinkCustomComponent;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHDoor : public ADoor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = SmartLink)
	TObjectPtr<UNavLinkCustomComponent> SmartLinkComp;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UChildActorComponent> LeftDoorActor;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UChildActorComponent> RightDoorActor;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UBoxComponent> BoxForPreventingSpawningNearby;

public:
	AFHDoor();

	UFUNCTION(BlueprintCallable)
	void TurnOnBoxCollisionForPreventingSpawningNearby();
	
	UFUNCTION(BlueprintCallable)
	void TurnOffBoxCollisionForPreventingSpawningNearby();


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void NotifySmartLinkReached(UNavLinkCustomComponent* LinkComp, UObject* PathingAgent, const FVector& DestPoint);
	void ResumeActorMove();

	UFUNCTION()
	void OnDoorCreated(AActor* Child);
private:
	UPROPERTY()
	TObjectPtr<AFHCreatureBase> ActorInUse;
	UPROPERTY()
	TObjectPtr<UNavLinkCustomComponent> NavLinkComp;

	FTimerHandle ResumeTimer;
	uint32 bIsInit : 1;
};
