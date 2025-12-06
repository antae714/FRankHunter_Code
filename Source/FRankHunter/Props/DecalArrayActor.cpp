// Copyright F Rank Hunter. All Rights Reserved.

#include "Props/DecalArrayActor.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/DecalComponent.h" // For UDecalComponent
#include "Delegates/DelegateCombinations.h" // For multicast delegate macros
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Net/NetworkProfiler.h"
#include "Net/Core/Trace/NetTrace.h"
#include "Player/FHPlayerBase.h"


// 엔진 버전에 따라 Ar.IsNetArchive() 존재하거나 Ar.ArIsNetArchive 플래그가 존재.
// 안전하게 조건 분기 + 캐스팅.
inline int32 GetCurrentBitPos(FArchive& Ar)
{
    // UE 버전에 따라 FNetBitWriter / FNetBitReader 헤더 포함 필요
    // #include "Net/Serialization/FNetBitWriter.h"
    // #include "Net/Serialization/FNetBitReader.h"

#if ENGINE_MAJOR_VERSION >= 5
    const bool bIsNet = Ar.IsNetArchive();
#else
    const bool bIsNet = Ar.ArIsNetArchive; // 4.x 에 있었던 플래그
#endif

    if (bIsNet)
    {
        if (Ar.IsSaving())
        {
            // Writer: 누적 기록된 비트 수
            return static_cast<FNetBitWriter&>(Ar).GetNumBits();
        }
        else if (Ar.IsLoading())
        {
            // Reader: 현재 읽기 포인터 비트 위치
            return static_cast<FNetBitReader&>(Ar).GetPosBits();
        }
    }

    // 비트 아카이브가 아니면 바이트 위치 * 8 (정밀 비트 단위 아님)
    return static_cast<int32>(Ar.Tell() * 8);
}


// ---- FDecalItem NetSerialize implementation ----
bool FDecalItem::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    int32 StartPos = GetCurrentBitPos(Ar);
    if (Ar.IsSaving())
    {
        FVector_NetQuantize100 QPos(Value);
        QPos.NetSerialize(Ar, Map, bOutSuccess);
        if (!bOutSuccess) return true;
        FVector_NetQuantizeNormal QNormal(Normal);
        QNormal.NetSerialize(Ar, Map, bOutSuccess);
        if (!bOutSuccess) return true;
        Ar << HitActor;
        Ar << HitComponentName;
        Ar << BoneName;
        Ar << DestroyTime;
        Ar << bActive;
        bOutSuccess = true;
    }
    else
    {
        FVector_NetQuantize100 QPos;
        QPos.NetSerialize(Ar, Map, bOutSuccess);
        if (!bOutSuccess) return true;
        Value = (FVector)QPos;
        FVector_NetQuantizeNormal QNormal;
        QNormal.NetSerialize(Ar, Map, bOutSuccess);
        if (!bOutSuccess) return true;
        Normal = (FVector)QNormal;
        Ar << HitActor;
        Ar << HitComponentName;
        Ar << BoneName;
        Ar << DestroyTime;
        Ar << bActive;
        bOutSuccess = true;
    }
    int32 EndPos = GetCurrentBitPos(Ar);

#if UE_NET_TRACE_ENABLED
    if (Ar.IsSaving())
    {
        FNetTraceCollector* Collector = GetTraceCollector(static_cast<FNetBitWriter&>(Ar));
        UE_NET_TRACE("FFastArraySerializerItem", Collector, StartPos, EndPos, ENetTraceVerbosity::Trace);
    }
#endif

    return true;
}

