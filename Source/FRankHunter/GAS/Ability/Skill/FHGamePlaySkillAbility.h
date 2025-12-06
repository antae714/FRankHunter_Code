// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/FHGameplayAbility.h"
#include "GAS/FHGameplayTags.h"
#include "Data/FHSkillTable.h"
#include "FHGamePlaySkillAbility.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHGamePlaySkillAbility : public UFHGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFHGamePlaySkillAbility();

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	ESkillType SkillType;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	FGameplayTag ToggleStateTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillTable")
	TSoftObjectPtr<UDataTable> SkillTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	bool bHasAnimation{ false };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (EditCondition = "bHasAnimation"))
	bool bWaitForMontageEnd{ false };

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

protected: 
	// ================ 서버 로직 (Server Logic) ================ //
	virtual void ApplyPassiveEffect() {}

	virtual void ExecuteActiveAbility() {}

	virtual void OnToggleOn() {}
	virtual void OnToggleOff() {}

	void ApplyCooldownWithLevelScaling();
	virtual float GetActiveSkillCooldown();

protected:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageCancelled();

public:
    template<typename T>
    T* GetSkillData(const FName& RowName) const
    {
		if (SkillTable.IsNull() || RowName.IsNone())
		{
			UE_LOG(LogTemp, Warning, TEXT("GetSkillData failed: SkillTable is null or RowName is None."));
			return nullptr;
		}

		UDataTable* LoadedTable = SkillTable.LoadSynchronous();
		if (!LoadedTable)
		{
			UE_LOG(LogTemp, Warning, TEXT("GetSkillData failed: Could not load SkillTable '%s'."), *SkillTable.ToString());
			return nullptr;
		}

		return LoadedTable->FindRow<T>(RowName, TEXT(""));
    }

private:
	void ExecuteSkill();
};
