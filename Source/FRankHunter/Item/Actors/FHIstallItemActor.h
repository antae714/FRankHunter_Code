// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item/Actors/FHWorldItemActor.h"
#include "GameplayAbilitySpec.h"
#include "FHIstallItemActor.generated.h"

class UStaticMeshComponent;
class UFHInventoryComponent;
class UGameplayAbility;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHIstallItemActor : public AFHWorldItemActor
{
	GENERATED_BODY()
	

public:
	AFHIstallItemActor();

protected:
	virtual void BeginPlay() override;
	virtual void OnInventoryChangedFunction(USiInventoryComponent* Inventory, int32 Index) override;

	UFUNCTION()
	void OnInstallEnd(UGameplayAbility* EndAbility);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> ASC;

private:
	UPROPERTY()
	FGameplayAbilitySpec LatestInstallAbilitySpec;
};