// ---- FDecalArray implementations ----
FDecalHandle FDecalArray::AddDecal(const FVector& Pos, const FVector& InNormal, AActor* HitActor, FName HitCompName, FName Bone, float DestroyTime)
{
    if (FreeIndices.Num() > 0)
    {
        const int32 Slot = FreeIndices.Pop();
        check(Items.IsValidIndex(Slot));
        FDecalItem& It = Items[Slot];
        It.Value = Pos;
        It.Normal = InNormal;
        It.HitActor = HitActor;
        It.HitComponentName = HitCompName;
        It.BoneName = Bone;
        It.DestroyTime = DestroyTime;
        It.bActive = true;
        if (!Generations.IsValidIndex(Slot))
        {
            Generations.SetNumZeroed(Slot + 1);
        }
        Generations[Slot] = Generations[Slot] + 1;
        It.Generation = Generations[Slot];
        MarkItemDirty(It);
        OnChangedValue.ExecuteIfBound(Slot);
        return FDecalHandle{ Slot, Generations[Slot] };
    }
    else
    {
        FDecalItem NewItem;
        NewItem.Value = Pos;
        NewItem.Normal = InNormal;
        NewItem.HitActor = HitActor;
        NewItem.HitComponentName = HitCompName;
        NewItem.BoneName = Bone;
        NewItem.DestroyTime = DestroyTime;
        NewItem.bActive = true;
        const int32 NewIndex = Items.Add(NewItem);
        if (!Generations.IsValidIndex(NewIndex))
        {
            Generations.SetNumZeroed(NewIndex + 1);
        }
        Generations[NewIndex] = 1;
        Items[NewIndex].Generation = 1;
        MarkItemDirty(Items[NewIndex]);
        OnChangedValue.ExecuteIfBound(NewIndex);
        return FDecalHandle{ NewIndex, 1 };
    }
}

void FDecalArray::DeactivateAt(int32 Index)
{
    if (!Items.IsValidIndex(Index)) return;
    FDecalItem& It = Items[Index];
    if (!It.bActive) return;
    It.bActive = false;
    MarkItemDirty(It);
    FreeIndices.Push(Index);
    OnChangedValue.ExecuteIfBound(Index);
}

bool FDecalArray::DeactivateHandle(const FDecalHandle& H)
{
    if (!H.IsValid()) return false;
    if (!Items.IsValidIndex(H.Index)) return false;
    if (!Generations.IsValidIndex(H.Index)) return false;
    if (Generations[H.Index] != H.Generation) return false;
    DeactivateAt(H.Index);
    return true;
}

void FDecalArray::CleanupInactive(int32 MaxRemovals)
{
    int32 Removed = 0;
    for (int32 i = Items.Num() - 1; i >= 0 && Removed < MaxRemovals; --i)
    {
        if (!Items[i].bActive)
        {
            Items.RemoveAt(i);
            Generations.RemoveAt(i);
            Removed++;
        }
    }
    if (Removed > 0)
    {
        RebuildFreeIndices();
    }
}

void FDecalArray::RebuildFreeIndices()
{
    FreeIndices.Empty();
    for (int32 i = 0; i < Items.Num(); ++i)
    {
        if (!Items[i].bActive)
        {
            FreeIndices.Add(i);
        }
    }
}

void FDecalArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 NewNum)
{
    for (int32 Idx : AddedIndices) { OnChangedValue.ExecuteIfBound(Idx); }
}
void FDecalArray::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 NewNum)
{
    for (int32 Idx : ChangedIndices) { OnChangedValue.ExecuteIfBound(Idx); }
}

// ================= Ability Implementation =================

UAddDecalAbility::UAddDecalAbility()
{
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UAddDecalAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    bool bIsSuccess = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
    if (!bIsSuccess)
    {
        return false;
    }

    return Cast<ADecalManagerActor>(GetSourceObject(Handle, ActorInfo)) != nullptr;
}

void UAddDecalAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    ADecalManagerActor* DecalActor = Cast<ADecalManagerActor>(GetSourceObject(Handle, ActorInfo));
    if (!DecalActor)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    const FGameplayAbilityTargetData* GameplayAbilityTargetData = TriggerEventData ? TriggerEventData->TargetData.Get(0) : nullptr;
    const FHitResult* Hit = GameplayAbilityTargetData ? GameplayAbilityTargetData->GetHitResult() : nullptr;
    if (!Hit)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    DecalActor->AddDecal_Implement(Hit->ImpactPoint, Hit->ImpactNormal, Hit->GetActor(), Hit->GetComponent(), Hit->BoneName);

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

ADecalManagerActor::ADecalManagerActor()
{
    // Initialize the RootComponent
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // Initialize replicated properties
    bReplicates = true;

    ManagerName = FName(TEXT("DecalManager"));
}

void ADecalManagerActor::BeginPlay()
{
    Super::BeginPlay();

    LocationsArray.OnChangedValue.BindUObject(this, &ThisClass::OnChangedLocation);

    if (OrganizeInterval > 0.f)
    {
        GetWorldTimerManager().SetTimer(OrganizeTimerHandle, this, &ADecalManagerActor::OrganizeData, OrganizeInterval, true);
    }
}

void ADecalManagerActor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
    Super::EndPlay(EndPlayReason);
}

void ADecalManagerActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADecalManagerActor, LocationsArray);
}

void ADecalManagerActor::StartControl(AActor* AbilityOwnerActor)
{
    UAbilitySystemComponent* ControllASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AbilityOwnerActor);
    FGameplayAbilitySpecHandle AddDecalAbilityHandle;

    if (ControllASC && HasAuthority())
    {
        FGameplayAbilitySpec GameplayAbilitySpec{ UAddDecalAbility::StaticClass(), 1, INDEX_NONE, this };

        AddDecalAbilityHandle = ControllASC->GiveAbility(GameplayAbilitySpec);
        ControllASCDecalMap.Add(ControllASC, AddDecalAbilityHandle);
    }
}

void ADecalManagerActor::AddDecal(const FHitResult& Hit, AActor* AbilityOwnerActor)
{
    UAbilitySystemComponent* ControllASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AbilityOwnerActor);
    if (!ControllASC) return;
    FGameplayAbilitySpecHandle* AddDecalAbilityHandlePTR = ControllASCDecalMap.Find(ControllASC);
    FGameplayAbilitySpecHandle AddDecalAbilityHandle;
    if (!AddDecalAbilityHandlePTR)
    {
        FGameplayAbilitySpec* GameplayAbilitySpec = ControllASC->FindAbilitySpecFromClass(UAddDecalAbility::StaticClass());

        AddDecalAbilityHandle = GameplayAbilitySpec ? GameplayAbilitySpec->Handle : FGameplayAbilitySpecHandle();
    }
    else
    {
        AddDecalAbilityHandle = *AddDecalAbilityHandlePTR;
    }

    if (!AddDecalAbilityHandle.IsValid())
    {
        return;
    }

    // 위치 정보 전달
    FGameplayEventData EventData;
    EventData.TargetData.Add(new FGameplayAbilityTargetData_SingleTargetHit(Hit));

    ControllASC->InternalTryActivateAbility(AddDecalAbilityHandle, FPredictionKey(), nullptr, nullptr, &EventData);
}

void ADecalManagerActor::EndControl(AActor* AbilityOwnerActor)
{
    UAbilitySystemComponent* ControllASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AbilityOwnerActor);
    FGameplayAbilitySpecHandle* AddDecalAbilityHandlePTR = ControllASCDecalMap.Find(ControllASC);
    if (AddDecalAbilityHandlePTR)
    {
        FGameplayAbilitySpecHandle AddDecalAbilityHandle = *AddDecalAbilityHandlePTR;
        ControllASC->ClearAbility(AddDecalAbilityHandle);
        ControllASCDecalMap.Remove(ControllASC);
    }
}

