// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FieldSystem/FHBoxFieldSystemActor.h"
#include "Field/FieldSystemObjects.h"
#include "Field/FieldSystemComponent.h"
#include "GeometryCollection/GeometryCollectionSimulationTypes.h"
#include "Components/BoxComponent.h"

AFHBoxFieldSystemActor::AFHBoxFieldSystemActor()
{
	PrimaryActorTick.bCanEverTick = true;

	bIsIntialEnabled = true;
	bIsTick = false;
	FieldPhysicsType = EFieldPhysicsType::Field_None;


	FallofBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AnchorBox"));
	FallofBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	FallofBox->InitBoxExtent(FVector{ 50.0f, 50.0f, 50.0f });
	FallofBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FallofBox->SetCollisionProfileName(TEXT("NoCollision"));

	BoxFalloff = CreateDefaultSubobject<UBoxFalloff>(TEXT("BoxFalloff"));
	CullingField = CreateDefaultSubobject<UCullingField>(TEXT("CullingField"));


}

void AFHBoxFieldSystemActor::Enable()
{
	if (!GetFieldSystemComponent())
	{
		return;
	}

	if (!bIsTick)
	{
		bool Enabled{ true };
		EFieldPhysicsType Target{ FieldPhysicsType };
		UFieldSystemMetaData* MetaData{ GetReturnMetaData()};
		UFieldNodeBase* Field = GetAddField();
		GetFieldSystemComponent()->AddPersistentField(Enabled, Target, MetaData, Field);
	}
	else
	{
		SetActorTickEnabled(true);
	}

}

void AFHBoxFieldSystemActor::Disable()
{
	if (!bIsTick)
	{
		GetFieldSystemComponent()->RemovePersistentFields();
	}
	else
	{
		GetFieldSystemComponent()->ResetFieldSystem();
		SetActorTickEnabled(false);
	}
}

void AFHBoxFieldSystemActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);

	if (bIsIntialEnabled)
	{
		Enable();
	}
}

void AFHBoxFieldSystemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bool Enabled{ true };
	EFieldPhysicsType Target{ FieldPhysicsType };
	UFieldSystemMetaData* MetaData{ GetReturnMetaData()};
	UFieldNodeBase* Field = GetAddField();
	GetFieldSystemComponent()->ApplyPhysicsField(Enabled, Target, MetaData, Field);
}

UFieldSystemMetaData* AFHBoxFieldSystemActor::GetReturnMetaData_Implementation()
{
	return nullptr;
}

UFieldNodeBase* AFHBoxFieldSystemActor::GetReturnValueField_Implementation()
{
	return nullptr;
}

UFieldNodeBase* AFHBoxFieldSystemActor::GetAddField()
{
	// Initialize the BoxFalloff
	{
		const float Magnitude{ 1.0f };
		const float MinRange{ 0.0f };
		const float MaxRange{ 1.0f };
		const float Default{ 0.0f };
		const FTransform Transform{ FallofBox->GetComponentTransform() };
		const EFieldFalloffType Falloff{ Field_FallOff_None };

		BoxFalloff->SetBoxFalloff(Magnitude, MinRange, MaxRange, Default, Transform, Falloff);
	}

	if (UFieldNodeBase* ReturnValueField = GetReturnValueField(); ReturnValueField)
	{
		CullingField->SetCullingField(BoxFalloff, ReturnValueField, Field_Culling_Outside);
		return CullingField;
	}
	else
	{
		return BoxFalloff;
	}
}
