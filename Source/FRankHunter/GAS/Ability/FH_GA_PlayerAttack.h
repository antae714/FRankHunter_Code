// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item\Abilities\FH_GA_UseItem.h"
#include "FH_GA_PlayerAttack.generated.h"

// 1인칭 메쉬 애니 인스턴스가 메인
// 3인칭 메쉬 애니메이션은 따라가면서 몽타쥬 재생만

UCLASS()
class FRANKHUNTER_API UFH_GA_PlayerAttack : public UFH_GA_UseItem
{
	GENERATED_BODY()
	


public:
	/*UPROPERTY(EditAnywhere, Category=Animation)
	TArray<FString> AttackSectionNames;*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayCue")
    FGameplayTag MontageCueTag_Ready;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayCue")
    FGameplayTag MontageCueTag_Ready_Loop;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayCue")
    FGameplayTag MontageCueTag_Swing;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayCue")
	FGameplayTag MontageCueTag_CancelMontage;

	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float FP_Ready_PlayRate{ 1.0f };
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float TP_Ready_PlayRate{ 0.8f };
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float FP_Swing_PlayRate{ 1.0f };
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float TP_Swing_PlayRate{ 1.4f };


public:
	UFH_GA_PlayerAttack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

public:
	UFUNCTION()
	void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);


	UFUNCTION()
	void OnStartCharge();

	void Swing();
	/*UFUNCTION(Server, Reliable)
	void Server_Swing();*/

protected:
	bool ContinuousAttack{ false };
	bool onCharge{ false };
	bool isReleased{ false };
	bool onSwing{ false };

	UPROPERTY()
	class AFHPlayerBase* Player{ nullptr };

	UPROPERTY()
	class UFHPlayerAnimInstance* AnimInstance{ nullptr };

	UPROPERTY()
	UAnimMontage* FPMontage{ nullptr };

	UPROPERTY()
	UAnimMontage* TPMontage{ nullptr };
};
