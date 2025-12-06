// Copyright F Rank Hunter. All Rights Reserved.


#include "Creature/FHCreatureBase.h"
#include "Creature/CreatureAIController.h"
#include "GAS/FHAbilitySystemComponent.h"
#include "GAS/FHAttributeSet_Health.h"
#include "GAS/Attributes/FHAttributeSet_Movement.h"
#include "GAS/FHAttributeSet_Creature.h"
#include "GAS/FHGameplayTags.h"
#include "AIController.h"
#include "Component/FHCharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
// VFX
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraActor.h"
#include "GameplayAbilitiesModule.h"


AFHCreatureBase::AFHCreatureBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UFHCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UFHAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AbilitySystemComponent->DefaultStartingData.AddDefaulted_GetRef().Attributes = UFHAttributeSet_Health::StaticClass();
	AbilitySystemComponent->DefaultStartingData.AddDefaulted_GetRef().Attributes = UFHAttributeSet_Creature::StaticClass();
	AbilitySystemComponent->DefaultStartingData.AddDefaulted_GetRef().Attributes = UFHAttributeSet_Movement::StaticClass();

	AggroSetting.TriggerAggro = 10;
	AggroSetting.SightAggroWeight = 1.2;
	AggroSetting.HitAggroWeight = 1.3;
	AggroSetting.SightAgroIncreaseDelta = 10;
	AggroSetting.SightAgroDecreaseDelta = 10;

	TeamID = ETeamID::Creature;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AFHCreatureBase::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->AbilityEndedCallbacks.AddUObject(this, &ThisClass::OnAbilityEnded);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UFHAttributeSet_Health::GetHealthAttribute()).AddUObject(this, &ThisClass::OnDamaged);

	AbilitySystemComponent->RegisterGameplayTagEvent(GET_GAMEPLAY_TAG_CREATURE_STATE_DETECTED, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnDetectedTagChanged);

	OnChase(false);

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();

	if (MovementComp)
	{
		const float RandomWeight = FMath::RandRange(0.3f, 0.9f);

		MovementComp->AvoidanceWeight = RandomWeight;
	}
}

void AFHCreatureBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);

	if (StunEffectTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(StunEffectTimerHandle);
	}
}

void AFHCreatureBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UAbilitySystemComponent* AFHCreatureBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AFHCreatureBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
	}
}

void AFHCreatureBase::OnChase(bool bIsChase)
{
	if (bIsChase)
	{
		UFHCharacterMovementComponent* moveComp = Cast<UFHCharacterMovementComponent>(GetCharacterMovement());
		moveComp->StartSprinting();
	}
	else
	{
		UFHCharacterMovementComponent* moveComp = Cast<UFHCharacterMovementComponent>(GetCharacterMovement());
		moveComp->StopSprinting();
	}

	//GetCharacterMovement()->MaxWalkSpeed = bIsChase ? ChaseSpeed : MoveSpeed;
}

void AFHCreatureBase::OnDamaged(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.NewValue <= 0.0f)
	{
		OnDeath();
	}
}

void AFHCreatureBase::OnDeath()
{
	SetActorEnableCollision(false);
	
}

void AFHCreatureBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	SelectRandomAttackData();

	check(CreatureDataAsset);


	//AAIController* AIController = Cast<AAIController>(NewController);
	//if (AIController)
	//{
	//	UBlackboardComponent* AIBlackboard = AIController->GetBlackboardComponent();
	//	if (AIController->UseBlackboard(BlackBoardData, AIBlackboard))
	//	{
	//		bool RunResult = AIController->RunBehaviorTree(BehaviorTree);
	//		ensure(RunResult);
	//	}

	//	UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(AIController->GetBrainComponent());
	//	if (BTComp)
	//	{
	//		for (auto& data : TreeInitData)
	//		{
	//			BTComp->SetDynamicSubtree(data.BTTag, data.SubTree);

	//		}
	//	}

	//	SelectRandomAttackData();
	//	AIController->SetGenericTeamId((uint8)TeamID);
	//	

	//	const UFHAttributeSet_Creature* CreatureData = AbilitySystemComponent->GetSet<UFHAttributeSet_Creature>();
	//	if (AIController->GetAIPerceptionComponent())
	//	{
	//		if (CreatureData)
	//		{
	//			UAISenseConfig_Sight* AISenseConfig_Sight = AIController->GetAIPerceptionComponent()->GetSenseConfig<UAISenseConfig_Sight>();
	//			if (AISenseConfig_Sight)
	//			{
	//				AISenseConfig_Sight->PeripheralVisionAngleDegrees = CreatureData->GetSightAngle();
	//				AISenseConfig_Sight->LoseSightRadius = CreatureData->GetSightRange() * 2;
	//				AISenseConfig_Sight->SightRadius = CreatureData->GetSightRange();
	//			}

	//			UAISenseConfig_Hearing* AISenseConfig_Hearing = AIController->GetAIPerceptionComponent()->GetSenseConfig<UAISenseConfig_Hearing>();
	//			if (AISenseConfig_Hearing)
	//			{
	//				AISenseConfig_Hearing->HearingRange = CreatureData->GetHearingRange();
	//			}
	//		}
	//	}
	//}

	//ACreatureAIController* CreatureAIController = Cast<ACreatureAIController>(NewController);
	//if (CreatureAIController)
	//{
	//	CreatureAIController->InitAggroSetting(AggroSetting);
	//}
}


