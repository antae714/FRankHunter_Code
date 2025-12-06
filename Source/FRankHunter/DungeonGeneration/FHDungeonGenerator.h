// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "DungeonGenerator.h"
#include "DungeonGeneration/FHGateDefines.h"
#include "DungeonGeneration/FHDungeonStructs.h"
#include "FHDungeonGenerator.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadRateChanged);
static class AFHDungeonGenerator* CurrentGenerator;


enum class EGateRank : uint8;

USTRUCT()
struct FSpawningCounter
{
	GENERATED_BODY()

	FString Name;
	uint32 Count;
	uint32 MaxCount;

	bool IsSpawnable()
	{
		return Count < MaxCount;
	}
};

USTRUCT()
struct FGateDescriptorPerRankSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EGateRank Rank = EGateRank::Invalid;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UFHDungeonDescriptor> Descriptor;
};

USTRUCT()
struct FGateThemeContainer
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FString Name;

	UPROPERTY(EditAnywhere)
	TArray<FGateDescriptorPerRankSet> Ranks;
};

// Room ID == GetAllRooms() index

UCLASS()
class FRANKHUNTER_API AFHDungeonGenerator : public ADungeonGenerator
{
	GENERATED_BODY()
	


	UPROPERTY()
	uint8 bEnabled : 1{ false };
public:
	UPROPERTY(EditAnywhere, Category = "Dungeon")
	TArray<FGateThemeContainer> GateDescriptors;

	UPROPERTY()
	TObjectPtr<class UFHDungeonDescriptor> SelectedDescriptor;

	int32 tryCount{ 0 };


protected:
	// 한 번 스폰하고 죽는 SpawningPoint와 달리 얘는 게임 내내 사용되므로 지우면 안 됨
	/*TArray<class ATaggedPoint*> TaggedPoints;
	TArray<class ATaggedPoint*> TaggedPoints_SpawnCreature;
	TArray<class ATaggedPoint*> TaggedPoints_SpawnMagicStone;
	TArray<class ATaggedPoint*> TaggedPoints_SpawnCore;*/

public:
	AFHDungeonGenerator();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void GenerateGate();

	void SettingUpGate(FString GateThemeName, EGateRank GateRank, struct FFHGateSpecificationTableRow* GateSpecification);

	virtual URoomData* ChooseFirstRoomData_Implementation() override;
	virtual URoomData* ChooseNextRoomData_Implementation(const URoomData* CurrentRoom, const TScriptInterface<IReadOnlyRoom>& CurrentRoomInstance, const FDoorDef& DoorData, int& DoorIndex) override;
	virtual bool ContinueToAddRoom_Implementation() override;
	virtual bool IsValidDungeon_Implementation() override;
	virtual TSubclassOf<ADoor> ChooseDoor_Implementation(const URoomData* CurrentRoom, const URoom* CurrentRoomInstance, const URoomData* NextRoom, const URoom* NextRoomInstance, const UDoorType* DoorType, bool& Flipped, EDoorTag RoomATag, EDoorTag RoomBTag) override;
	virtual bool CreateDungeon_Implementation() override;

	UFUNCTION(NetMulticast, Reliable)
	void UnloadDungeon();
	void UnloadDungeon_Implementation();


	// instance loading rate
protected:
	bool bCheckLoadingRate{ false };
	uint32 RoomsToCheckNum{ 0 };

	UPROPERTY()
	TArray<class URoom*> RoomsToCheckLoading;
	float loadingRate{ 0.0f };
public:
	float GetLoadingRate() { return loadingRate; }
	FOnLoadRateChanged OnLoadRateChangedDelegate;

	// OnPreGeneration -> LoopEnter: OnGenerationInit(OnDungeonGenerationStart) -> OnPostInitialization(OnDungeonGraphSet) -> LoopExit: OnPostGeneration(OnDungeonGenerationDone) -> Unlock NavBuildingLock -> OnNavMeshGeneratedInGeneratedDungeon(GateGameMode->GeneratedCompleted)
	bool bIsCreatedSuccessfully{ false };
	FTimerHandle WaitNavMeshBakingTimer;

	float LoadingGateInsuranceTime{ 1.0f };
	FTimerHandle LoadingInsuranceTimer;

	UFUNCTION()
	void OnDungeonGenerationStart();

	UFUNCTION()
	void OnDungeonGraphSet();

	UFUNCTION()
	void OnDungeonGenerationDone();
	
	UFUNCTION()
	void OnWaitNavMeshBakingTimerDone();
	bool bIsFirstNavMeshUpdate{ true };
	UFUNCTION()
	void OnNavMeshGeneratedInDungeon(class ANavigationData* NavData);

	UFUNCTION(NetMulticast, Reliable)
	void ConstructMinimap();
	void ConstructMinimap_Implementation();


