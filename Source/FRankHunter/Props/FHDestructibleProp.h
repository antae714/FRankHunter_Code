// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "FHDestructibleProp.generated.h"


class UPrimitiveComponent;
class UAbilitySystemComponent;
class UFHAttributeSet_Health;
class UGeometryCollectionComponent;
struct FOnAttributeChangeData;
struct FChaosBreakEvent;


UCLASS()
class FRANKHUNTER_API AFHDestructibleProp : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UFHAttributeSet_Health> HealthAttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> RootPrimitiveComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UGeometryCollectionComponent> GeometryCollectionComponent;



public:	
	AFHDestructibleProp();

	UFUNCTION(Server, Reliable, BlueprintCallable, BlueprintAuthorityOnly, meta = (DisplayName = "Destroy"))
	void Server_Destroy();
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void OnHealthChangedData(const FOnAttributeChangeData& ChangeData);
	virtual void OnHealthChanged(float NewHealth) {}

	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "MagicStone")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MagicStone")
	TArray<TObjectPtr<UChildActorComponent>> ChildDestructionPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MagicStone")
	TArray<float> HpPercent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MagicStone")
	int32 CurrentIndex;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnDestroy"))
	void K2_OnDestroy();


	UPROPERTY(ReplicatedUsing = OnRep_IsDestroy)
	uint32 bIsDestroy : 1;

	UFUNCTION()
	virtual void OnRep_IsDestroy();
};
