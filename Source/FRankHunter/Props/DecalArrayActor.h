// Copyright F Rank Hunter. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Containers/List.h" // For TList used indirectly by base headers
#include "GameplayTagContainer.h"
#include "Core/ManagerActor.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Abilities/GameplayAbility.h"
#include "Containers/ArrayView.h"
#include "DecalArrayActor.generated.h"

class UMaterialInterface;
class UDecalComponent;
class UPrimitiveComponent;
class UAbilitySystemComponent;
class AActor;

// 생성된 데칼 컴포넌트를 블루프린트로 넘겨주는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDecalCreated, UDecalComponent*, DecalComponent, int32, Index);

// ---------------- Loose Decal Array Structures ----------------

USTRUCT(BlueprintType)
struct FDecalHandle
{
    GENERATED_BODY()

    UPROPERTY()
    int32 Index = INDEX_NONE;

    UPROPERTY()
    uint32 Generation = 0;

    bool IsValid() const { return Index != INDEX_NONE; }
};

USTRUCT()
struct FDecalItem : public FFastArraySerializerItem
{
    GENERATED_BODY()

    FDecalItem()
        : Value(FVector::ZeroVector)
        , Normal(FVector::UpVector)
        , HitActor(nullptr)
        , HitComponentName(NAME_None)
        , BoneName(NAME_None)
        , DestroyTime(0.f)
        , bActive(false)
        , Generation(0)
    {}

    UPROPERTY()
    FVector Value;

    UPROPERTY()
    FVector Normal;

    UPROPERTY()
    TObjectPtr<AActor> HitActor;

    UPROPERTY()
    FName HitComponentName;

    UPROPERTY()
    FName BoneName;

    UPROPERTY()
    float DestroyTime;

    UPROPERTY()
    bool bActive;

    uint32 Generation; // local only

    bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FDecalItem> : public TStructOpsTypeTraitsBase2<FDecalItem>
{
    enum { WithNetSerializer = true };
};

USTRUCT()
struct FDecalArray : public FFastArraySerializer
{
    GENERATED_BODY()

    DECLARE_DELEGATE_OneParam(FOnChangedValue, int32);

    FOnChangedValue OnChangedValue;

    UPROPERTY()
    TArray<FDecalItem> Items;

    TArray<int32> FreeIndices;
    TArray<uint32> Generations;

    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
    {
        return FFastArraySerializer::FastArrayDeltaSerialize<FDecalItem, FDecalArray>(Items, DeltaParms, *this);
    }

    FDecalHandle AddDecal(const FVector& Pos, const FVector& InNormal, AActor* HitActor, FName HitCompName, FName Bone, float DestroyTime);
    void DeactivateAt(int32 Index);
    bool DeactivateHandle(const FDecalHandle& H);
    void CleanupInactive(int32 MaxRemovals = 64);
    void RebuildFreeIndices();

    void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 OldNum) {}
    void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 NewNum);
    void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 NewNum);
};

template<>
struct TStructOpsTypeTraits<FDecalArray> : public TStructOpsTypeTraitsBase2<FDecalArray>
{
    enum { WithNetDeltaSerializer = true };
};

// ---------------- End Structures ----------------

UCLASS()
class FRANKHUNTER_API UAddDecalAbility : public UGameplayAbility
{
    GENERATED_BODY()
public:
    UAddDecalAbility();

    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};

//추가 O(1)
//삭제 O(1)
UCLASS()
class FRANKHUNTER_API ADecalManagerActor : public AManagerActor
{
    GENERATED_BODY()
public:
    ADecalManagerActor();

    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditAnywhere, Category = "Decal")
    FName ManagerName;

    virtual FName GetManagerName() const override { return ManagerName; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
    TObjectPtr<UMaterialInterface> DecalMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
    float DecalTime;

    // Interval (seconds) to run OrganizeData periodically
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal|Cleanup", meta=(ClampMin="0.01"))
    float OrganizeInterval = 0.25f;

    UPROPERTY(BlueprintAssignable, Category="Decal")
    FOnDecalCreated OnDecalCreated;

    UPROPERTY(Transient)
    TMap<UAbilitySystemComponent*, FGameplayAbilitySpecHandle> ControllASCDecalMap;

    UFUNCTION(BlueprintCallable)
    void StartControl(AActor* AbilityOwnerActor);

    UFUNCTION(BlueprintCallable)
    void AddDecal(const FHitResult& Hit, AActor* AbilityOwnerActor);

    UFUNCTION(BlueprintCallable)
    void EndControl(AActor* AbilityOwnerActor);

    void AddDecal_Implement(FVector Location, FVector Normal, AActor* HitActor, UPrimitiveComponent* HitComponent, FName BoneName);

    // 남은 시간(초). 인덱스가 잘못되거나 비활성/시간 정보 없음 -> 0
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Decal")
    float GetDecalRemainingTime(int32 Index) const;
protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UPROPERTY(Replicated, VisibleAnywhere, Category = "Decal")
    FDecalArray LocationsArray;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UDecalComponent>> Decals;

    // timer handle for periodic cleanup
    FTimerHandle OrganizeTimerHandle;

    void OnChangedLocation(int32 index);
    void OrganizeData();
};
