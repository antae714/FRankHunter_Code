// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FHTestCart.generated.h"

UCLASS()
class FRANKHUNTER_API AFHTestCart : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFHTestCart();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UFHInteractableComponent> InteractableComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UFHInventoryComponent> InventoryComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void Interact_Impl(class AFHPlayerBase* Player);

	void AddItem(TSubclassOf<class UFHItemBase> ItemClass, int32 ItemCount);

	void LateInit();

	void LockNotUsedSlot();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="StartingSlot")
	int32 StartingSlotCount = 0;
};
