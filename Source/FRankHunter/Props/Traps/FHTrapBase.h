// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FHTrapBase.generated.h"

UCLASS()
class FRANKHUNTER_API AFHTrapBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFHTrapBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual FName GetTrapName() { return NAME_None; }
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
