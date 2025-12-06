// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "FindPathToActorAsync.generated.h"


USTRUCT(BlueprintType)
struct FRANKHUNTER_API FPathFindCompletParam
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Default)
	uint32 bIsSuccess : 1;

};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPathFindCompleted, FPathFindCompletParam, param);



/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFindPathToActorAsync : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UFindPathToActorAsync* FindPathToActorAsync(AController* Controller, AActor* TargetActor);
	virtual void Cancel() override;
	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
	FOnPathFindCompleted OnPathFindCompleted;

private:
	UPROPERTY()
	AController* TargetController;
	UPROPERTY()
	AActor* TargetActor;
	int32 QueryID;
	void OnPathFindCompletedDelegate(uint32 UniqueQueryID, ENavigationQueryResult::Type QueryResult, FNavPathSharedPtr NavPath);
};