	// ======== spawn creature ==========
public:
	UPROPERTY()
	uint32 bTrySpawnCreature : 1{ false };

	UFUNCTION()
	void TurnOnSpawnCreatureFlag();
protected:
	float elapsedTimeAfterLastTryingOfSpawingCreature{ 0.0f };

	bool TrySpawnCreature();
	bool HasChanceToBeDetectedByPlayer(FVector TargetLocation);
	bool IsColinear(FVector PlayerLocation, FVector TargetLocation);
	// ======== spawn creature ==========



	UFUNCTION()
	void Event_OnRoomAdded(const URoomData* Room, const TScriptInterface<IReadOnlyRoom>& RoomInstance);
	UFUNCTION()
	void Event_OnFailedToAddRoom(const URoomData* Room, const FDoorDef& Door);


protected:
	TArray<TPair<TObjectPtr<class UFHRoomData>, uint32>> SpecialRoomOrder;
	
	UPROPERTY()
	TArray<TObjectPtr<class UFHRoomData>> SpecialRoomReady;

	// 코어룸 설치 시도시 플래그 켜기 -> 성공시에만 OnRoomAdded에서 CoreRoomReady Pop
	bool bTryToPlaceCoreRoomBefore{ false };
	uint32 AddedRoomIndexOfCoreRoomReady{ 0 };

	// == minimap ==
	int32 DungeonLowestFloor{ 9999 };
	int32 DungeonHighestFloor{ -9999 };
	TArray<FMinimapRoomSpec> ConstructRoomSpecs();

	// == distance rule ==
	TMap<class UFHRoomData*, TArray<FVector>> positionsOfPlacedRoom;

	void RemovePreventSpawnOnlyNavVolumes();
	// SpawningPoint 사용
	void SpawnPropsFromSpawningPoints();
	void SpawnMagicStonesRandomly();
	bool TrySpawnMagicStoneInDaRoom(class UNavigationSystemV1* navSys, URoom* room, const struct FMagicStoneSet& magicStoneSet);
	void SpawnRandomPropsOnNavMesh();

	FTimerHandle timerHandle;

	void TurnOnDynamicNavMesh();

	bool CheckDoorDirectionIfYouNeed(const EDoorDirection dir1, const EDoorDirection dir2);
	bool IsRoomAddable(const class UFHRoomData* prevRoom, const TScriptInterface<IReadOnlyRoom>& CurrentRoomInstance, const class UFHRoomData* targetRoom, const FDoorDef& DoorData);
	void PickCoreRoomOrder();
	TArray<URoomData*> MakeCandidates(const class UFHRoomData* prevRoom, const TScriptInterface<IReadOnlyRoom>& PrevRoomInstance, const FDoorDef& DoorData, OUT TArray<float>& roomWeights);
	bool IsLastRoomPlacing();
	int32 GetCompatibleRandomDoorIndex(const class URoomData* targetRoom, const FDoorDef& DoorData);


	// N번 내로 RootRoom에서 도달할 수 있는 모든 룸 구하기 (RootRoom과 자기자신도 개수에 포함)
	TSet<class URoom*> GetAllRoomsInSuchDistance(int64 RootRoomID, uint32 RoomDistance);
	TSet<class URoom*> GetAllRoomsInSuchDistance(class URoom* RootRoom, uint32 RoomDistance);


	// N개의 방만큼 떨어진 거리에 있는 모든 방 구하기
	TSet<class URoom*> GetAllRoomsAwaySuchDistance(int64 RootRoomID, uint32 RoomDistance);
	TSet<class URoom*> GetAllRoomsAwaySuchDistance(class URoom* RootRoom, uint32 RoomDistance);


	// 던전 생성 -> 플레이어 스타트 위치로 텔포 -> 그 때부터 갱신




	// 플레이어가 룸에 들어가거나 나갔다는 정보 처리
public:
	UFUNCTION()
	void Event_PlayerEnterTo(int64 RoomEnter);
	UFUNCTION()
	void Event_PlayerExitFrom(int64 RoomExit);



protected:
	bool GetRandomLocationOfRoom(OUT FVector& Result, class UNavigationSystemV1* navSys, URoom* TargetRoom, int32 MaximumTry = 100);
	bool GetRandomLocationOfRoom(OUT FVector& Result, class UNavigationSystemV1* navSys, URoom* TargetRoom, FCollisionObjectQueryParams AvoidPreset, float AvoidSphereRadius, int32 MaximumTry = 100, float AvoidEpsilon = 1.0f);



public:
	FVector GetRandomRoomLocation(FVector TeleportTargetPosition, float MinimumDistanceFromTarget = 100.0f);

	FTimerHandle AssholdTimer;

private:

	UPROPERTY()
	TArray<class AFHDoor*> DoorsCache;
};
