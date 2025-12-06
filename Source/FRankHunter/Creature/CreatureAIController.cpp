// Copyright F Rank Hunter. All Rights Reserved.


#include "Creature/CreatureAIController.h"
#include "Creature/FHCreatureBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/FHAttributeSet_Health.h"
#include "Creature/AggroSetting.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHSoundManagerActor.h"
#include "GAS/FHGameplayTags.h"
#include "Player/FHPlayerBase.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GAS/FHAttributeSet_Creature.h"
#include "Perception/AISenseConfig_Hearing.h"

ACreatureAIController::ACreatureAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent")); 

	IsNoiseDetectName = TEXT("IsNoiseDetect");
	IsSightDetectName = TEXT("IsSightDetect");
	IsMoveToLastTargetLocationName = TEXT("IsMoveToLastTargetLocation");
	LastTargetLocationName = TEXT("LastTargetLocation");
	TargetActorName = TEXT("TargetActor");
	AttackRangeName = TEXT("AttackRange");
	IsStunName = TEXT("IsStun");
	IsDieName = TEXT("IsDie");

	PrimaryActorTick.bCanEverTick = true;
}

void ACreatureAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	check(AIPerceptionComponent);

	//Setting UCreatureDataAsset
	AFHCreatureBase* FHCreatureBase = Cast<AFHCreatureBase>(InPawn);
	UCreatureDataAsset* CreatureData = FHCreatureBase ? FHCreatureBase->GetCreatureDataAsset() : nullptr;
	if (CreatureData)
	{
		UBlackboardComponent* AIBlackboard = GetBlackboardComponent();
		if (UseBlackboard(CreatureData->BlackBoardData, AIBlackboard))
		{
			bool RunResult = RunBehaviorTree(CreatureData->BehaviorTree);
			ensure(RunResult);
		}

		UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(GetBrainComponent());
		if (BTComp)
		{
			for (auto& data : CreatureData->TreeInitData)
			{
				BTComp->SetDynamicSubtree(data.BTTag, data.SubTree);
			}
		}
		FHCreatureBase->SelectRandomAttackData();
		SetGenericTeamId((uint8)CreatureData->TeamID);
		InitAggroSetting(CreatureData->AggroSetting);
	}

	//Setting AIPerceptionComponent Delegate
	if (!AIPerceptionComponent->OnTargetPerceptionUpdated.IsAlreadyBound(this, &ThisClass::OnTargetPerceptionUpdated))
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetPerceptionUpdated);
	}

	//Setting ASC
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn);
	if (ASC)
	{
		const UFHAttributeSet_Creature* AttributeSet_Creature = ASC->GetSet<UFHAttributeSet_Creature>();
		if (AttributeSet_Creature && AIPerceptionComponent)
		{
			UAISenseConfig_Sight* AISenseConfig_Sight = AIPerceptionComponent->GetSenseConfig<UAISenseConfig_Sight>();
			if (AISenseConfig_Sight)
			{
				AISenseConfig_Sight->PeripheralVisionAngleDegrees = AttributeSet_Creature->GetSightAngle();
				AISenseConfig_Sight->LoseSightRadius = AttributeSet_Creature->GetSightRange() * 2;
				AISenseConfig_Sight->SightRadius = AttributeSet_Creature->GetSightRange();
				CachedSightAngle = AISenseConfig_Sight->PeripheralVisionAngleDegrees;
			}

			UAISenseConfig_Hearing* AISenseConfig_Hearing = AIPerceptionComponent->GetSenseConfig<UAISenseConfig_Hearing>();
			if (AISenseConfig_Hearing)
			{
				AISenseConfig_Hearing->HearingRange = AttributeSet_Creature->GetHearingRange();
			}
		}

		FOnGameplayEffectTagCountChanged::FDelegate OnStunTagChangedDelegate;
		FOnGameplayEffectTagCountChanged::FDelegate OnDieTagChangedDelegate;

		OnStunTagChangedDelegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &ThisClass::OnStunTagChanged);
		OnDieTagChangedDelegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &ThisClass::OnDieTagChanged);

		ASC->RegisterAndCallGameplayTagEvent(GET_GAMEPLAY_TAG_CREATURE_STATE_STUN,
											 OnStunTagChangedDelegate,
											 EGameplayTagEventType::NewOrRemoved);


		ASC->RegisterAndCallGameplayTagEvent(GET_GAMEPLAY_TAG_CREATURE_STATE_DEATH,
											 OnDieTagChangedDelegate,
											 EGameplayTagEventType::NewOrRemoved);
	}


	//Update UAIPerceptionSystem
	UAIPerceptionSystem* AIPerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld());
	if (AIPerceptionSys != nullptr)
	{
		if (AIPerceptionComponent->GetSensesConfigIterator())
		{
			AIPerceptionSys->UpdateListener(*AIPerceptionComponent);
		}
	}

}

