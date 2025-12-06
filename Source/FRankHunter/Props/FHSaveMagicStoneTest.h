// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FHSaveMagicStoneTest.generated.h"

UCLASS()
class FRANKHUNTER_API AFHSaveMagicStoneTest : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY()
	TObjectPtr<class UFHInteractableComponent> InteractableComp;

	// Sets default values for this actor's properties
	AFHSaveMagicStoneTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
