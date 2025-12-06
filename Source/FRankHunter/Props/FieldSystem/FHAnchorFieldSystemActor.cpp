// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FieldSystem/FHAnchorFieldSystemActor.h"
#include "Field/FieldSystemObjects.h"
#include "Field/FieldSystemComponent.h"
#include "GeometryCollection/GeometryCollectionSimulationTypes.h"
#include "Components/BoxComponent.h"

AFHAnchorFieldSystemActor::AFHAnchorFieldSystemActor()
{
	bIsIntialEnabled = true;

	AnchorBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AnchorBox"));
	AnchorBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	AnchorBox->InitBoxExtent(FVector{ 50.0f, 50.0f, 50.0f });
	AnchorBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AnchorBox->SetCollisionProfileName(TEXT("NoCollision"));

	UniformInteger = CreateDefaultSubobject<UUniformInteger>(TEXT("UniformInteger"));
	BoxFalloff = CreateDefaultSubobject<UBoxFalloff>(TEXT("BoxFalloff"));
	CullingField = CreateDefaultSubobject<UCullingField>(TEXT("CullingField"));
	Filter = CreateDefaultSubobject<UFieldSystemMetaDataFilter>(TEXT("Filter"));

	FieldFilterType = EFieldFilterType::Field_Filter_All;
	FieldObjectType = EFieldObjectType::Field_Object_All;
}

void AFHAnchorFieldSystemActor::Enable()
{
	// Initialize the BoxFalloff
	{
		const float Magnitude{ 1.0f };
		const float MinRange{ 1.0f };
		const float MaxRange{ 1.0f };
		const float Default{ 0.0f };
		const FTransform Transform{ AnchorBox->GetComponentTransform() };
		const EFieldFalloffType Falloff{ Field_FallOff_None };

		BoxFalloff->SetBoxFalloff(Magnitude, MinRange, MaxRange, Default, Transform, Falloff);
	}

	// Initialize the UniformInteger
	{
		EObjectStateTypeEnum Magnitude{ EObjectStateTypeEnum::Chaos_Object_Kinematic };
		UniformInteger->SetUniformInteger(static_cast<int32>(Magnitude));
	}

	// Initialize the CullingField
	{
		CullingField->SetCullingField(BoxFalloff, UniformInteger, Field_Culling_Outside);
	}

	// Add the CullingField to the FieldSystemComponent
	if (GetFieldSystemComponent())
	{
		Filter->SetMetaDataFilterType(FieldFilterType, FieldObjectType, EFieldPositionType::Field_Position_CenterOfMass);
		bool Enabled{ true };
		EFieldPhysicsType Target{ EFieldPhysicsType::Field_DynamicState };
		UFieldSystemMetaData* MetaData{ Filter };
		
		GetFieldSystemComponent()->AddPersistentField(Enabled, Target, MetaData, CullingField);
	}
}

void AFHAnchorFieldSystemActor::Disable()
{
	GetFieldSystemComponent()->RemovePersistentFields();

}

void AFHAnchorFieldSystemActor::BeginPlay()
{
	Super::BeginPlay();

	if (bIsIntialEnabled)
	{
		Enable();
	}
}

