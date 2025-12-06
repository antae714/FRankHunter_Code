// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FieldSystem/FHDestructionFieldSystemActor.h"
#include "Field/FieldSystemObjects.h"
#include "Field/FieldSystemComponent.h"
#include "GeometryCollection/GeometryCollectionSimulationTypes.h"
#include "Components/SphereComponent.h"

void AFHDestructionFieldSystemActor::Enable(float LifeTime)
{

	// Initialize the DamageRadialFalloff
	{
		const float Magnitude{ DamageMagnitude };
		const float MinRange{ 0.0f };
		const float MaxRange{ 1.0f };
		const float Default{ 0.0f };
		const float Radius{ FieldRadius > 0.0f ? FieldRadius : AnchorSphere->GetScaledSphereRadius() };
		const FVector Position{ AnchorSphere->GetComponentLocation() };
		const EFieldFalloffType Falloff{ Field_FallOff_None };

		DamageRadialFalloff->SetRadialFalloff(Magnitude, MinRange, MaxRange, Default, Radius, Position, Falloff);
		ForceRadialFalloff->SetRadialFalloff(1.0f, MinRange, MaxRange, Default, Radius, Position, Falloff);
		DynamicRadialFalloff->SetRadialFalloff(1.0f, MinRange, MaxRange, Default, Radius, Position, Falloff);
	}

	{
		const float MinRange{ 0.0f };
		const float MaxRange{ 1.0f };
		const FTransform Transform{ AnchorSphere->GetComponentTransform() };

		ForceNoiseField->SetNoiseField(MinRange, MaxRange, Transform);
	}

	{
		const float Magnitude{ ForcesMagnitude };
		const FVector Position{ AnchorSphere->GetComponentLocation() };

		ForceDirection->SetRadialVector(Magnitude, Position);
	}

	{
		const float Magnitude{ 1.0f };

		ForceOperatorField->SetOperatorField(Magnitude, ForceNoiseField, ForceDirection, EFieldOperationType::Field_Multiply);
	}
	{
		ForceCullingField->SetCullingField(ForceRadialFalloff, ForceOperatorField, Field_Culling_Outside);
	}

	if (GetFieldSystemComponent())
	{
		bool Enabled{ true };
		UFieldSystemMetaData* MetaData{ nullptr };

		{
			EFieldPhysicsType Target{ EFieldPhysicsType::Field_ExternalClusterStrain };
			GetFieldSystemComponent()->AddPersistentField(Enabled, Target, MetaData, DamageRadialFalloff);
		}
		{
			EFieldPhysicsType Target{ EFieldPhysicsType::Field_LinearForce };
			GetFieldSystemComponent()->AddPersistentField(Enabled, Target, MetaData, ForceCullingField);
		}
	}

	GetWorld()->GetTimerManager().SetTimer(
		FieldDeadHandle,                            // 핸들
		this,                                   // 호출 대상
		&ThisClass::Disable,                  // 호출할 함수
		LifeTime,                                   // 3초 후 실행
		false                                   // 반복 X
	);
}

void AFHDestructionFieldSystemActor::Disable()
{
	GetFieldSystemComponent()->RemovePersistentFields();
}

AFHDestructionFieldSystemActor::AFHDestructionFieldSystemActor()
{
	DamageMagnitude = 20.0f;
	ForcesMagnitude = 20.0f;
	FieldRadius = 0.0f;

	AnchorSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AnchorSphere"));
	AnchorSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	AnchorSphere->InitSphereRadius(50.0f);
	AnchorSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AnchorSphere->SetCollisionProfileName(TEXT("NoCollision"));


	DamageRadialFalloff = CreateDefaultSubobject<URadialFalloff>(TEXT("DamageRadialFalloff"));
	ForceNoiseField = CreateDefaultSubobject<UNoiseField>(TEXT("ForceNoiseField"));
	ForceDirection = CreateDefaultSubobject<URadialVector>(TEXT("ForceDirection"));
	ForceOperatorField = CreateDefaultSubobject<UOperatorField>(TEXT("ForceOperatorField"));
	ForceRadialFalloff = CreateDefaultSubobject<URadialFalloff>(TEXT("ForceRadialFalloff"));
	ForceCullingField = CreateDefaultSubobject<UCullingField>(TEXT("ForceCullingField"));
	DynamicRadialFalloff = CreateDefaultSubobject<URadialFalloff>(TEXT("DynamicRadialFalloff"));
	DynamicUniformInteger = CreateDefaultSubobject<UUniformInteger>(TEXT("DynamicUniformInteger"));
	DynamicCullingField = CreateDefaultSubobject<UCullingField>(TEXT("DynamicCullingField"));

}

void AFHDestructionFieldSystemActor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}


