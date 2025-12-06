// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "FHItemSubsystem.generated.h"

class UFHItemBase;
class UFHItemClassDataAsset;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHItemSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection);
	TSubclassOf<UFHItemBase> GetItemClass(FName ItemID);

	void DataSetting();


private:

	// 두번다시 로드 안하려고 캐싱
	// 사소한 메모리 누수
	UPROPERTY()
	TObjectPtr<UFHItemClassDataAsset> ItemClassDataAsset;

};
