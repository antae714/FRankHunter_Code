// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHCoreObject.h"
#include "Core/FHGateGameModeBase.h"
#include "Component/FHInteractableComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraShakeSourceActor.h"
#include "Camera/CameraShakeSourceComponent.h"
#include "Core/FHGateGameModeBase.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "GAS/FHAbilitySystemComponent.h"
#include "GAS/FHAttributeSet_Health.h"
#include "Field/FieldSystemActor.h"
#include "Components/SphereComponent.h"


AFHCoreObject::AFHCoreObject()
{
	PrimaryActorTick.bCanEverTick = true;

    SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
    SetRootComponent(SphereCollision);

    CoreGeometry = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("CoreGeometry"));
    CoreGeometry->SetupAttachment(SphereCollision);

    CoreDestructionField = CreateDefaultSubobject<UChildActorComponent>(TEXT("CoreDestructionField"));
    CoreDestructionField->SetupAttachment(SphereCollision);

    AbilitySystemComponent = CreateDefaultSubobject<UFHAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
    HealthAttributeSet = CreateDefaultSubobject<UFHAttributeSet_Health>(TEXT("HealthAttributeSet"));

    // for test
    // InteractableComponent = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComp"));
    // InteractableComponent->OnInteractServer.AddDynamic(this, &AFHCoreObject::OnInteract_Impl);
}

void AFHCoreObject::BeginPlay()
{
	Super::BeginPlay();
    initialLocation = GetActorLocation();
    HealthAttributeSet->SetMaxHealth(1.0f);
    HealthAttributeSet->SetHealth(1.0f);
}

void AFHCoreObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (bIsFloating)
    {
	    UpdateFloating(DeltaTime);
    }
}

UAbilitySystemComponent* AFHCoreObject::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AFHCoreObject::UpdateFloating(float DeltaTime)
{ 
    FVector NewLocation = initialLocation;

    elapsedTime += DeltaTime;
    float offsetZ = FMath::Sin(elapsedTime * FloatSpeed) * FloatRange;
    NewLocation.Z += offsetZ;

    SetActorLocation(NewLocation);
}

//void AFHCoreObject::OnInteract_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction)
//{
//    AFHGateGameModeBase* GateMode = Cast<AFHGateGameModeBase>(GetWorld()->GetAuthGameMode());
//    if (!GateMode) return;
//
//    GateMode->CoreDestroyed();
//
//    // ´ëÃæ VFX
//
//    ShakeCamera();
//
//    Destroy();
//}

void AFHCoreObject::ShakeCamera()
{
    if (CameraShakeBaseOnDestroy == nullptr) return;

    ACameraShakeSourceActor* Shaker = GetWorld()->SpawnActor<ACameraShakeSourceActor>(GetActorLocation(), GetActorRotation());
    UCameraShakeSourceComponent* ShakeComp = Shaker->GetCameraShakeSourceComponent();
    
	FCameraShakeSourceComponentStartParams param;
	param.ShakeClass = CameraShakeBaseOnDestroy;
	param.Scale = 1.0f;
	param.PlaySpace = ECameraShakePlaySpace::CameraLocal;
	param.UserPlaySpaceRot = FRotator{ 0, 0, 0 };
    ShakeComp->StartCameraShake(param);
}