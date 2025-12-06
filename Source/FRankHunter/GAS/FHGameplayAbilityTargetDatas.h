// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Common/CommonItemEnum.h"
#include "FHGameplayAbilityTargetDatas.generated.h"

USTRUCT(BlueprintType)
struct FFHGACustomData_SwitchingType : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	EItemHoldingType FromType{ EItemHoldingType::NoHanded };
	UPROPERTY()
	EItemHoldingType ToType{ EItemHoldingType::NoHanded };
	UPROPERTY()
	TSoftObjectPtr<UStaticMesh> PendingMesh_Ready;

	// 이 구조체의 타입을 식별하기 위한 스크립트 구조체 포인터 반환 (필수?)
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FFHGACustomData_SwitchingType::StaticStruct();
	}
};

USTRUCT(BlueprintType)
struct FFHGACustomData_ThrowingPlayer : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FVector ThrowDirection{};
	UPROPERTY()
	float ThrowPower = 0.0f;
	UPROPERTY()
	uint8 bIsDead : 1;
	UPROPERTY()
	FName Targetbone;
};