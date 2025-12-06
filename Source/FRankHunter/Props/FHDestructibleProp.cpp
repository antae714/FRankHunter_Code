// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHDestructibleProp.h"
#include "GAS/FHAbilitySystemComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "GAS/FHAttributeSet_Health.h"
#include "Props/FieldSystem/FHDestructionFieldSystemActor.h"
#include "Net/UnrealNetwork.h"
#include "GeometryCollectionProxyData.h"

// Sets default values
AFHDestructibleProp::AFHDestructibleProp()
{
	bReplicates = true;
	RootPrimitiveComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootPrimitiveComponent"));
	SetRootComponent(RootPrimitiveComponent);

	AbilitySystem = CreateDefaultSubobject<UFHAbilitySystemComponent>(TEXT("FHAbilitySystem"));
	HealthAttributeSet = CreateDefaultSubobject<UFHAttributeSet_Health>(TEXT("HealthAttributeSet"));

	GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollectionComponent"));
	GeometryCollectionComponent->DamageThreshold.SetNum(3);
	GeometryCollectionComponent->DamageThreshold[0] = 3;
	GeometryCollectionComponent->DamageThreshold[1] = 2;
	GeometryCollectionComponent->DamageThreshold[2] = 1;
	GeometryCollectionComponent->bEnableDamageFromCollision = false;
	GeometryCollectionComponent->SetupAttachment(GetRootComponent());

	Health = 1.0f;
	HpPercent.SetNum(1);
	HpPercent[0] = 0.0f;
}

void AFHDestructibleProp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsDestroy);
}

UAbilitySystemComponent* AFHDestructibleProp::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void AFHDestructibleProp::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystem)
	{
		AbilitySystem->InitAbilityActorInfo(this, this);
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(UFHAttributeSet_Health::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChangedData);
	}

	if (HasAuthority() && HealthAttributeSet)
	{
		HealthAttributeSet->SetMaxHealth(Health);
		HealthAttributeSet->SetHealth(Health);
	}

}

void AFHDestructibleProp::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GeometryCollectionComponent)
	{
		GeometryCollectionComponent->SetSimulatePhysics(false);
		GeometryCollectionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	Super::EndPlay(EndPlayReason);
}


void AFHDestructibleProp::OnHealthChangedData(const FOnAttributeChangeData& ChangeData)
{
	if (HpPercent.IsValidIndex(CurrentIndex))
	{
		while (ChangeData.NewValue / HealthAttributeSet->GetMaxHealth() * 100 <= HpPercent[CurrentIndex])
		{
			if (ChildDestructionPoints.IsValidIndex(CurrentIndex))
			{
				AActor* childActor = ChildDestructionPoints[CurrentIndex]->GetChildActor();
				AFHDestructionFieldSystemActor* FHDestructionFieldSystemActor = Cast<AFHDestructionFieldSystemActor>(childActor);
				if (FHDestructionFieldSystemActor)
				{
					FHDestructionFieldSystemActor->Enable();
				}
			}
			OnHealthChanged(ChangeData.NewValue);

			++CurrentIndex;
			if (!HpPercent.IsValidIndex(CurrentIndex))
			{
				break;
			}
		}
	}
	if (!bIsDestroy && ChangeData.NewValue <= 0)
	{
		Server_Destroy();
	}

}


void AFHDestructibleProp::OnRep_IsDestroy()
{
	K2_OnDestroy();
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(GetRootComponent()); PrimitiveComponent)
	{
		PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (GeometryCollectionComponent)
	{
		//GeometryCollectionComponent->CrumbleActiveClusters();

		/*GetWorldTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateLambda([this]()
		{
			if (IsValid(GeometryCollectionComponent))
			{
				GeometryCollectionComponent->CrumbleActiveClusters();

			}
		})
			);*/

		TWeakObjectPtr<UGeometryCollectionComponent> WeakGeometryCollection = GeometryCollectionComponent;
		GetWorldTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateLambda([WeakGeometryCollection]()
		{
			if (WeakGeometryCollection.IsValid())
			{
				WeakGeometryCollection->CrumbleActiveClusters();
			}
		})
		);
	}
}

void AFHDestructibleProp::Server_Destroy_Implementation()
{
	if (!bIsDestroy)
	{
		bIsDestroy = true;
		OnRep_IsDestroy();
	}
}


