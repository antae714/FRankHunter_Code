// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset/FHRoomData.h"
#include "DungeonGenerator.h"
#include "FHDungeonDescriptor.generated.h"


USTRUCT()
struct FDoorSet
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UDoorType> DoorType;
	
	UPROPERTY(EditAnywhere)
	uint32 bUseDoorArray : 1{ false };
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseDoorArray == false", EditConditionHides))
	TSubclassOf<ADoor> Door;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseDoorArray == true", EditConditionHides))
	TArray<TSubclassOf<ADoor>> Doors;
};

USTRUCT()
struct FWallSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDoorType> TargetType;
	
	UPROPERTY(EditAnywhere)
	uint32 bUseRandomPick : 1{ false };

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseRandomPick == false", EditConditionHides))
	TSubclassOf<ADoor> Wall;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseRandomPick == true", EditConditionHides))
	TArray<TSubclassOf<ADoor>> Walls;
};

USTRUCT()
struct FSpecialRoomSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UFHRoomData> RoomData;

	UPROPERTY(EditAnywhere)
	uint32 bUseRandomRange : 1{ false };

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseRandomRange == false", EditConditionHides))
	uint32 RoomNum{ 1 };

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseRandomRange == true", EditConditionHides))
	uint32 MinRoomNum{ 0 };
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseRandomRange == true", EditConditionHides))
	uint32 MaxRoomNum{ 2 };

	uint32 FixedRoomNum{ 0 };
};

/*
여기서 따로 룸 타입을 정의하진 않고, FH_DA_DungeonRules에서 특정 타입에 대한 커스텀 룰을 만들 수 있는 방향으로
*/


USTRUCT()
struct FCreatureSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AFHCreatureBase> MonsterClass;
	
	UPROPERTY(EditAnywhere)
	uint32 PowerLevel{ 10 };

	UPROPERTY(EditAnywhere)
	float Weight{ 1.0f };
};

USTRUCT()
struct FMagicStoneSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AFHMagicStone> MagicStoneClass;
	// The value of each actor.
	UPROPERTY(EditAnywhere)
	uint32 Value = 0;
	// Affects the quantity and distribution.
	UPROPERTY(EditAnywhere)
	float weighting{ 1.0f };
	// Collision sphere's radius to test gap.
	UPROPERTY(EditAnywhere)
	float MinimumDistanceFromStaticStructures{ 50.0f };
	UPROPERTY(EditAnywhere)
	float MinimumDistanceFromOtherMagicStones{ 50.0f };


	UPROPERTY(EditAnywhere)
	uint32 bUseNumLimit : 1{ false };
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseNumLimit == true", EditConditionHides))
	uint32 NumLimit{ 1 };
};

USTRUCT()
struct FRandomSpawnPropSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> RandomSpawnPropActorClass;
	// The value of each actor.
	UPROPERTY(EditAnywhere)
	uint32 Value = 0;
	// Affects the quantity and distribution.
	UPROPERTY(EditAnywhere)
	float Weight{ 1.0f };
	// Collision sphere's radius to test gap.
	UPROPERTY(EditAnywhere)
	float MinimumDistanceFromOtherStructures{ 50.0f };
};



UCLASS()
class FRANKHUNTER_API UFHDungeonDescriptor : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "CommonSettings")
	FVector DungeonGridUnit{ 512, 512, 600 };
