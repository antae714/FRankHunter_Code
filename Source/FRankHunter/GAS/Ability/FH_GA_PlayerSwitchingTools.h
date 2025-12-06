// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/FHGameplayAbility.h"
#include "Common/CommonItemEnum.h"
#include "FH_GA_PlayerSwitchingTools.generated.h"

// 대략 0.6초 정도 기준
// PlayMontageCue를 쓰고 싶으므로 계산은 수동으로 하기 바람
// <25-07-10 기준>
// 1인칭: 양손꺼내기 1.00초 / 양손넣기 0.5초 / 한손꺼내기 1.1초 / 한손넣기 0.54초
// 3인칭: 양손꺼내기 1.67초 / 양손넣기 1.67초 / 한손꺼내기 1.1초 / 한손넣기 0.68초

UCLASS()
class FRANKHUNTER_API UFH_GA_PlayerSwitchingTools : public UFHGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayCue")
    FGameplayTag MontageCueTag_0to1T;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayCue")
    FGameplayTag MontageCueTag_0to1G;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayCue")
    FGameplayTag MontageCueTag_1Tto0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayCue")
    FGameplayTag MontageCueTag_1Gto0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayCue")
    FGameplayTag MontageCueTag_0to2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayCue")
    FGameplayTag MontageCueTag_2to0;

	UPROPERTY(EditDefaultsOnly, Category = "Ability Settings")
	float AbilityDuration{ 0.6f };

protected:
	UPROPERTY()
	class AFHPlayerBase* Player{ nullptr };

	UPROPERTY()
	class UFHPlayerAnimInstance* AnimInstance{ nullptr };

public:
	UFH_GA_PlayerSwitchingTools();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnTimerSet();


	void PlayMontage(EItemHoldingType PrevType, EItemHoldingType NewType);

	bool bIsTerminalOpen{ false };
};
