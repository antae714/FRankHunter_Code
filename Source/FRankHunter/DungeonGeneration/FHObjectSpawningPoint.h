// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGeneration/FHGateDefines.h"
#include "FHObjectSpawningPoint.generated.h"


UENUM(BlueprintType)
enum class ESpawningType : uint8
{
	None 		UMETA(DisplayName = "None"),
	TypeA 		UMETA(DisplayName = "TypeA"),
	TypeB 		UMETA(DisplayName = "TypeB"),
	TypeC 		UMETA(DisplayName = "TypeC")
};


UCLASS()
class FRANKHUNTER_API AFHObjectSpawningPoint : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UArrowComponent> ArrowComponent;

	UPROPERTY()
	TObjectPtr<class UChildActorComponent> PreviewActor;

	int32 previewIdx{ 0 };
	
	UFUNCTION(CallInEditor)
	void HidePreviewActor();
	UFUNCTION(CallInEditor)
	void ShowPreviewActor();

	// 트랜스폼이 변할 때도 호출됨
	//virtual void OnConstruction(const FTransform& Transform) override;



	UPROPERTY(EditAnywhere, Category=Dungeon)
	uint32 bUseDifferentPresetByGateRank : 1{ false };
	
	UPROPERTY(EditAnywhere, Category = Dungeon, meta = (EditCondition = "bUseDifferentPresetByGateRank == false", EditConditionHides))
	TObjectPtr<class UFHSpawningPointDescriptor> SpawningPointDescriptor;

	UPROPERTY(EditAnywhere, Category = Dungeon, meta = (EditCondition = "bUseDifferentPresetByGateRank == true", EditConditionHides))
	TMap<EGateRank, TObjectPtr<class UFHSpawningPointDescriptor>> SpawningPointDescriptorByGateRank;



public:	
	AFHObjectSpawningPoint();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;



protected:
	void SpawnObject();
};
