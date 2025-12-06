// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item/Actors/FHWorldItemActor.h"
#include "GameplayAbilitySpecHandle.h"
#include "Item/Actors/FHPickupItemActor.h"
#include "FHThrowItemActor.generated.h"

class UGameplayAbility;

UCLASS(Blueprintable)
class FRANKHUNTER_API AFHThrowItemActor : public AFHPickupItemActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFHThrowItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnInventoryChangedFunction(USiInventoryComponent* Inventory, int32 Index) override;


	UFUNCTION()
	void OnImpactEnd(UGameplayAbility* EndAbility);
	
public:

	UFUNCTION()
	void OnImpact(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	UFUNCTION()
	void OnRep_LaunchVelocity();
	UPROPERTY(ReplicatedUsing = OnRep_LaunchVelocity, BlueprintReadWrite)
	FVector LaunchVelocity;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> ASC;

private:

	UPROPERTY()
	FGameplayAbilitySpecHandle LatestImpactAbilitySpecHandle;

	
};