FGenericTeamId AFHCreatureBase::GetGenericTeamId() const
{
	return GetCreatureDataAsset() ? FGenericTeamId((uint8)GetCreatureDataAsset()->TeamID) : FGenericTeamId::NoTeam;
}

void AFHCreatureBase::OnAbilityEnded(UGameplayAbility* EndAbility)
{
	if (EndAbility->GetAssetTags().HasTag(GET_GAMEPLAY_TAG_CREATURE_ABILITY_ATTACK))
	{
		SelectRandomAttackData();
	}
}

void AFHCreatureBase::SelectRandomAttackData()
{
	LastestAttackIndex = FMath::RandRange(0, CreatureDataAsset->AttackDataTable.Num() - 1);


	ACreatureAIController* AIController = GetController<ACreatureAIController>();
	UBlackboardComponent* BlackboardComponent = AIController ? AIController->GetBlackboardComponent() : nullptr;
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsFloat(AIController->GetAttackRangeName(), GetAttackRange());
	}
}

UAnimMontage* AFHCreatureBase::GetAttackMontage() const 
{
	return CreatureDataAsset->AttackDataTable.IsValidIndex(LastestAttackIndex) ? CreatureDataAsset->AttackDataTable[LastestAttackIndex].AttackMontage : nullptr;
}

AActor* AFHCreatureBase::GetTargetActor() const
{
	ACreatureAIController* AIController = GetController<ACreatureAIController>();
	return AIController ? AIController->GetTargetActor() : nullptr;
}

AActor* AFHCreatureBase::GetCurrentTargetActor() const
{
	ACreatureAIController* AIController = GetController<ACreatureAIController>();
	return AIController ? AIController->GetTargetActor() : nullptr;
}

float AFHCreatureBase::GetAttackRange()
{
	return CreatureDataAsset->AttackDataTable.IsValidIndex(LastestAttackIndex) ? CreatureDataAsset->AttackDataTable[LastestAttackIndex].AttackRange : 0.0f;
}

float AFHCreatureBase::GetDoorOpenTime()
{
	const UFHAttributeSet_Creature* CreatureData = AbilitySystemComponent->GetSet<UFHAttributeSet_Creature>();
	return CreatureData ? CreatureData->GetDoorOpenTime() : 0.0f;
}

void AFHCreatureBase::ActivateBloodSplashEffect_Implementation(FVector Location, FRotator Rotation)
{
	if (!BloodSplashNiagara) return;

	Multicast_ActivateBloodSplashEffect(Location, Rotation);
}

void AFHCreatureBase::ActivateStunEffect_Implementation()
{
	if (!StunNiagaraActorClass) return;

	Multicast_ActivateStunEffect();
}

void AFHCreatureBase::Multicast_ActivateBloodSplashEffect_Implementation(FVector Location, FRotator Rotation)
{
	// A hit box of creature is too big to using effect origin of blood splash.
	//Location.Z -= 20.0f;
	
	FVector ClosestBoneLocation;
	GetMesh()->FindClosestBone(Location, &ClosestBoneLocation);

	Location = ClosestBoneLocation; 

	FActorSpawnParameters param{};
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ANiagaraActor* actor = GetWorld()->SpawnActor<ANiagaraActor>(ANiagaraActor::StaticClass(), Location, Rotation, param);
	actor->SetDestroyOnSystemFinish(true);

	actor->GetNiagaraComponent()->SetAsset(BloodSplashNiagara, true);
	actor->GetNiagaraComponent()->Activate(false);
}

void AFHCreatureBase::Multicast_ActivateStunEffect_Implementation()
{
	TArray<USkeletalMeshComponent*> Components;
	GetComponents<USkeletalMeshComponent>(Components);

	for (auto* mesh : Components)
	{
		mesh->SetRenderCustomDepth(true);
		mesh->SetCustomDepthStencilValue(77);
	}

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(GetActorLocation());
	SpawnTransform.SetRotation(FQuat::Identity);

	StunNiagaraActor = GetWorld()->SpawnActor<ANiagaraActor>(
		StunNiagaraActorClass,
		SpawnTransform
	);

	GetWorld()->GetTimerManager().SetTimer(
		StunEffectTimerHandle,
		FTimerDelegate::CreateLambda([this]()
	{
		TArray<USkeletalMeshComponent*> Components;
		GetComponents<USkeletalMeshComponent>(Components);

		for (auto* mesh : Components)
		{
			mesh->SetRenderCustomDepth(true);
		}
		StunNiagaraActor = nullptr;
	}), 3.0f, false);
}

void AFHCreatureBase::OnDetectedTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	const bool bIsDiscovered = NewCount > 0;

	TArray<USkeletalMeshComponent*> SkeletalMeshes;
	GetComponents<USkeletalMeshComponent>(SkeletalMeshes);
	for (USkeletalMeshComponent* SkeletalMesh : SkeletalMeshes)
	{
		if (SkeletalMesh)
		{
			SkeletalMesh->SetRenderCustomDepth(bIsDiscovered);
			if (bIsDiscovered)
			{
				SkeletalMesh->SetCustomDepthStencilValue(78);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Creature '%s' Detected state changed: %s"), *GetName(), (bIsDiscovered ? TEXT("ON") : TEXT("OFF")));

}
