// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISense.h"
#include "AISense_SeenBy.generated.h"

class UAISenseConfig_SeenBy;

struct FSeenSenseData
{
	FSeenSenseData(const UAISenseConfig_SeenBy* AISenseConfig_SeenBy);
	float SightRadius;
	float SightRange;
	FVector SightStartOffset;
};


struct FSeenQuery
{
	TArray<FPerceptionListenerID> SeenListenerIDs;
};

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UAISense_SeenBy : public UAISense
{
	GENERATED_BODY()

public:
    UAISense_SeenBy();

protected:
	virtual void RegisterSource(AActor& SourceActors) override;
	virtual void UnregisterSource(AActor& SourceActor) override;
	virtual float Update() override;

protected:

	TMap<FPerceptionListenerID, FSeenSenseData> LitenerData;

	TMap<TObjectPtr<AActor>, FSeenQuery> TargetActors;


	void OnNewListenerImpl(const FPerceptionListener& NewListener);
	void OnListenerUpdateImpl(const FPerceptionListener& UpdatedListener);
	void OnListenerRemovedImpl(const FPerceptionListener& RemovedListener);


	bool IsTrgger(AActor* TargetActor, const FPerceptionListener& Listener);
};
