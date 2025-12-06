// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/Actors/FHThrowItemActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Item/FHInventoryComponent.h"
#include "Item/FHItemThrowable.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Component/FHInteractableComponent.h"


// Sets default values
AFHThrowItemActor::AFHThrowItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetInterpolatedComponent(GetRootComponent());
	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &ThisClass::OnImpact);



	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	SetReplicateMovement(true);

}

// Called when the game starts or when spawned
void AFHThrowItemActor::BeginPlay()
{
	Super::BeginPlay();

	ASC->InitAbilityActorInfo(this, this);

	//StaticMeshComponent->SetVisibility(false);

	UFHInteractableComponent* interactable = GetComponentByClass<UFHInteractableComponent>();
	interactable->bIsHighlightable = false;
}

void AFHThrowItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ThisClass, LaunchVelocity, COND_InitialOnly);
}

void AFHThrowItemActor::OnInventoryChangedFunction(USiInventoryComponent* Inventory, int32 Index)
{
	Super::OnInventoryChangedFunction(Inventory, Index);

	UFHItemThrowable* item = Inventory->GetItemInstance<UFHItemThrowable>(Index);
	if (item)
	{
		LaunchVelocity = item->LaunchVelocity;
		OnRep_LaunchVelocity();
	}
}

void AFHThrowItemActor::OnImpactEnd(UGameplayAbility* EndAbility)
{
	if (ASC && LatestImpactAbilitySpecHandle.IsValid())
	{
		const FGameplayAbilitySpec* FoundSpec = ASC->FindAbilitySpecFromHandle(LatestImpactAbilitySpecHandle);
		if (FoundSpec && FoundSpec->Ability == EndAbility->GetClass()->GetDefaultObject())
		{
			Destroy(true);
		}
	}

}

void AFHThrowItemActor::OnImpact(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (!HasAuthority())
	{
		return;
	}
	if (LatestImpactAbilitySpecHandle.IsValid())
	{
		return;
	}


	StaticMeshComponent->SetSimulatePhysics(true);

	for (size_t i = 0; i < InventoryComponent->GetItemNum(); i++)
	{
		UFHItemThrowable* throwableItem = InventoryComponent->GetItemInstance<UFHItemThrowable>(i);
		if (throwableItem && throwableItem->ImpactAbility)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(throwableItem->ImpactAbility);
			if (ASC)
			{
				ASC->AbilityEndedCallbacks.AddUObject(this, &ThisClass::OnImpactEnd);
				
				FGameplayAbilitySpecHandle SpecHandle = ASC->GiveAbility(AbilitySpec);
				if (SpecHandle.IsValid())
				{
					LatestImpactAbilitySpecHandle = SpecHandle;
					ASC->TryActivateAbility(SpecHandle);
				}
			}

		}
	}
}

void AFHThrowItemActor::OnRep_LaunchVelocity()
{
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->SetVelocityInLocalSpace(LaunchVelocity);
		//ProjectileMovementComponent->Velocity = GetActorTransform().TransformVectorNoScale(LaunchVelocity);

		StaticMeshComponent->SetVisibility(true);
	}
	else
	{
		Destroy();
	}
}

