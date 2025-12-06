// Copyright F Rank Hunter. All Rights Reserved.


#include "BTElement/Tasks/BTTask_FindManaStone.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/FHAttributeSet_Health.h"

UBTTask_FindManaStone::UBTTask_FindManaStone()
{
	TargetActor.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindManaStone, TargetActor), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_FindManaStone::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
	{
		return EBTNodeResult::Failed;
	}

	APawn* AIPawn = AICon->GetPawn();
	if (!AIPawn || !FindClass)
	{
		return EBTNodeResult::Failed;
	}

	// 가장 가까운 액터 찾기
	AActor* NearestActor = nullptr;
	float NearestDistSq = FLT_MAX;
	const FVector PawnLocation = AIPawn->GetActorLocation();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(AIPawn->GetWorld(), FindClass, FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (!Actor)
		{
			continue;
		}

		UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
		bool isFound;
		float Health = AbilitySystemComponent->GetGameplayAttributeValue(UFHAttributeSet_Health::GetHealthAttribute(), isFound);

		if (!isFound)
		{
			continue;
		}

		if (Health <= 0.0f)
		{
			continue;
		}

		float DistSq = FVector::DistSquared(PawnLocation, Actor->GetActorLocation());
		if (DistSq < NearestDistSq)
		{
			NearestDistSq = DistSq;
			NearestActor = Actor;
		}
	}

	if (NearestActor)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetActor.SelectedKeyName, NearestActor);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(TargetActor.SelectedKeyName);
	}
	return EBTNodeResult::Succeeded;
}