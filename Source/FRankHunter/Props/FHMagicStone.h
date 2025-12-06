// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Props/FHDestructibleProp.h"
#include "AbilitySystemInterface.h"
#include "Interfaces\SiInventorySystemInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayEffectTypes.h"
#include "FHMagicStone.generated.h"

class UAbilitySystemComponent;
class UFHAttributeSet_Health;
class UFHInventoryComponent;
class UGameplayAbility;
class UFieldSystemComponent;
class AFieldSystemActor;
class UBoxComponent;
class AFHDestructionFieldSystemActor;
class UChildActorComponent;
class UGeometryCollectionComponent;
class USceneComponent;
class UFHItemDropPoinManualComponent;

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE(FOnManaStoneSpawn, AFHMagicStone, OnManaStoneSpawn);

UCLASS()
class FRANKHUNTER_API AFHMagicStone : public AFHDestructibleProp, public ISiInventorySystemInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UFHInventoryComponent> InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> HitBoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UFHItemDropPoinManualComponent> ItemDropPoint;
	
protected:
	// ==== for sound ====
	float InitHealth;
	bool bIsDestroyedHalf{ false };
	bool bIsDestroyedFully{ false };



public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Value{ 0.0f };


	// Sets default values for this actor's properties
	AFHMagicStone();

	virtual USiInventoryComponent* GetInventoryComponent() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnHealthChanged(float NewHealth) override;
	virtual void OnRep_IsDestroy() override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnHealthChanged"))
	void K2_OnHealthChanged(float NewHealth);
};
