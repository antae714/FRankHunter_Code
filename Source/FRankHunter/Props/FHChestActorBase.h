// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FHChestActorBase.generated.h"


class UInputAction;

UCLASS()
class FRANKHUNTER_API AFHChestActorBase : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UFHChestDescriptor> ChestDescriptor;
	UPROPERTY(EditAnywhere)
	FString SpawnPointSocketNameBase{ TEXT("ItemSpawnPoint") };
	UPROPERTY(EditAnywhere)
	int32 SpawnPointSocketNum{ 8 };


	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> ChestBodyMesh;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> ChestLidMesh;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UPhysicsConstraintComponent> PhysicsConstraint;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UFHInteractableComponent> InteractableComp;
	
	//UPROPERTY(EditAnywhere)
	//FName ForceAnchorSocketName;

	UPROPERTY(EditAnywhere)
	float OpenLidMotorStrength{ 50.0f };
	UPROPERTY(EditAnywhere)
	float RotationLimit{ 60.0f };

public:	
	AFHChestActorBase();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;


public:
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	UPROPERTY(EditAnywhere, Category = "Chest")
	FText DefaultInteractTooltip;
	UPROPERTY(EditAnywhere, Category = "Chest")
	FText LockedInteractTooltip;


private:
	bool bIsFirstOpen{ true };
	bool bIsOpened{ false };

	UPROPERTY(ReplicatedUsing = "OnRep_bIsLocked")
	bool bIsLocked{ false };
	UFUNCTION()
	void OnRep_bIsLocked();

	UPROPERTY(ReplicatedUsing = "OnRep_bIsTrap")
	bool bIsTrap{ false };
	UFUNCTION()
	void OnRep_bIsTrap();



	TArray<FString> RandomlyAlignedSocketNames;

protected:
	void SpawnItem();
	UFUNCTION(NetMulticast, Reliable)
	void ToggleChestOpen();

	UFUNCTION(NetMulticast, Reliable)
	void UnlockChest(bool bResult);

	UFUNCTION()
	void OnInteract_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction);
	UFUNCTION()
	void OnInteractHold_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration);
	UFUNCTION()
	void OnInteractRelease_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration);

public:
	UFUNCTION(BlueprintImplementableEvent)
	void Blueprint_SpawnItem(FTransform Transform, TSubclassOf<class UFHItemBase> Item);






	// ChestTrap
protected:
	FVector TeleportLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChestTrap")
	class UNiagaraSystem* FogNiagara;
	FTimerHandle TrapTimer;
	// called on server only
	void ActivateTeleportTrap(class AFHPlayerBase* Target);

	UFUNCTION(NetMulticast, Reliable)
	void SpawnFogNiagara(FVector Loc1, FVector Loc2);
	void SpawnFogNiagara_Implementation(FVector Loc1, FVector Loc2);
};