void ACreatureAIController::OnUnPossess()
{
	Super::OnUnPossess();

	AIPerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &ThisClass::OnTargetPerceptionUpdated);

}

void ACreatureAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); 
	
	if (bIsDie)
	{
		return;
	}


	AActor* MaxPointActor = nullptr;
	float MaxAggroPoint = FLT_MIN;

	for (auto It = AggroMap.CreateIterator(); It; ++It)
	{
		auto& [Actor, Point] = *It;
		if (Point.bIsSight && !bIsStun)
		{
			Point.AggroPoint += SightAgroIncreaseDelta * SightAggroWeight * DeltaTime;
		}
		else
		{
			Point.AggroPoint += -SightAgroDecreaseDelta * SightAggroWeight * DeltaTime;
		}

		if (Point.AggroPoint <= 0)
		{
			It.RemoveCurrent();
			continue;
		}

		APawn* ActorPawn = Cast<APawn>(Actor);
		if (!ActorPawn || !ActorPawn->Controller)
		{
			It.RemoveCurrent();
			continue;
		}

		MaxAggroPoint = FMath::Max(MaxAggroPoint, Point.AggroPoint);

		if (MaxAggroPoint == Point.AggroPoint)
		{
			MaxPointActor = Actor;
		}

	}

	if (MaxAggroPoint >= TriggerAggro)
	{
		SetTarget(MaxPointActor);
	}

	if (AggroMap.Num() > 0 && TargetActor)
	{
		if (!AggroMap.Find(TargetActor))
		{
			ForgetTarget();
		}
	}
}

void ACreatureAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}

void ACreatureAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		OnTargetPerceptionUpdated_Sight(Actor, Stimulus);
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		OnTargetPerceptionUpdated_Hearing(Actor, Stimulus);
	}
}

void ACreatureAIController::OnTargetPerceptionUpdated_Sight(AActor* Actor, FAIStimulus Stimulus)
{
	FAggroPoint& AggroPoint = AggroMap.FindOrAdd(Actor);
	AggroPoint.bIsSight = Stimulus.IsActive();


}

void ACreatureAIController::OnTargetPerceptionUpdated_Hearing(AActor* Actor, FAIStimulus Stimulus)
{
	if (!IsHearingSenseTrigger(Stimulus))
	{
		return;
	}


	if (Stimulus.WasSuccessfullySensed())
	{
		if (GetBlackboardComponent()->GetValueAsBool(IsNoiseDetectName))
		{
			//if (FVector::DistSquared(LastHearingLocation, Stimulus.StimulusLocation) < FMath::Square(HearingDistance))
			//{
			//}
			GetWorldTimerManager().ClearTimer(HearingForggetHandle);

			GetBlackboardComponent()->SetValueAsBool(IsMoveToLastTargetLocationName, true);
			GetBlackboardComponent()->SetValueAsVector(LastTargetLocationName, Stimulus.StimulusLocation);
		}
		if(!GetBlackboardComponent()->GetValueAsBool(IsMoveToLastTargetLocationName))
		{
			LastHearingLocation = Stimulus.StimulusLocation;
			GetBlackboardComponent()->SetValueAsBool(IsNoiseDetectName, true);
			GetBlackboardComponent()->SetValueAsVector(LastTargetLocationName, Stimulus.StimulusLocation);

			GetWorldTimerManager().ClearTimer(HearingForggetHandle);
			GetWorldTimerManager().SetTimer(HearingForggetHandle, this, &ThisClass::ForgetHearing, 5.0f);
		}
	}
}

void ACreatureAIController::ForgetTarget()
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UFHAttributeSet_Health::GetHealthAttribute()).RemoveAll(this);
	}
	AggroMap.Remove(TargetActor);

	TargetActor = nullptr;
	GetBlackboardComponent()->ClearValue(TargetActorName);
	GetBlackboardComponent()->ClearValue(LastTargetLocationName);
	GetPawn<AFHCreatureBase>()->OnChase(false);

	UAISenseConfig_Sight* AISenseConfig_Sight = GetAIPerceptionComponent()->GetSenseConfig<UAISenseConfig_Sight>();
	if (AISenseConfig_Sight)
	{
		AISenseConfig_Sight->PeripheralVisionAngleDegrees = CachedSightAngle;
		UAIPerceptionSystem* AIPerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld());
		if (AIPerceptionSys != nullptr)
		{
			if (AIPerceptionComponent->GetSensesConfigIterator())
			{
				AIPerceptionSys->UpdateListener(*AIPerceptionComponent);
			}
		}
	}

}

