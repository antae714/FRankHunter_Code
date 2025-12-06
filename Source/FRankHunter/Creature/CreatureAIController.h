// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "CreatureAIController.generated.h"

struct FAggroSetting;
struct FAIStimulus;
struct FOnAttributeChangeData;
class UAIPerceptionComponent;


USTRUCT()
struct FAggroPoint
{
	GENERATED_BODY()

	uint32 bIsSight : 1;
	float AggroPoint;
};

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API ACreatureAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACreatureAIController();

public:
	//virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	FName GetAttackRangeName();
	void InitAggroSetting(const FAggroSetting& AggroSetting);

	UFUNCTION(BlueprintCallable)
	void Die();

	UFUNCTION(BlueprintCallable)
	void ForgetTarget();

	UFUNCTION(BlueprintCallable)
	void ForgetHearing();

	UFUNCTION(BlueprintCallable)
	void ClearAggro();

	UFUNCTION(BlueprintCallable)
	AActor* GetTargetActor() const;
private:
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	void OnTargetPerceptionUpdated_Sight(AActor* Actor, FAIStimulus Stimulus);
	void OnTargetPerceptionUpdated_Hearing(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(Transient)
	TObjectPtr<AActor> TargetActor;
	float AggroElapsedTime;

	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* NewTargetActor);

	UFUNCTION(BlueprintCallable)
	void OnDamaged(float Damage, AActor* DamageInstigator);

	void OnStunTagChanged(const FGameplayTag GameplayTag, int32 TagCount);
	void OnDieTagChanged(const FGameplayTag GameplayTag, int32 TagCount);
	void OnTargetDie(const FOnAttributeChangeData& ChangeData);
protected:
	UFUNCTION(BlueprintImplementableEvent)
	bool IsHearingSenseTrigger(FAIStimulus Stimulus) const;

private:
	uint32 bIsHearing : 1;
	uint32 bIsMovetoHearing : 1;
	uint32 bIsStun : 1;
	uint32 bIsDie : 1;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BlackBoardKey, meta = (AllowPrivateAccess = "true"))
	FName IsNoiseDetectName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BlackBoardKey, meta = (AllowPrivateAccess = "true"))
	FName IsSightDetectName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BlackBoardKey, meta = (AllowPrivateAccess = "true"))
	FName IsMoveToLastTargetLocationName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BlackBoardKey, meta = (AllowPrivateAccess = "true"))
	FName LastTargetLocationName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BlackBoardKey, meta = (AllowPrivateAccess = "true"))
	FName TargetActorName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BlackBoardKey, meta = (AllowPrivateAccess = "true"))
	FName AttackRangeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BlackBoardKey, meta = (AllowPrivateAccess = "true"))
	FName IsStunName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BlackBoardKey, meta = (AllowPrivateAccess = "true"))
	FName IsDieName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;


	float TriggerAggro;
	float SightAggroWeight;
	float HitAggroWeight;
	float SightAgroIncreaseDelta;
	float SightAgroDecreaseDelta;


	UPROPERTY()
	TMap<AActor*, FAggroPoint> AggroMap;

	FTimerHandle HearingForggetHandle;

	FVector LastHearingLocation;
	float CachedSightAngle;
};
