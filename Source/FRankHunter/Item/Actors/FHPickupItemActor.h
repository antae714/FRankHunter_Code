// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item/Actors/FHWorldItemActor.h"
#include "FHPickupItemActor.generated.h"

class UFHInventoryComponent;


UCLASS()
class FRANKHUNTER_API AFHPickupItemActor : public AFHWorldItemActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UFHInteractableComponent> InteractableComp;


	AFHPickupItemActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void Interact_Impl(class AFHPlayerBase* Player);
public:
	UFUNCTION(Server, Reliable)
	void Server_Interact(UFHInventoryComponent* ToInventory);
};
