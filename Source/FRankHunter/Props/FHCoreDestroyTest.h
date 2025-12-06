// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FHCoreDestroyTest.generated.h"

class UInputAction;

UCLASS()
class FRANKHUNTER_API AFHCoreDestroyTest : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UFHInteractableComponent> InteractableComp;

	// Sets default values for this actor's properties
	AFHCoreDestroyTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	UFUNCTION()
	void OnInteract_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction);
};
