// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FHMagicExtractor.generated.h"

UCLASS()
class FRANKHUNTER_API AFHMagicExtractor : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UFHInteractableComponent> InteractableComp;

	// Sets default values for this actor's properties
	AFHMagicExtractor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void Interact_Impl(class AFHPlayerBase* Player);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TargetScreen")
	TObjectPtr<class AFHDisplayBoard> Sign;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<class UFHPaymentBase> PaymentUI;
};