void ADecalManagerActor::AddDecal_Implement(FVector Location, FVector Normal, AActor* HitActor, UPrimitiveComponent* HitComponent, FName BoneName)
{
    AGameStateBase* State = GetWorld()->GetGameState();
    if (!State) return;
    float EndTime = DecalTime + State->GetServerWorldTimeSeconds();
    FName HitCompName = HitComponent ? HitComponent->GetFName() : NAME_None;

    if (AFHPlayerBase* player = Cast<AFHPlayerBase>(HitActor); IsValid(player))
    {
        return;
    }
    LocationsArray.AddDecal(Location, Normal, HitActor, HitCompName, BoneName, EndTime);
}

void ADecalManagerActor::OnChangedLocation(int32 index)
{
    if (!LocationsArray.Items.IsValidIndex(index))
    {
        return; // invalid
    }

    const FDecalItem& Item = LocationsArray.Items[index];
    if (!Item.bActive)
    {
        if (Decals.IsValidIndex(index))
        {
            if (UDecalComponent* Existing = Decals[index])
            {
                Existing->SetFadeOut(0.0f, Existing->FadeDuration, false);
            }
            Decals[index] = nullptr;
        }
        return;
    }

    if (!Decals.IsValidIndex(index))
    {
        Decals.SetNumZeroed(index + 1);
    }

    UDecalComponent* DecalComp = Decals[index];
    if (!IsValid(DecalComp))
    {
        DecalComp = NewObject<UDecalComponent>(this);
        if (DecalMaterial)
        {
            DecalComp->SetDecalMaterial(DecalMaterial);
        }
        USceneComponent* TargetComp = nullptr;
        if (Item.HitActor)
        {
            if (Item.HitComponentName != NAME_None)
            {
                TArray<UActorComponent*> AllComponents;
                Item.HitActor->GetComponents(AllComponents);
                for (UActorComponent* C : AllComponents)
                {
                    if (C && C->GetFName() == Item.HitComponentName)
                    {
                        TargetComp = Cast<USceneComponent>(C);
                        break;
                    }
                }
            }
            if (!TargetComp)
            {
                TargetComp = Item.HitActor->GetRootComponent();
            }
        }
        if (!TargetComp)
        {
            TargetComp = RootComponent.Get();
        }
        if (TargetComp)
        {
            DecalComp->AttachToComponent(TargetComp, FAttachmentTransformRules::KeepWorldTransform, Item.BoneName);
        }
        DecalComp->RegisterComponent();
    }

    DecalComp->SetWorldLocation(Item.Value);
    DecalComp->SetWorldRotation(Item.Normal.Rotation());
    // 회전만 랜덤
	DecalComp->AddRelativeRotation(FRotator(FMath::FRandRange(0.f, 360.f), 0.f, 0.f));
    // DecalComp->AddRelativeRotation()

    Decals[index] = DecalComp;
    OnDecalCreated.Broadcast(DecalComp, index);
}

void ADecalManagerActor::OrganizeData()
{
    AGameStateBase* State = GetWorld()->GetGameState();
    if (!State)
    {
        return;
    }
    float CurrentTime = State->GetServerWorldTimeSeconds();

    if (HasAuthority())
    {
        for (int32 i = 0; i < LocationsArray.Items.Num(); ++i)
        {
            FDecalItem& It = LocationsArray.Items[i];
            if (It.bActive && It.DestroyTime <= CurrentTime)
            {
                LocationsArray.DeactivateAt(i);
            }
        }

    }
}

float ADecalManagerActor::GetDecalRemainingTime(int32 Index) const
{
    if (!LocationsArray.Items.IsValidIndex(Index)) return 0.f;
    const FDecalItem& Item = LocationsArray.Items[Index];
    if (!Item.bActive) return 0.f;
    const UWorld* World = GetWorld();
    if (!World) return 0.f;
    const AGameStateBase* State = World->GetGameState();
    if (!State) return 0.f;
    const float Now = State->GetServerWorldTimeSeconds();
    return FMath::Max(0.f, Item.DestroyTime - Now);
}
