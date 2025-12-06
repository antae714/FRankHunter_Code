// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces\SiInventorySystemInterface.h"
#include "AbilitySystemInterface.h"
#include "GameSavable.h"
#include "FHBackpack.generated.h"


class UFHInteractableComponent;
class UFHInventoryComponent;
class UInventoryBase;
class AFHPlayerBase;
class UInputAction;

UCLASS()
class FRANKHUNTER_API AFHBackpack : public AActor, public ISiInventorySystemInterface, public IAbilitySystemInterface, public IGameSavable
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFHInteractableComponent> InteractableComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFHInventoryComponent> InventoryComponent;

	UPROPERTY(EditAnywhere, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInventoryBase> InventoryUIClass;

	UPROPERTY()
	TObjectPtr<UInventoryBase> InventoryUIInstance;
	uint8 bIsWidgetPopup : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> OpenInventoryUIInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> PickUpInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	int32 InventoryCount;

public:	
	AFHBackpack();

public:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual USiInventoryComponent* GetInventoryComponent() const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void SerializeData(FArchive& Ar) override;

	UPROPERTY(Replicated, Transient)
	uint32 bIsEquipped : 1;

	UPROPERTY(ReplicatedUsing=OnRep_BackpackName, BlueprintReadWrite, Transient)
	FName BackpackName;

	UPROPERTY()
	AActor* PickedUpActor;


	UFUNCTION()
	void OnRep_BackpackName();

	UFUNCTION(BlueprintImplementableEvent)
	void OnChangedBackpackName(FName NewBackpackName);

	UFUNCTION()
	void OnInteract_Impl(AFHPlayerBase* Player, const UInputAction* InputAction);
	UFUNCTION()
	void OnInteractHold_Impl(AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration);
};
