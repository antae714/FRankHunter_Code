// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Props/Traps/FHTrapBase.h"
#include "FHChestTrap.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHChestTrap : public AFHTrapBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> ChestMesh;

	AFHChestTrap();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UFHInteractableComponent> InteractableComponent;

	virtual void BeginPlay() override;
protected:

public:
	UFUNCTION(BlueprintCallable)
	virtual void Interact_Impl(class AFHPlayerBase* Player);

	UFUNCTION(BlueprintCallable)
	void TPPlayer();

	UPROPERTY()
	class AFHPlayerBase* CachedPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogActor")
	TSubclassOf<AActor> FogActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TempTPPositionActor")
	TObjectPtr<AActor> TPPositionActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FogActor")
	TObjectPtr<AActor> FogActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogNiagara")
	class UNiagaraSystem* NiagaraSystem;

};
