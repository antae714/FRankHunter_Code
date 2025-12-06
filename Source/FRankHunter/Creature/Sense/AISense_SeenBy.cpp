// Copyright F Rank Hunter. All Rights Reserved.


#include "Creature/Sense/AISense_SeenBy.h"
#include "Creature/Sense/AISenseConfig_SeenBy.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/SpotLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "AIHelpers.h"
#include "GenericTeamAgentInterface.h" // team checks


UAISense_SeenBy::UAISense_SeenBy()
{
    NotifyType = EAISenseNotifyType::OnPerceptionChange;
	bAutoRegisterAllPawnsAsSources = true;
	RequestImmediateUpdate();

	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		OnNewListenerDelegate.BindUObject(this, &UAISense_SeenBy::OnNewListenerImpl);
		OnListenerUpdateDelegate.BindUObject(this, &UAISense_SeenBy::OnListenerUpdateImpl);
		OnListenerRemovedDelegate.BindUObject(this, &UAISense_SeenBy::OnListenerRemovedImpl);
	}
}

void UAISense_SeenBy::RegisterSource(AActor& SourceActors)
{
	TargetActors.Add(&SourceActors);
}

void UAISense_SeenBy::UnregisterSource(AActor& SourceActor)
{
	TargetActors.Remove(&SourceActor);
}

float UAISense_SeenBy::Update()
{
	Super::Update();

	for (auto& [TargetActor, QueryData] : TargetActors)
	{
		for (auto& [ID, Listener] : *GetListeners())
		{

			if (!Listener.HasSense(UAISense::GetSenseID(UAISense_SeenBy::StaticClass())))
			{
				continue;
			}
			bool IsTrigger = IsTrgger(TargetActor, Listener);
			FVector StartLocation = TargetActor->GetActorLocation();

			int FindIndex = QueryData.SeenListenerIDs.Find(ID);
			if(IsTrigger && FindIndex == INDEX_NONE)
			{
				QueryData.SeenListenerIDs.Add(ID);
				Listener.RegisterStimulus(TargetActor, FAIStimulus{ *this, 1.0, StartLocation, Listener.CachedLocation, FAIStimulus::SensingSucceeded });
			}
			else if(!IsTrigger && FindIndex != INDEX_NONE)
			{
				QueryData.SeenListenerIDs.RemoveAt(FindIndex);
				Listener.RegisterStimulus(TargetActor, FAIStimulus{ *this, 0.0, StartLocation, Listener.CachedLocation, FAIStimulus::SensingFailed });
			}
		}
	}

	return 0.0f;
}

void UAISense_SeenBy::OnNewListenerImpl(const FPerceptionListener& NewListener)
{
	UAIPerceptionComponent* NewListenerPtr = NewListener.Listener.Get();
	check(NewListenerPtr);
	const UAISenseConfig_SeenBy* SenseConfig = Cast<const UAISenseConfig_SeenBy>(NewListenerPtr->GetSenseConfig(GetSenseID()));
	check(SenseConfig);

	const FSeenSenseData SeenSenseData{ SenseConfig };
	LitenerData.Add(NewListener.GetListenerID(), SeenSenseData);
}

void UAISense_SeenBy::OnListenerUpdateImpl(const FPerceptionListener& UpdatedListener)
{
	FSeenSenseData* SeenSenseData = LitenerData.Find(UpdatedListener.GetListenerID());
	if(!SeenSenseData)
	{
		return;
	}
	UAIPerceptionComponent* NewListenerPtr = UpdatedListener.Listener.Get();
	check(NewListenerPtr);
	const UAISenseConfig_SeenBy* SenseConfig = Cast<const UAISenseConfig_SeenBy>(NewListenerPtr->GetSenseConfig(GetSenseID()));
	check(SenseConfig);
	
	*SeenSenseData = FSeenSenseData{ SenseConfig };
}

void UAISense_SeenBy::OnListenerRemovedImpl(const FPerceptionListener& RemovedListener)
{
	LitenerData.Remove(RemovedListener.GetListenerID());
}

bool UAISense_SeenBy::IsTrgger(AActor* TargetActor, const FPerceptionListener& Listener)
{
	APawn* Source = Cast<APawn>(TargetActor);
	if (!Source)
	{
		return false;
	}

	const AActor* ListenerBody = Listener.GetBodyActor();

	// 팀 식별 및 적대 관계 판단 (엔진 기본 솔버 활용)
	FGenericTeamId SourceTeamId = FGenericTeamId::GetTeamIdentifier(Source);
	FGenericTeamId ListenerTeamId = FGenericTeamId::GetTeamIdentifier(ListenerBody);
	ETeamAttitude::Type Attitude = ETeamAttitude::Neutral;
	if (SourceTeamId != FGenericTeamId::NoTeam && ListenerTeamId != FGenericTeamId::NoTeam)
	{
		Attitude = FGenericTeamId::GetAttitude(SourceTeamId, ListenerTeamId);
	}

	if (Attitude != ETeamAttitude::Hostile)
	{
		return false;
	}

	const FSeenSenseData& SeenSenseData = LitenerData[Listener.GetListenerID()];
	FVector StartLocation = Source->GetActorLocation();
	FVector Forward = Source->GetControlRotation().Vector();
	FVector TargetLocation = Listener.CachedLocation + SeenSenseData.SightStartOffset;



	
	// 시야각 (기존 값 유지)
	float PeripheralVisionAngleCos = FMath::Cos(FMath::DegreesToRadians(SeenSenseData.SightRadius * 0.5f));

	float NearClip = 0.0f;
	float FarClip = SeenSenseData.SightRange;
	bool bInSight = FAISystem::CheckIsTargetInSightCone(
		StartLocation,
		Forward,
		PeripheralVisionAngleCos,
		150.0f,
		FMath::Square(NearClip),
		FMath::Square(FarClip),
		TargetLocation
	);

	if (!bInSight)
	{
		return false;
	}

	bool bHit;
	FHitResult HitResult;

	const FCollisionQueryParams QueryParams = FCollisionQueryParams(TEXT("하"), true, ListenerBody);
	bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Listener.CachedLocation, StartLocation, ECC_Visibility, QueryParams, FCollisionResponseParams::DefaultResponseParam);


	return bHit && HitResult.GetActor() == TargetActor;
}

FSeenSenseData::FSeenSenseData(const UAISenseConfig_SeenBy* AISenseConfig_SeenBy)
{
	if (AISenseConfig_SeenBy)
	{
		SightRadius = AISenseConfig_SeenBy->SightRadius;
		SightRange = AISenseConfig_SeenBy->SightRange;
		SightStartOffset = AISenseConfig_SeenBy->SightStartOffset;
	}
}
