// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/FHGameplayAbility.h"
#include "FH_GA_PlayerAttributeInitialize.generated.h"


UENUM(BlueprintType)
enum class EAttributeInitializeType : uint8
{
	Float UMETA(DisplayName = "Float"),
	DataTable UMETA(DisplayName = "DataTable"),
	InfiniteEffect UMETA(DisplayName = "InfiniteEffect")
};
USTRUCT(BlueprintType)
struct FAttributeInitializeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = GAS)
	FGameplayAttribute Attribute;

	UPROPERTY(EditDefaultsOnly, Category = GAS)
	EAttributeInitializeType InitializeType = EAttributeInitializeType::Float;

	UPROPERTY(EditDefaultsOnly, Category = GAS, meta = (EditCondition = "InitializeType == EAttributeInitializeType::Float", EditConditionHides))
	float FloatValue = 0.0f;
};


UCLASS()
class FRANKHUNTER_API UFH_GA_PlayerAttributeInitialize : public UFHGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = GAS)
	TArray<FAttributeInitializeInfo> AttributeInitializeInfoArray;

	UPROPERTY(EditDefaultsOnly, Category = GAS)
	TSubclassOf<class UGameplayEffect> SettingDerivedAttributeEffect;

	UPROPERTY(EditDefaultsOnly, Category = GAS)
	TObjectPtr<class UDataTable> InitDataTable;

protected:
	uint8 bUseEffect : 1{ false };


public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


protected:
	void InitAttribute(class UAbilitySystemComponent* ASC, FAttributeInitializeInfo& info);
};
