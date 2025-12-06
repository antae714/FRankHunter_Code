// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/GameTeamTypes.h"
#include "GameplayTagAssetInterface.h"
#include "Creature/AggroSetting.h"
#include "CreatureDataAsset.generated.h"



class UFHAbilitySystemComponent;
class UBlackboardData;
class UBehaviorTree;



/**
 * 공격관련 작은 테이블
 */
USTRUCT(BlueprintType, Blueprintable)
struct FSimpleAttackTableElement
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Default)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Default)
	float AttackRange = 0.0f;
};

USTRUCT(BlueprintType, Blueprintable)
struct FDynamicTreeInitData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag BTTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBehaviorTree> SubTree;

};


/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UCreatureDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Default, meta = (AllowPrivateAccess = "true"))
	ETeamID TeamID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default|AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBlackboardData> BlackBoardData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default|AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default|AI", meta = (AllowPrivateAccess = "true"))
	TArray<FDynamicTreeInitData> TreeInitData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Default, meta = (AllowPrivateAccess = "true"))
	FAggroSetting AggroSetting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Default)
	TArray<FSimpleAttackTableElement> AttackDataTable;
};