void ACreatureAIController::ForgetHearing()
{
	GetBlackboardComponent()->ClearValue(IsNoiseDetectName);
	GetBlackboardComponent()->ClearValue(LastTargetLocationName);
	GetBlackboardComponent()->ClearValue(IsMoveToLastTargetLocationName);
}

void ACreatureAIController::ClearAggro()
{
	AggroMap.Empty();
	ForgetTarget();
	GetBlackboardComponent()->ClearValue(IsNoiseDetectName);
	GetBlackboardComponent()->ClearValue(LastTargetLocationName);
	GetBlackboardComponent()->ClearValue(IsMoveToLastTargetLocationName);
	GetWorldTimerManager().ClearTimer(HearingForggetHandle);
}

AActor* ACreatureAIController::GetTargetActor() const
{
	return TargetActor;
}

void ACreatureAIController::SetTarget(AActor* NewTargetActor)
{
	if (!NewTargetActor)
	{
		return;
	}
	if (TargetActor != NewTargetActor)
	{
		TargetActor = NewTargetActor;
		GetBlackboardComponent()->SetValueAsObject(TargetActorName, NewTargetActor);
		GetPawn<AFHCreatureBase>()->OnChase(true);


		UAISenseConfig_Sight* AISenseConfig_Sight = GetAIPerceptionComponent()->GetSenseConfig<UAISenseConfig_Sight>();
		if (AISenseConfig_Sight)
		{
			AISenseConfig_Sight->PeripheralVisionAngleDegrees = 180.0f;
			UAIPerceptionSystem* AIPerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld());
			if (AIPerceptionSys != nullptr)
			{
				if (AIPerceptionComponent->GetSensesConfigIterator())
				{
					AIPerceptionSys->UpdateListener(*AIPerceptionComponent);
				}
			}
		}

		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(NewTargetActor);
		if (ASC)
		{
			ASC->GetGameplayAttributeValueChangeDelegate(UFHAttributeSet_Health::GetHealthAttribute()).AddUObject(this, &ThisClass::OnTargetDie);
		}
	}
}

void ACreatureAIController::OnDamaged(float Damage, AActor* DamageInstigator)
{
	if (DamageInstigator)
	{
		FAggroPoint& AggroPoint = AggroMap.FindOrAdd(DamageInstigator);
		AggroPoint.AggroPoint += Damage * HitAggroWeight;
	}
}

void ACreatureAIController::OnStunTagChanged(const FGameplayTag GameplayTag, int32 TagCount)
{
	bIsStun = !!TagCount;
	if (GetBlackboardComponent())
	{
		if (bIsStun)
		{
			GetBlackboardComponent()->SetValueAsBool(IsStunName, true);
		}
		else
		{
			GetBlackboardComponent()->ClearValue(IsStunName);
		}
	}
}

void ACreatureAIController::OnDieTagChanged(const FGameplayTag GameplayTag, int32 TagCount)
{
	bIsDie = !!TagCount;
	if (GetBlackboardComponent())
	{
		if (bIsDie)
		{
			GetBlackboardComponent()->SetValueAsBool(IsDieName, true);
		}
		else
		{
			GetBlackboardComponent()->ClearValue(IsDieName);
		}
	}
}

void ACreatureAIController::OnTargetDie(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.NewValue <= 0)
	{
		ForgetTarget();

		GetBlackboardComponent()->ClearValue(TEXT("IsCombat"));
	}

}

FName ACreatureAIController::GetAttackRangeName()
{
	return AttackRangeName;
}

void ACreatureAIController::InitAggroSetting(const FAggroSetting& AggroSetting)
{
	TriggerAggro = AggroSetting.TriggerAggro;
	SightAggroWeight = AggroSetting.SightAggroWeight;
	HitAggroWeight = AggroSetting.HitAggroWeight;
	SightAgroIncreaseDelta = AggroSetting.SightAgroIncreaseDelta;
	SightAgroDecreaseDelta = AggroSetting.SightAgroDecreaseDelta;
}


void ACreatureAIController::Die()
{
	ForgetHearing();
	ForgetTarget();
	UAIPerceptionSystem* AIPerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld());
	if (AIPerceptionSys)
	{
		if (AIPerceptionComponent->GetSensesConfigIterator())
		{
			AIPerceptionSys->UnregisterListener(*AIPerceptionComponent);

		}
	}
}
