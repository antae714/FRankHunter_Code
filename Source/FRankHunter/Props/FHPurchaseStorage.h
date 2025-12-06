// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FHPurchaseStorage.generated.h"

UCLASS()
class FRANKHUNTER_API AFHPurchaseStorage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFHPurchaseStorage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void PurchaseEnd();
	UFUNCTION()
	void NotifyPropManagerRegisted();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StorageState")
	bool bIsOpen;
};
