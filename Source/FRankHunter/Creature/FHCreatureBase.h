// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Core/GameTeamTypes.h"
#include "Creature/AggroSetting.h"
#include "Creature/CreatureDataAsset.h"
#include "FHCreatureBase.generated.h"


struct FOnAttributeChangeData;

class UFHAbilitySystemComponent;
class UFHAttributeSet_Health;
class UFHAttributeSet_Creature;
class UBlackboardData;
class UBehaviorTree;
class UAnimMontage;
class UGameplayAbility;
class UAISenseConfig;
struct FAIStimulus;
class UCreatureDataAsset;

UCLASS(BlueprintType, Blueprintable)
class FRANKHUNTER_API AFHCreatureBase : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AFHCreatureBase(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	void OnChase(bool bIsChase);
	void OnDamaged(const FOnAttributeChangeData& ChangeData);
	void OnDeath();

	UPROPERTY(EditAnywhere)
	uint8 bPlayChasingSound : 1{ true };

	// Begin IGenericTeamAgentInterface
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Default, meta = (AllowPrivateAccess = "true"))
	ETeamID TeamID;

public:
	virtual FGenericTeamId GetGenericTeamId() const override;
	// ~End IGenericTeamAgentInterface


	void OnAbilityEnded(UGameplayAbility* EndAbility);
	void SelectRandomAttackData();

	UFUNCTION(BlueprintCallable)
	UAnimMontage* GetAttackMontage() const;

	UFUNCTION(BlueprintCallable)
	AActor* GetTargetActor() const;

	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentTargetActor() const;

	UFUNCTION(BlueprintCallable)
	float GetAttackRange();

	float GetDoorOpenTime();

	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);


	UFUNCTION(BlueprintImplementableEvent)
	void OnSetTargetActor(AActor* TargetActor);


	UCreatureDataAsset* GetCreatureDataAsset() const
	{
		return CreatureDataAsset;
	}

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFHAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default|AI", meta = (AllowPrivateAccess = "true", DeprecatedProperty, DeprecationMessage = "Use UCreatureDataAsset::BlackBoardData"))
	TObjectPtr<UBlackboardData> BlackBoardData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default|AI", meta = (AllowPrivateAccess = "true", DeprecatedProperty, DeprecationMessage = "Use UCreatureDataAsset::BehaviorTree"))
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default|AI", meta = (AllowPrivateAccess = "true", DeprecatedProperty, DeprecationMessage = "Use UCreatureDataAsset::TreeInitData"))
	TArray<FDynamicTreeInitData> TreeInitData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Default, meta = (AllowPrivateAccess = "true", DeprecatedProperty, DeprecationMessage = "Use UCreatureDataAsset::AggroSetting"))
	FAggroSetting AggroSetting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default|AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCreatureDataAsset> CreatureDataAsset;
	


	// AttributeSet_Movement::WalkSpeed -> MoveSpeed / RunSpeed -> ChaseSpeed
	//float ChaseSpeed;
	//float MoveSpeed;
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Default, meta = (DeprecatedProperty, DeprecationMessage = "Use UCreatureDataAsset::AttackDataTable"))
	TArray<FSimpleAttackTableElement> AttackDataTable;

	int32 LastestAttackIndex;



	// ========== Sound ===========
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Creature")
	void PlayGotDamageSound();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Creature")
	void PlayDeadSound();


	// ========= Handle Death ==========
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Creature")
	void K2_OnCreatureDeath(); // Called by GA_CreatureDeath




protected:
	// =========== VFX ===========
	UPROPERTY(EditAnywhere, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> BloodSplashNiagara;
	UPROPERTY(EditAnywhere, Category = "VFX")
	TSubclassOf<class ANiagaraActor> StunNiagaraActorClass;
	FTimerHandle StunEffectTimerHandle;
	UPROPERTY()
	TObjectPtr<ANiagaraActor> StunNiagaraActor;

public:
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "VFX")
	void ActivateBloodSplashEffect(FVector Location, FRotator Rotation);
	void ActivateBloodSplashEffect_Implementation(FVector Location, FRotator Rotation);
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "VFX")
	void ActivateStunEffect();
	void ActivateStunEffect_Implementation();
protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ActivateBloodSplashEffect(FVector Location, FRotator Rotation);
	void Multicast_ActivateBloodSplashEffect_Implementation(FVector Location, FRotator Rotation);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ActivateStunEffect();
	void Multicast_ActivateStunEffect_Implementation();

	// =========== Skill ===========
	void OnDetectedTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};

