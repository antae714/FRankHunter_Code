// Copyright F Rank Hunter.. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "FHQuotaIncreaseTable.generated.h"

// 게임 반복 시행시 할당량(상납금) 상승률 정의를 위한 테이블
// <06-27> 현재 사용하지 않음

USTRUCT(BlueprintType)
struct FFHQuotaIncreaseTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	double IncreaseRate = 0;
};