public:
	UPROPERTY(EditAnywhere, Category = "CommonSetting")
	float MaximumNavBakingDelayPerRoom{ 0.4f };

	UPROPERTY(EditAnywhere, Category = "Dungeon")
	FString GateType;

	UPROPERTY(EditAnywhere, Category = "Dungeon")
	float PowerLevelIncreaseCool{ 180.0f };
	UPROPERTY(EditAnywhere, Category = "Dungeon")
	float InitialPowerLevelIncreasingDelay{ 60.0f };
	
	// First room is not always necessary.
	UPROPERTY(EditAnywhere, Category = "Dungeon")
	uint32 bMustStartOnStartRoom : 1 { true };
	UPROPERTY(EditAnywhere, Category = "Dungeon", meta = (EditCondition = "bMustStartOnStartRoom == true", EditConditionHides))
	TObjectPtr<UFHRoomData> StartRoom{ nullptr };

	// Last room is not always necessary. If last room exists, generator try to create it for finalizing dungeon but not guaranted.
	UPROPERTY(EditAnywhere, Category = "Dungeon")
	uint32 bUseLastRoom : 1 { true };
	UPROPERTY(EditAnywhere, Category = "Dungeon", meta = (EditCondition = "bUseLastRoom == true", EditConditionHides))
	TObjectPtr<UFHRoomData> LastRoom{ nullptr };
	
	// There is no limit to the number of regular rooms.
	UPROPERTY(EditAnywhere, Category = "Dungeon")
	TArray<TObjectPtr<UFHRoomData>> RegularRooms;

	// Special room is not always necessary. 
	UPROPERTY(EditAnywhere, Category = "Dungeon")
	TArray<FSpecialRoomSet> SpecialRooms;


	// Default door type to use when door type is empty.
	UPROPERTY(EditAnywhere, Category = "Dungeon")
	TObjectPtr<UDoorType> DefaultDoorType;

	// Door list - Randomly select the matching type of door in the list.
	// [Warning!] An error occurs if no door matching type exists.
	UPROPERTY(EditAnywhere, Category = "Dungeon")
	TArray<FDoorSet> DoorSetting;

	UPROPERTY(EditAnywhere, Category = "Dungeon")
	TArray<FWallSet> WallSetting;



	// ------------- 게이트 선택 후 초기화할 때 지정 -------------
	// 게이트 초기화할 때 수치만 지정해놓고 실제 배치는 게이트 구조 완성된 다음에 실시함.
	//UPROPERTY(EditAnywhere, Category = "Dungeon")
	uint32 RoomNum; // Room number of the gate.
	//UPROPERTY(EditAnywhere, Category = "Dungeon")
	float MagicStoneValue; // Maximum value of magic stones spawning in the gate.
	//UPROPERTY(EditAnywhere, Category = "Dungeon")
	float MaxCreaturePower; // Maximum power of creatures spawning in the gate.
	uint32 DungeonCoreCount;
	uint32 RandomSpawnPropValue;


	
	// ------------- Monster --------------
	UPROPERTY(EditAnywhere, Category="Creature")
	uint32 SpawnCreatureMinimumRoomGap{ 3 };
	UPROPERTY(EditAnywhere, Category = "Creature")
	float TrySpawnCreatureInterval{ 0.5f };
	UPROPERTY(EditAnywhere, Category = "Creature")
	FVector CheckIsColinearWithPlayerRange{ 100.0f, 100.0f, 100.0f };
	UPROPERTY(EditAnywhere, Category = "Creature")
	TArray<FCreatureSet> SpawnableCreatures;



	// ------------- MagicStone --------------
	UPROPERTY(EditAnywhere, Category = "MagicStone")
	TArray<FMagicStoneSet> SpawnableMagicStones;
	UPROPERTY(EditAnywhere, Category = "MagicStone")
	float MagicStoneSpawnChanceByRoomSizeMult{ 1.0f };
	UPROPERTY(EditAnywhere, Category = "MagicStone")
	float MagicStoneSpawnChanceByDistanceMult{ 0.0f };



	// =========== Dungeon Core ============
	UPROPERTY(EditAnywhere, Category = "DungeonCore")
	TSubclassOf<class AFHCoreObject> CoreObjectClass;


	// === ETC ===
	UPROPERTY(EditAnywhere, Category = "RandomSpawnProps")
	TArray<FRandomSpawnPropSet> RandomSpawnProps;



	// ----------- For debugging ---------------
	UPROPERTY(EditAnywhere, Category = "Debugging")
	uint32 bIsTest : 1{ false };
	
	UPROPERTY(EditAnywhere, Category = "Debugging", meta = (EditCondition = "bIsTest == true", EditConditionHides))
	uint32 TestRoomNum;
	UPROPERTY(EditAnywhere, Category = "Debugging", meta = (EditCondition = "bIsTest == true", EditConditionHides))
	float TestMagicStoneAmount;
	UPROPERTY(EditAnywhere, Category = "Debugging", meta = (EditCondition = "bIsTest == true", EditConditionHides))
	float TestMaxCreaturePower;
	UPROPERTY(EditAnywhere, Category = "Debugging", meta = (EditCondition = "bIsTest == true", EditConditionHides))
	TSubclassOf<AActor> TestMagicStoneActor;



	// 나중에 미리 좋은 맵 시드값 알아두는 방식으로 할 거면 사용
	//UPROPERTY(EditAnywhere, Category = "Dungeon")
	//TArray<int32> FixedSeedList;
};
