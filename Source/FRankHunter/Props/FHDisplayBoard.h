// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FHDisplayBoard.generated.h"

UCLASS()
class FRANKHUNTER_API AFHDisplayBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFHDisplayBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void NotifyPropManagerRegisted();
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetCurrentQuota(int32 Value);

	UFUNCTION(BlueprintImplementableEvent)
	void AddExtractResult(int32 Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	TObjectPtr<class UStaticMeshComponent> Body;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	TObjectPtr<class UWidgetComponent> Widget;
};
