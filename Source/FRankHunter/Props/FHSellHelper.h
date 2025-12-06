// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FHSellHelper.generated.h"

UCLASS()
class FRANKHUNTER_API AFHSellHelper : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFHSellHelper();
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UFHInteractableComponent> InteractableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class AFHTray> ConnectedTray;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void Interact_Impl(class AFHPlayerBase* Player);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void TurnOffInteract();
	UFUNCTION(NetMulticast, Reliable)
	void TurnOnInteract();


	UFUNCTION(BlueprintCallable)
	void MakeInteractable();
};
