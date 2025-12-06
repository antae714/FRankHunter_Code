// Copyright F Rank Hunter. All Rights Reserved.


#include "DungeonGeneration/FHDungeonGenerator.h"
#include "ProceduralDungeonUtils.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "FRankHunter.h"
#include "Kismet/GameplayStatics.h"
#include "DungeonGeneration/FHObjectSpawningPoint.h"
#include "DungeonGeneration/FHSpawningPointDescriptor.h"
#include "DungeonGeneration/FHDungeonDescriptor.h"
#include "DungeonGeneration/FHNavModifierVolume.h"
#include "Props/FHMagicStone.h"
#include "DungeonGraph.h"
#include "Room.h"
#include "Door.h"
#include "DataAsset/FHRoomData.h"
#include "Player/FHPlayerBase.h"
#include "Core/FHGateGameModeBase.h"
#include "Core/FHGateGameStateBase.h"
#include "Core/FRankHunterTypes.h"
#include "Core/FHPlayerStateBase.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Creature/FHCreatureBase.h"
#include "Data/FHGateSpecificationTable.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "Blueprint/UserWidget.h"
#include "UI/FHMinimapBase.h"
#include "Core/FHPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Props/FHCoreObject.h"
#include "Props/FHMagicStone.h"
#include "Props/FHDoor.h"
#include "Components/CapsuleComponent.h"


AFHDungeonGenerator::AFHDungeonGenerator()
{
    GenerationType = EGenerationType::BFS;
    bCanLoop = true;

    bIsCreatedSuccessfully = false;
    // OnPreGenerationEvent는 시작할 때 최초 한 번임
    OnGenerationInitEvent.AddDynamic(this, &AFHDungeonGenerator::OnDungeonGenerationStart);
    OnPostInitializationEvent.AddDynamic(this, &AFHDungeonGenerator::OnDungeonGraphSet);
    OnPostGenerationEvent.AddDynamic(this, &AFHDungeonGenerator::OnDungeonGenerationDone);
    OnRoomAddedEvent.AddDynamic(this, &AFHDungeonGenerator::Event_OnRoomAdded);
    OnFailedToAddRoomEvent.AddDynamic(this, &AFHDungeonGenerator::Event_OnFailedToAddRoom);

	OnPlayerEnterEvent.AddDynamic(this, &AFHDungeonGenerator::Event_PlayerEnterTo);
	OnPlayerExitEvent.AddDynamic(this, &AFHDungeonGenerator::Event_PlayerExitFrom);
}

void AFHDungeonGenerator::BeginPlay()
{
    Super::BeginPlay();
}

void AFHDungeonGenerator::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bEnabled && HasAuthority())
    {
        if (bCheckLoadingRate)
        {
            TArray<class URoom*> newArr;
            for (URoom* room : RoomsToCheckLoading)
            {
                if (room->IsInstanceInitialized())
                {
                    
                }
                else
                {
                    newArr.Add(room);
                }
            }

            RoomsToCheckLoading = newArr;
            loadingRate = 1 - (static_cast<float>(RoomsToCheckLoading.Num()) / static_cast<float>(RoomsToCheckNum)) * 0.9f;
            OnLoadRateChangedDelegate.Broadcast();
        }

        if (bTrySpawnCreature)
        {
            if (elapsedTimeAfterLastTryingOfSpawingCreature >= SelectedDescriptor->TrySpawnCreatureInterval)
            {
                bool isSpawned = TrySpawnCreature();
                if (isSpawned == true)
                {
                    bTrySpawnCreature = false;
                }
                elapsedTimeAfterLastTryingOfSpawingCreature -= SelectedDescriptor->TrySpawnCreatureInterval;
            }

            elapsedTimeAfterLastTryingOfSpawingCreature += DeltaTime;
        }
    }
}

void AFHDungeonGenerator::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
    Super::EndPlay(EndPlayReason);

    if (AssholdTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(AssholdTimer);
    }
}

void AFHDungeonGenerator::GenerateGate()
{
    ensureMsgf(HasAuthority(), TEXT("Please run this function on server only."));

    Generate();
}

// 상수값으로 고정
void AFHDungeonGenerator::SettingUpGate(FString GateThemeName, EGateRank GateRank, FFHGateSpecificationTableRow* GateSpecification)
{
    bEnabled = true;
    AFHGateGameModeBase* FHGateGameModeBase = Cast<AFHGateGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
    if (HasAuthority() && ensureMsgf(FHGateGameModeBase, TEXT("Failed to find GateGameStateBase.")))
    {
        FHGateGameModeBase->OnGatePowerLevelChangeDelegate.AddDynamic(this, &AFHDungeonGenerator::TurnOnSpawnCreatureFlag);
    }

    bool bDescriptorFound = false;
    for (auto& Theme : GateDescriptors)
    {
        if (Theme.Name == GateThemeName)
        {
            for (auto& RankSet : Theme.Ranks)
            {
                if (RankSet.Rank == GateRank)
                {
                    if (RankSet.Descriptor != nullptr)
                    {
                        bDescriptorFound = true;
                        SelectedDescriptor = RankSet.Descriptor;
                        break;
                    }
                    else
                    {
                        checkf(false, TEXT("Invalid Descriptor."));
                    }
                }
            }

            if (bDescriptorFound)
            {
                break;
            }
            else
            {
                checkf(false, TEXT("Failed to find rank."));
            }
        }
    }

    if (bDescriptorFound == false)
    {
        checkf(false, TEXT("Failed to find theme."));
    }
    


    // 그리드 유닛 변경 및 전파
    this->DungeonGridUnit = SelectedDescriptor->DungeonGridUnit;
    this->bGridUnitReplicated = true;

    // RandRange 사용하는 SpecialRoom 개수 고정
    for (auto& room : SelectedDescriptor->SpecialRooms)
    {
        if (room.bUseRandomRange)
        {
            room.FixedRoomNum = FMath::RandRange(static_cast<int32>(room.MinRoomNum), static_cast<int32>(room.MaxRoomNum));
        }
    }

    // 전체 룸 개수 고정
    SelectedDescriptor->RoomNum = FMath::RandRange(GateSpecification->MinRoomNum, GateSpecification->MaxRoomNum);
    
    // 최대 스폰량 결정
    SelectedDescriptor->MagicStoneValue = GateSpecification->MaxMagicStoneValue;
    SelectedDescriptor->MaxCreaturePower = GateSpecification->MaxCreaturePower;
    SelectedDescriptor->RandomSpawnPropValue = static_cast<uint32>(FMath::RandRange(GateSpecification->MinRandomSpawnPropValue, GateSpecification->MaxRandomSpawnPropValue));

    SelectedDescriptor->DungeonCoreCount = 0;

    // RoomData에 빈 Door 타입 채우기
    if (SelectedDescriptor->DefaultDoorType == nullptr) return;
    if (SelectedDescriptor->bMustStartOnStartRoom == true && SelectedDescriptor->StartRoom != nullptr)
    {
        for (auto& door : SelectedDescriptor->StartRoom->Doors)
        {
            if (door.Type == nullptr)
            {
                door.Type = SelectedDescriptor->DefaultDoorType;
            }
        }
    }
    if (SelectedDescriptor->bUseLastRoom == true && SelectedDescriptor->LastRoom != nullptr)
    {
        for (auto& door : SelectedDescriptor->LastRoom->Doors)
        {
            if (door.Type == nullptr)
            {
                door.Type = SelectedDescriptor->DefaultDoorType;
            }
        }
    }
    for (auto& regularRoom : SelectedDescriptor->RegularRooms)
    {
        for (auto& door : regularRoom->Doors)
        {
            if (door.Type == nullptr)
            {
                door.Type = SelectedDescriptor->DefaultDoorType;
            }
        }
    }
    for (auto& specialRoomSet : SelectedDescriptor->SpecialRooms)
    {
        for (auto& door : specialRoomSet.RoomData->Doors)
        {
            if (door.Type == nullptr)
            {
                door.Type = SelectedDescriptor->DefaultDoorType;
            }
        }
    }

    PRINT_LOG(TEXT("Gate setted up."));
}

void AFHDungeonGenerator::UnloadDungeon_Implementation()
{
    Graph->ForceUnloadAllRooms();
}

void AFHDungeonGenerator::OnDungeonGenerationStart()
{
    ++tryCount;

    PRINT_LOG(TEXT("Generation Try: %d"), tryCount);



    // 초기화
    SpecialRoomOrder.Empty();
    SpecialRoomReady.Empty();

	bTryToPlaceCoreRoomBefore = false;
    bIsCreatedSuccessfully = false;
    bIsFirstNavMeshUpdate = true;

    positionsOfPlacedRoom.Empty();

    // 최소 거리 규칙 체크용 룸 중심 위치 벡터 저장 배열 초기화
    if (SelectedDescriptor->StartRoom)
    {
        positionsOfPlacedRoom.Add(SelectedDescriptor->StartRoom.Get());
    }
    for (const auto& room : SelectedDescriptor->RegularRooms)
    {
        positionsOfPlacedRoom.Add(room.Get());
    }
    for (const auto& room : SelectedDescriptor->SpecialRooms)
    {
        positionsOfPlacedRoom.Add(room.RoomData.Get());
    }



    // 스페셜 룸 배치 인덱스 구성, 누락 주의
    PickCoreRoomOrder();
}

void AFHDungeonGenerator::OnDungeonGraphSet()
{
    bCheckLoadingRate = true;
    RoomsToCheckLoading = GetRooms()->GetAllRooms();
    RoomsToCheckNum = RoomsToCheckLoading.Num();
}

void AFHDungeonGenerator::OnDungeonGenerationDone()
{
    if (!bIsCreatedSuccessfully) return;
    if (!HasAuthority()) return;

    bCheckLoadingRate = false;
    PRINT_LOG(TEXT("Dungeon Generation Success"));

    // Replace player characters
    AFHGateGameModeBase* GateGameMode = Cast<AFHGateGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GateGameMode == nullptr)
    {
        ensureMsgf(false, TEXT("Failed to find gate game mode."));
        return;
    }


    GetWorld()->GetTimerManager().SetTimer(
        AssholdTimer,
        FTimerDelegate::CreateLambda([this, GateGameMode]()
    {
        GateGameMode->ReplacePlayerCharacters();

        // =========== spawning points ================
        SpawnPropsFromSpawningPoints();

        loadingRate = 1.0f;
        OnLoadRateChangedDelegate.Broadcast();


        // =========== Count core num ==============
        if (SelectedDescriptor->CoreObjectClass != nullptr)
        {
            TArray<AActor*> cores;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), SelectedDescriptor->CoreObjectClass, cores);
            SelectedDescriptor->DungeonCoreCount = cores.Num();
        }


        TArray<AActor*> doors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFHDoor::StaticClass(), doors);
        for (AActor* _door : doors)
        {
            AFHDoor* door = Cast<AFHDoor>(_door);
            if (door)
            {
                DoorsCache.Add(door);
            }
        }


        PRINT_LOG(TEXT("Success to spawn random props..."));

        UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

        TArray<AActor*> NavMeshes;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANavMeshBoundsVolume::StaticClass(), NavMeshes);
        if (NavMeshes.IsEmpty())
        {
            PRINT_LOG(TEXT("There is no nav mesh volume actor in this level."));
            OnNavMeshGeneratedInDungeon(nullptr);
        }
        else
        {
            navSys->OnNavigationGenerationFinishedDelegate.AddDynamic(this, &AFHDungeonGenerator::OnNavMeshGeneratedInDungeon);
            TurnOnDynamicNavMesh();

            GetWorldTimerManager().SetTimer(
                WaitNavMeshBakingTimer,
                this,
                &AFHDungeonGenerator::OnWaitNavMeshBakingTimerDone,
                static_cast<float>(GetRooms()->GetAllRooms().Num()) * SelectedDescriptor->MaximumNavBakingDelayPerRoom,
                false);
        }
    }), LoadingGateInsuranceTime, false);
}

void AFHDungeonGenerator::OnWaitNavMeshBakingTimerDone()
{
    OnNavMeshGeneratedInDungeon(nullptr);
}

// Server only
void AFHDungeonGenerator::OnNavMeshGeneratedInDungeon(class ANavigationData* NavData)
{
    if (bIsFirstNavMeshUpdate)
    {
        bIsFirstNavMeshUpdate = false;
    }
    else
    {
        return;
    }


    PRINT_LOG(TEXT("Enter OnNavMeshGeneratedInGeneratedDungeon..."));
    
    if (!HasAuthority()) return;

    WaitNavMeshBakingTimer.Invalidate();
    UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    navSys->OnNavigationGenerationFinishedDelegate.RemoveDynamic(this, &AFHDungeonGenerator::OnNavMeshGeneratedInDungeon);

    AFHGateGameModeBase* GateGameMode = Cast<AFHGateGameModeBase>(GetWorld()->GetAuthGameMode());

    // if (bIsFirstNavMeshUpdate) { }
	SpawnMagicStonesRandomly();

    SpawnRandomPropsOnNavMesh();

    RemovePreventSpawnOnlyNavVolumes();

    for (AFHDoor* door : DoorsCache)
    {
        door->TurnOffBoxCollisionForPreventingSpawningNearby();
    }


    if (GateGameMode)
    {
        GateGameMode->GenerateCompleted();
    }

    AFHGateGameStateBase* GateState = Cast<AFHGateGameStateBase>(GetWorld()->GetGameState());
    if (GateState)
    {
        GateState->RoomDisclosedStates.Init(0, GetRooms()->Count());
    }
    ConstructMinimap();

    PRINT_LOG(TEXT("Success to create dungeon environment."));
}

void AFHDungeonGenerator::ConstructMinimap_Implementation()
{
    TArray<FMinimapRoomSpec> roomSpecs = ConstructRoomSpecs();

    float RoomHeight = DungeonGridUnit.Z;
    float GeneratorPosZ = GetActorLocation().Z;

    Cast<AFHPlayerBase>(GetWorld()->GetFirstPlayerController()->GetCharacter())->ConstructMinimapWidget(DungeonGridUnit, roomSpecs, DungeonLowestFloor, DungeonHighestFloor, RoomHeight, GeneratorPosZ);

	/*for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AFHPlayerController* PlayerController = Cast<AFHPlayerController>(Iterator->Get());
		if (PlayerController)
		{
			PlayerController->ConstructMinimapWidget(roomSpecs, RoomHeight, GeneratorPosZ);
		}
	}*/
}

void AFHDungeonGenerator::TurnOnSpawnCreatureFlag()
{
    bTrySpawnCreature = true;
}

bool AFHDungeonGenerator::TrySpawnCreature()
{
    // return false -> failed to spawn, retry on next tick...
    // return true -> success to spawn

    if (!HasAuthority()) return false;

    AFHGateGameModeBase* FHGateGameModeBase = Cast<AFHGateGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
    if (FHGateGameModeBase == nullptr)
    {
        PRINT_LOG(TEXT("Failed to find game Mode base."));
        return false;
    }

    AFHGateGameStateBase* FHGateGameStateBase = FHGateGameModeBase->GetGameState<AFHGateGameStateBase>();
    if (FHGateGameStateBase == nullptr)
    {
        PRINT_LOG(TEXT("Failed to find game state base."));
        return false;
    }

    while (true)
    {
        // 스폰할 몹 정하기
        float leftPower = FHGateGameModeBase->GetGatePowerLevel() - FHGateGameModeBase->AccumulatedSpawnPower;
        TArray<float> creatureWeights;
        TArray<TSubclassOf<AFHCreatureBase>> creatureTypes;
        for (auto& creatureSet : SelectedDescriptor->SpawnableCreatures)
        {
            if (leftPower >= creatureSet.PowerLevel && creatureSet.MonsterClass != nullptr)
            {
                creatureWeights.Add(creatureSet.Weight);
                creatureTypes.Add(creatureSet.MonsterClass);
            }
        }

        if (creatureTypes.IsEmpty())
        {
            return true;
        }

        int32 creatureIdx = UFHBlueprintFunctionLibrary::GetRandIndexUsingDiscreteDistribution(creatureWeights);
        if (creatureIdx == -1)
        {
            return true;
        }
        TSubclassOf<AFHCreatureBase> spawnTarget = creatureTypes[creatureIdx];
        float spawnTargetCost = creatureWeights[creatureIdx];


        // 마력석 스폰 로직 재활용했음 / 나중에 정리좀
        
        // 스폰할 장소 찾기
        TSet<URoom*> allRooms(GetRooms()->GetAllRooms());
        TArray<AFHPlayerStateBase*> AlivePlayerList;
        FHGateGameStateBase->GetAlivePlayerList(AlivePlayerList);
        for (AFHPlayerStateBase* PlayerState : AlivePlayerList)
        {
            TArray<URoom*> roomInside = GetPlayerInsideRooms(PlayerState->GetPlayerController()->GetPawn());
            for (URoom* room : roomInside)
            {
                TSet<URoom*> exceptRooms(GetAllRoomsInSuchDistance(room, SelectedDescriptor->SpawnCreatureMinimumRoomGap));
                allRooms = allRooms.Difference(exceptRooms);
            }
        }
        if (allRooms.IsEmpty())
        {
            PRINT_LOG(TEXT("Failed to find adequate room for spawning creature."));
            return false;
        }

        int32 roomIdx = FMath::RandRange(0, allRooms.Num() - 1);
        URoom* spawnTargetRoom = allRooms.Array()[roomIdx];

        UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
        FVector TargetLocation;
        FCollisionObjectQueryParams targetPreset = FCollisionObjectQueryParams(ECC_WorldStatic);

        bool canSpawn = false;
        for (int i = 0; i < 10; ++i)
        {
            bool found = GetRandomLocationOfRoom(TargetLocation, navSys, spawnTargetRoom, 2); // 2번?

            if (found == true)
            {
                if (HasChanceToBeDetectedByPlayer(TargetLocation))
                {
                    continue;
                }
                else
                {
                    canSpawn = true;
                    break;
                }
            }
        }

        // 임시 코드
        {
            ACharacter* CreatureActorCDO = Cast<ACharacter>(spawnTarget->GetDefaultObject());
            if (CreatureActorCDO)
            {
                if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
                {
                    FNavLocation Projected;
                    FVector Extent{ 25.0f, 25.0f , 25.0f }; // 탐색 박스 반경
                    Extent.X = CreatureActorCDO->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
                    Extent.Y = CreatureActorCDO->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
					Extent.Z = CreatureActorCDO->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

                    if (NavSys->ProjectPointToNavigation(TargetLocation, Projected, Extent))
                    {
                        TargetLocation = Projected.Location;
                    }
                }
                TargetLocation += FVector(0.0f, 0.0f, CreatureActorCDO->GetDefaultHalfHeight() + 1);


            }
        }

        AActor* spawnedCreature = nullptr;
        if (canSpawn)
        {
            FRotator randomRotation = FRotator(0, FMath::RandRange(0.0f, 1.0f) * 360.0f, 0);
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
            spawnedCreature = GetWorld()->SpawnActor<AActor>(spawnTarget, TargetLocation, randomRotation, SpawnParams);
        }

        AFHCreatureBase* monster = Cast<AFHCreatureBase>(spawnedCreature);
        if (monster == nullptr)
        {
            PRINT_LOG(TEXT("Failed to find adequate place for spawning creature."));
            return false;
        }

        PRINT_LOG(TEXT("Creature spawned in room %d, creature type is %s"), spawnTargetRoom->GetRoomID(), *spawnTarget->GetName());
        FHGateGameModeBase->AccumulatedSpawnPower += spawnTargetCost;
        continue;
    }
}

bool AFHDungeonGenerator::HasChanceToBeDetectedByPlayer(FVector TargetLocation)
{
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        AFHPlayerController* PlayerController = Cast<AFHPlayerController>(Iterator->Get());
        AFHPlayerBase* player = Cast<AFHPlayerBase>(PlayerController->GetPawn());
        if (!player)
        {
            continue;
        }
        if (IsColinear(player->GetActorLocation(), TargetLocation))
        {
            return true;
        }
    }
    return false;
}

bool AFHDungeonGenerator::IsColinear(FVector PlayerLocation, FVector TargetLocation)
{
    if (FMath::Abs(PlayerLocation.Z - TargetLocation.Z) >= SelectedDescriptor->CheckIsColinearWithPlayerRange.Z)
    {
        return false;
    }
    if (FMath::Abs(PlayerLocation.X - TargetLocation.X) >= SelectedDescriptor->CheckIsColinearWithPlayerRange.X)
    {
        return false;
    }
    if (FMath::Abs(PlayerLocation.Y - TargetLocation.Y) >= SelectedDescriptor->CheckIsColinearWithPlayerRange.Y)
    {
        return false;
    }
    return true;
}

void AFHDungeonGenerator::Event_OnRoomAdded(const URoomData* Room, const TScriptInterface<IReadOnlyRoom>& RoomInstance)
{
    // 룸 추가 성공시 공통
    const UFHRoomData* FHRoom = Cast<const UFHRoomData>(Room);
    if (FHRoom->bIsCoreRoom)
    {
        //DungeonDescriptor->LeftCoreRoomCount++;
    }
    if (positionsOfPlacedRoom.Find(FHRoom))
    {
        positionsOfPlacedRoom[FHRoom].Add(RoomInstance->GetBoundsCenter());
    }


    // 스페셜룸 배치였을 때
    if (bTryToPlaceCoreRoomBefore == true)
    {
        bTryToPlaceCoreRoomBefore = false;
        if (SpecialRoomReady.IsEmpty())
        {
            ensureMsgf(false, TEXT("CoreRoomReady array should not be empty."));
        }

        if (AddedRoomIndexOfCoreRoomReady >= static_cast<uint32>(SpecialRoomReady.Num()))
        {
            ensureMsgf(false, TEXT("Critical Error: AddedRoomIndexOfCoreRoomReady over SpecialRoomReady num."));
            SpecialRoomReady.RemoveAt(0);
            return;
        }
        SpecialRoomReady.RemoveAt(AddedRoomIndexOfCoreRoomReady);
    }
}

void AFHDungeonGenerator::Event_OnFailedToAddRoom(const URoomData* Room, const FDoorDef& Door)
{
    // 룸 추가 실패

    // 코어룸 배치 시도 플래그도 끄지 않음 (코어 룸 배치 시도에 실패했으면 대신 일반룸 한 번 배치)
}

TArray<FMinimapRoomSpec> AFHDungeonGenerator::ConstructRoomSpecs()
{
    FVector RoomUnit = DungeonGridUnit;
    FVector GeneratorLocation = GetActorLocation();
    double epsilon = 10.0f;

	TArray<FMinimapRoomSpec> roomSpecs;
	const TArray<URoom*>& rooms = GetRooms()->GetAllRooms();
    roomSpecs.Reserve(rooms.Num());

	for (int i = 0; i < rooms.Num(); ++i)
	{
        const URoom& room = *rooms[i];
        const UFHRoomData& roomData = *Cast<const UFHRoomData>(room.GetRoomData());

        FVector roomPos = room.GetBoundsCenter();
        FVector roomExt = room.GetBoundsExtent();
        
        int32 roomFloors = static_cast<int32>(roomExt.Z * 2.0f / RoomUnit.Z);

        double lowestPos = roomPos.Z - roomExt.Z + epsilon;
        int32 lowestFloor = static_cast<int32>(lowestPos / RoomUnit.Z);
        if (lowestPos < 0.0f) lowestFloor -= 1; // 음수층 처리
        int32 highestFloor = lowestFloor + roomFloors - 1;
        
        DungeonLowestFloor = DungeonLowestFloor > lowestFloor ? lowestFloor : DungeonLowestFloor;
        DungeonHighestFloor = DungeonHighestFloor < highestFloor ? highestFloor : DungeonHighestFloor;

        roomSpecs.Add({rooms[i], Cast<const UFHRoomData>(rooms[i]->GetRoomData()), lowestFloor, highestFloor });
	}

    return roomSpecs;
}

void AFHDungeonGenerator::RemovePreventSpawnOnlyNavVolumes()
{
    TArray<AActor*> Volumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFHNavModifierVolume::StaticClass(), Volumes);

    for (auto* actor : Volumes)
    {
        AFHNavModifierVolume* volume = Cast<AFHNavModifierVolume>(actor);
        if (!volume) continue;

        if (volume->VolumeType == EFHNavModifierVolumeType::PreventSpawn)
        {
            volume->Destroy();
        }
    }
}

void AFHDungeonGenerator::SpawnPropsFromSpawningPoints()
{
    TArray<AActor*> SpawningPoints;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFHObjectSpawningPoint::StaticClass(), SpawningPoints);

    // 모든 스폰 포인트 순회하며 각자 확률 계산
    for (auto* pointRaw : SpawningPoints)
    {
        AFHObjectSpawningPoint* point = Cast<AFHObjectSpawningPoint>(pointRaw);
        if (!point)
            continue;

        UFHSpawningPointDescriptor* descriptor = point->SpawningPointDescriptor;
        if (!descriptor)
            continue;
        if (descriptor->SpawnableActorList.Num() == 0)
            continue;
        if (descriptor->SpawnChance == 0.0f || FMath::RandRange(0.0f, 1.0f) >= descriptor->SpawnChance)
            continue;

        TArray<float> weights;
        for (FSpawnableActorDescriptor& desc : descriptor->SpawnableActorList)
        {
            if (desc.bSpawnable == false)
            {
                weights.Add(0.0f);
            }
            else
            {
                weights.Add(desc.weights);
			}
        }

        uint32 index = UFHBlueprintFunctionLibrary::GetRandIndexUsingDiscreteDistribution(weights);
        if (index == -1) continue;
        
        TSubclassOf<AActor> ac = descriptor->SpawnableActorList[index].actor;
        if (ac == nullptr) continue;

        FVector SpawningLocation = point->GetActorLocation();
        FRotator SpawningRotation;
        if (descriptor->SpawnableActorList[index].bRandomRotation == true)
        {
            SpawningRotation = FRotator(0, FMath::FRand() * 360.f, 0);
        }
        else
        {
            SpawningRotation = point->GetActorRotation();
        }
        AActor* spawnedActor = GetWorld()->SpawnActor<AActor>(ac, SpawningLocation, SpawningRotation);
        spawnedActor->SetActorScale3D(point->GetActorScale3D());
        point->Destroy();
    }
}


struct FRoomSpec
{
    float distFromStart{ 0.0f };
    float roomSize{ 0.0f };

    float distScore{ 0 };
    float sizeScore{ 0 };
    float totalScore{ 0 };
};

void AFHDungeonGenerator::SpawnMagicStonesRandomly()
{
    if (SelectedDescriptor->SpawnableMagicStones.IsEmpty())
    {
		UE_LOG(LogTemp, Warning, TEXT("DungeonDescriptor has no spawnable magic stones."));
        return;
    }
    TArray<URoom*> rooms = GetRooms()->GetAllRooms();
	/*URoom* startRoom = rooms[0];
	rooms.RemoveAt(0);*/ // 시작룸은 더이상 0번 인덱스가 아님.


    // 룸 점수 매기기
    TArray<FRoomSpec> roomSpecs;
    roomSpecs.Init({}, rooms.Num());
    
    double longestDist = 0;
    uint32 largestSize = 0;
    FVector lenDivUnit = FVector{2.0f, 2.0f, 2.0f} / DungeonGridUnit;
    for (int i = 0; i < rooms.Num(); ++i)
    {
        //roomSpecs[i].distFromStart = FVector::Distance(rooms[i]->GetBoundsCenter(), startRoom->GetBoundsCenter());
        roomSpecs[i].distFromStart = 0;
        FVector sizeVec = rooms[i]->GetBoundsExtent() * lenDivUnit;
        roomSpecs[i].roomSize = sizeVec.X * sizeVec.Y; // 높이는 제외

        if (longestDist < roomSpecs[i].distFromStart) longestDist = roomSpecs[i].distFromStart;
        if (largestSize < roomSpecs[i].roomSize) largestSize = roomSpecs[i].roomSize;
    }
    TArray<float> roomScores;
    for (FRoomSpec& spec : roomSpecs)
    {
        if (longestDist > 0)
        {
            //spec.distScore = spec.distFromStart / longestDist;
            spec.distScore = 0;
        }
        if (largestSize > 0)
        {
            spec.sizeScore = spec.roomSize / largestSize;
        }
        spec.totalScore = (spec.distScore * SelectedDescriptor->MagicStoneSpawnChanceByDistanceMult) + (spec.sizeScore * SelectedDescriptor->MagicStoneSpawnChanceByRoomSizeMult);

        roomScores.Add(spec.totalScore);
    }

    const TArray<FMagicStoneSet>& magicStones = SelectedDescriptor->SpawnableMagicStones;
    TArray<float> stoneValues;
    for (const FMagicStoneSet& set : magicStones)
    {
        stoneValues.Add(set.weighting);
    }

    // 단순 랜덤 반복
    const float maximumValue = SelectedDescriptor->MagicStoneValue;
    float currentValue = 0;
    UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    
    while (currentValue < maximumValue)
    {
        int maxTry = 50;
        bool success = false;
        for (int curTry = 0; curTry < maxTry; ++curTry)
        {
            int32 roomIdx = UFHBlueprintFunctionLibrary::GetRandIndexUsingDiscreteDistribution(roomScores);
            int32 stoneIdx = UFHBlueprintFunctionLibrary::GetRandIndexUsingDiscreteDistribution(stoneValues);
            if (roomIdx == -1 || stoneIdx == -1)
            {
                return;
            }

            success = TrySpawnMagicStoneInDaRoom(navSys, rooms[roomIdx], magicStones[stoneIdx]);
            if (success)
            {
                currentValue += magicStones[stoneIdx].Value;
                break;
            }
        }

        if (success == false)
        {
            PRINT_LOG(TEXT("Spawning magic stone phase failed due to critical problem..."));
            return;
        }
    }

    AFHGateGameStateBase* GateState = Cast<AFHGateGameStateBase>(GetWorld()->GetGameState());
    if (GateState)
    {
        GateState->TotalMagicStoneValue = currentValue;
    }

    PRINT_LOG(TEXT("Success to spawn magic stones..."));

    /*FBoxCenterAndExtent dungeonBounds = GetRooms()->GetDungeonBounds();

    FVector SerachArea = dungeonBounds.Center + dungeonBounds.Extent;

    float SearchAreaRadius = FMath::Max3(SerachArea.X, SerachArea.Y, SerachArea.Z);
    float MagicStoneActorSweepShapeRadius = 1.0f;
    float extraRadiusForAvoidOverlapping = 1.0f;
    uint32 maxTryPerEachMagicStone = 999;

    float MinimamDistanceFromOtherMagicStone = 500.0f;

    UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    
    // test2
    // 방마다 하나씩 배치
    const TArray<URoom*>& rooms = GetRooms()->GetAllRooms();
    for (const URoom* room : rooms)
    {
        FVector roomCenter = room->GetBoundsCenter();
        FVector roomExtents = room->GetBoundsExtent();

        float searchRadius = FMath::Max3(roomExtents.X, roomExtents.Y, roomExtents.Z);
        float lowestZ = roomCenter.Z - roomExtents.Z + 30.0f;

        {
            FVector Location = roomCenter; // 혹은 원하는 위치
            Location.Z = lowestZ;
            float Radius = 100.0f;
            int32 Segments = 12;
            FColor Color = FColor::Green;
            bool bPersistentLines = false;
            float LifeTime = 30.0f;
            uint8 DepthPriority = 0;
            float Thickness = 2.0f;

            DrawDebugSphere(GetWorld(), Location, Radius, Segments, Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
        }
        

        FNavLocation targetLocation;
        bool found = navSys->GetRandomReachablePointInRadius(FVector{roomCenter.X, roomCenter.Y, lowestZ}, searchRadius, targetLocation);
        
    
        if (found)
        {
            FVector SpawnLocation = targetLocation.Location;
            bool spawned = false;
            for (uint32 j = 0; j < maxTryPerEachMagicStone; ++j)
            {
                UWorld* World = GetWorld();
                FCollisionShape SweepShape = FCollisionShape::MakeSphere(MagicStoneActorSweepShapeRadius + extraRadiusForAvoidOverlapping);
                FCollisionQueryParams QueryParams;
                QueryParams.AddIgnoredActor(nullptr); // 필요시 스폰 주체 등 무시할 액터 추가
                bool bOverlap = GetWorld()->OverlapAnyTestByObjectType(SpawnLocation + FVector{ 0, 0, MagicStoneActorSweepShapeRadius * 1.05f }, FQuat::Identity, FCollisionObjectQueryParams(ECC_WorldStatic), SweepShape, QueryParams);

                if (bOverlap == false)
                {
                    SweepShape = FCollisionShape::MakeSphere(MinimamDistanceFromOtherMagicStone);
                    bOverlap = GetWorld()->OverlapAnyTestByObjectType(SpawnLocation, FQuat::Identity, FCollisionObjectQueryParams(ECC_Vehicle), SweepShape, QueryParams);
                    if (bOverlap == false)
                    {
                        FActorSpawnParameters SpawnParams;
                        // 밀어내기가 아예 작동을 안 한다???
                        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
                        //SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
                        GetWorld()->SpawnActor<AActor>(DungeonDescriptor->TestMagicStoneActor, SpawnLocation, FRotator{}, SpawnParams);
                        spawned = true;
                        break;
                    }


                }
            }

            if (!spawned)
            {
                PRINT_LOG(TEXT("Failed to spawn magic stone due to failed to find adequate location for spawning."));
            }
        }
        else
        {
            PRINT_LOG(TEXT("Failed to find reachable point."));
        }
    }*/
}

bool AFHDungeonGenerator::TrySpawnMagicStoneInDaRoom(UNavigationSystemV1* navSys, URoom* room, const FMagicStoneSet& magicStoneSet)
{
    FVector TargetLocation;
    // 문이나 겹치면 안 되는 애들만 다른 콜리전 채널로 빼고 그거에 대해서만 체크해야 할 듯?
    FCollisionObjectQueryParams targetPreset = FCollisionObjectQueryParams(ECC_WorldStatic | ECC_WorldDynamic | ECC_Pawn);

    bool found = false;
    for (int i = 0; i < 10; ++i)
    {
        found = GetRandomLocationOfRoom(TargetLocation, navSys, room, targetPreset, magicStoneSet.MinimumDistanceFromStaticStructures, 1);
        if (found == true)
        {
            TArray<FHitResult> HitResults;
            TArray<TEnumAsByte<EObjectTypeQuery>> CollisionChannels{
                UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Destructible)
            };
            TArray<AActor*> Ignores;
            TArray<AActor*> Overlapped;
            bool bIsHit = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), TargetLocation, magicStoneSet.MinimumDistanceFromOtherMagicStones, CollisionChannels, nullptr, Ignores, Overlapped);

            // Z값 체크를 해야 할까?
            if (bIsHit == true)
            {
                found = false;
            }
            else
            {
                break;
            }
        }
        else
        {
            continue;
        }
    }

    if (found == true)
    {
        FRotator randomRotation = FRotator(0, FMath::RandRange(0.0f, 1.0f) * 360.0f, 0);
        FActorSpawnParameters SpawnParams;
        // 밀어내기가 작동을 하는 건지 모르겠음
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(magicStoneSet.MagicStoneClass, TargetLocation, randomRotation, SpawnParams);

        if (AFHMagicStone* SpawnedStone = Cast<AFHMagicStone>(SpawnedActor); SpawnedStone != nullptr)
        {
            SpawnedStone->Value = magicStoneSet.Value;
        }

        return true;
    }
    else
    {
        return false;
    }
}

void AFHDungeonGenerator::SpawnRandomPropsOnNavMesh()
{
    if (!HasAuthority()) return;

    AFHGateGameModeBase* FHGateGameModeBase = Cast<AFHGateGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
    if (FHGateGameModeBase == nullptr)
    {
        PRINT_LOG(TEXT("Failed to find game Mode base."));
        return;
    }

    AFHGateGameStateBase* FHGateGameStateBase = FHGateGameModeBase->GetGameState<AFHGateGameStateBase>();
    if (FHGateGameStateBase == nullptr)
    {
        PRINT_LOG(TEXT("Failed to find game state base."));
        return;
    }

	uint32 LeftValue = SelectedDescriptor->RandomSpawnPropValue;
	TArray<URoom*> rooms = GetRooms()->GetAllRooms();
	UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    
    if (rooms.Num() == 0) return;

    uint32 tryCounter = LeftValue * 2;
    while (LeftValue > 0)
    {
        URoom* TargetRoom = rooms[FMath::RandRange(0, rooms.Num() - 1)];

        TArray<float> PropWeights;
        TArray<TSubclassOf<AActor>> PropTypes;
        TArray<float> PropMinDist;
        for (auto& propSet : SelectedDescriptor->RandomSpawnProps)
        {
            if (LeftValue >= propSet.Value && propSet.RandomSpawnPropActorClass != nullptr)
            {
                PropWeights.Add(propSet.Weight);
                PropTypes.Add(propSet.RandomSpawnPropActorClass);
                PropMinDist.Add(propSet.MinimumDistanceFromOtherStructures);
            }
        }

        if (PropTypes.IsEmpty()) break;

        int32 SelectedIdx = UFHBlueprintFunctionLibrary::GetRandIndexUsingDiscreteDistribution(PropWeights);
        if (SelectedIdx == -1) break;
        
        TSubclassOf<AActor> TargetClass = PropTypes[SelectedIdx];
        float TargetCost = PropWeights[SelectedIdx];
        float TargetMinDistance = PropMinDist[SelectedIdx];


        FVector TargetLocation;
        FCollisionObjectQueryParams targetPreset = FCollisionObjectQueryParams(ECC_WorldStatic | ECC_WorldDynamic | ECC_Pawn | ECC_Destructible);

        bool found = false;
        for (int i = 0; i < 10; ++i)
        {
            found = GetRandomLocationOfRoom(TargetLocation, navSys, TargetRoom, targetPreset, TargetMinDistance, 1);
            if (found == true)
            {
                break;
            }
        }

        if (found == true)
        {
            FRotator randomRotation = FRotator(0, FMath::RandRange(0.0f, 1.0f) * 360.0f, 0);
            FActorSpawnParameters SpawnParams;
            // 밀어내기가 작동을 하는 건지 모르겠음
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            GetWorld()->SpawnActor<AActor>(TargetClass, TargetLocation, randomRotation, SpawnParams);

            LeftValue -= TargetCost;
        }
        else
        {
            --tryCounter;
            if (tryCounter <= 0)
            {
                PRINT_LOG(TEXT("Unexpected halting on spawn random props due to too many tries."))
                break;
            }
        }
    }
}

void AFHDungeonGenerator::TurnOnDynamicNavMesh()
{   
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys)
	{
		NavSys->ReleaseInitialBuildingLock();
        // 내부적으로 RebuildAll() 호출)
	}
}

bool AFHDungeonGenerator::CheckDoorDirectionIfYouNeed(const EDoorDirection dir1, const EDoorDirection dir2)
{
    switch (dir1)
    {
        case EDoorDirection::North:
			if (dir2 == EDoorDirection::South) return true;
            return false;
            break;
        case EDoorDirection::South:
            if (dir2 == EDoorDirection::North) return true;
            return false;
            break;
        case EDoorDirection::East:
            if (dir2 == EDoorDirection::West) return true;
            return false;
            break;
        case EDoorDirection::West:
            if (dir2 == EDoorDirection::East) return true;
            return false;
            break;
    }

    return false;
}

bool AFHDungeonGenerator::IsRoomAddable(const UFHRoomData* prevRoom, const TScriptInterface<IReadOnlyRoom>& PrevRoomInstance, const UFHRoomData* targetRoom, const FDoorDef& DoorData)
{
    if (targetRoom->HasCompatibleDoor(DoorData) == false)
    {
        return false;
    }

    if (prevRoom != nullptr)
    {
        if (prevRoom->CheckConnectable(targetRoom) == false)
            return false;

        float dungeonGenProgressRate = SelectedDescriptor->RoomNum / GetRooms()->Count();
        if (prevRoom->CheckProgressRate(dungeonGenProgressRate) == false)
            return false;

        uint32 curRoomTypeCount = GetRooms()->CountRoomData(targetRoom);
        if (prevRoom->CheckRoomNum(curRoomTypeCount) == false)
            return false;

        FVector generatorPos = bUseGeneratorTransform ? GetActorLocation() : FVector{ 0,0,0 };
        if (targetRoom->CheckDistanceFromStartRoom(DungeonGridUnit, generatorPos, DoorData) == false)
            return false;


        // 최소 거리 룰
        if (targetRoom->bUseMinimumDistanceRule && targetRoom->MinimumDistanceRules.Num() > 0)
        {
            for (const auto& distRule : targetRoom->MinimumDistanceRules)
            {
                UFHRoomData* distRuleTarget = distRule.TargetRoom.Get();
                if (positionsOfPlacedRoom.Find(distRuleTarget))
                {
                    for (FVector& loc : positionsOfPlacedRoom[distRuleTarget])
                    {
                        FVector doorPos = FDoorDef::GetRealDoorPosition(DungeonGridUnit, DoorData);
                        double distanceBetween = FVector::Distance(doorPos, loc);
                        if (distRule.Distance > distanceBetween)
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

void AFHDungeonGenerator::PickCoreRoomOrder()
{
    for (FSpecialRoomSet& roomSet : SelectedDescriptor->SpecialRooms)
    {
        uint32 thisRoomNum;
        if (roomSet.bUseRandomRange)
        {
            thisRoomNum = FMath::RandRange(static_cast<int32>(roomSet.MinRoomNum), static_cast<int32>(roomSet.MaxRoomNum));
        }
        else
        {
            thisRoomNum = roomSet.RoomNum;
        }

		for (uint32 i = 0; i < thisRoomNum; ++i)
		{
			TPair<TObjectPtr<UFHRoomData>, uint8> pair;
			pair.Key = roomSet.RoomData;
			pair.Value = FMath::RandRange(0, SelectedDescriptor->RoomNum - 1);

			SpecialRoomOrder.Add(pair);
		}
    }
    
    SpecialRoomOrder.Sort([](const TPair<TObjectPtr<UFHRoomData>, uint8>& a, const TPair<TObjectPtr<UFHRoomData>, uint8>& b)
    {
        return a.Value < b.Value;
    });

    while (SpecialRoomOrder.IsEmpty() == false)
    {
        const auto& pair = SpecialRoomOrder[0];
        if (pair.Value == 0)
        {
            SpecialRoomReady.Add(pair.Key);
            SpecialRoomOrder.RemoveAt(0);
        }
        else
        {
            break;
        }
    }
}

TArray<URoomData*> AFHDungeonGenerator::MakeCandidates(const UFHRoomData* prevRoom, const TScriptInterface<IReadOnlyRoom>& CurrentRoomInstance, const FDoorDef& DoorData, TArray<float>& roomWeights)
{
    TArray<URoomData*> result;
    for (TObjectPtr<UFHRoomData> room : SelectedDescriptor->RegularRooms)
    {
        if (IsRoomAddable(prevRoom, CurrentRoomInstance, room.Get(), DoorData) == false)
        {
            continue;
        }

        if (prevRoom->bUseCustomConnectionSetting)
        {
            bool isExist = false;
            for (const FFHCustomRoomConnectionSetting& setting : prevRoom->CustomConnectionSetting)
            {
                if (setting.OtherRoom == room)
                {
                    isExist = true;

                    if (setting.Weight == 0.0f)
                    {
                        break;
                    }
                    roomWeights.Add(setting.Weight);
                    result.Add(Cast<URoomData>(room.Get()));
                    break;
                }
            }

            if (isExist == false)
            {
                if (prevRoom->bUseOnlyCustomConnectionRule)
                {
                    // nobody here but us chicken
                }
                else
                {
                    if (room->DefaultWeight > 0.0f)
                    {
                        roomWeights.Add(room->DefaultWeight);
                        result.Add(Cast<URoomData>(room.Get()));
                    }
                }
            }
        }
        else
        {
            if (room->DefaultWeight > 0.0f)
            {
                roomWeights.Add(room->DefaultWeight);
                result.Add(Cast<URoomData>(room.Get()));
            }
        }
    }

    return result;
}

bool AFHDungeonGenerator::IsLastRoomPlacing()
{
    return static_cast<uint32>(GetRooms()->Count()) >= SelectedDescriptor->RoomNum - 1;
}

int32 AFHDungeonGenerator::GetCompatibleRandomDoorIndex(const class URoomData* targetRoom, const FDoorDef& DoorData)
{
    TArray<int32> compatibleDoors;
    targetRoom->GetCompatibleDoors(DoorData, compatibleDoors);

    if (compatibleDoors.IsEmpty())
        return -1;

    return FMath::RandRange(0, compatibleDoors.Num() - 1);
}

bool AFHDungeonGenerator::CreateDungeon_Implementation()
{
    CurrentGenerator = this;

    bool result = Super::CreateDungeon_Implementation();

    if (result == false)
    {
        ensureMsgf(0, TEXT("ALERT. ALERT. SEVERE ERROR OCCURED."));
        bIsCreatedSuccessfully = false;
        return false;
    }

    bIsCreatedSuccessfully = true;
    return true;
}

URoomData* AFHDungeonGenerator::ChooseFirstRoomData_Implementation()
{
    if (SelectedDescriptor->bMustStartOnStartRoom && SelectedDescriptor->StartRoom)
    {
        return SelectedDescriptor->StartRoom;
    }

    if (!SpecialRoomReady.IsEmpty())
    {
        URoomData* readyToPlace = SpecialRoomReady[0].Get();
        SpecialRoomReady.RemoveAt(0);
        return readyToPlace;
    }

    TMap<URoomData*, int> RegularRooms;
    for (const auto& room : SelectedDescriptor->RegularRooms)
    {
        RegularRooms.Add(Cast<URoomData>(room.Get()), room->DefaultWeight);
    }
    return GetRandomRoomDataWeighted(RegularRooms);
	/*TArray<URoomData*> regularRooms;
	for (const auto& room : DungeonDescriptor->RegularRooms)
	{
		regularRooms.Add(Cast<URoomData>(room.Get()));
	}
	return GetRandomRoomData(regularRooms);*/
}

URoomData* AFHDungeonGenerator::ChooseNextRoomData_Implementation(const URoomData* CurrentRoom, const TScriptInterface<IReadOnlyRoom>& CurrentRoomInstance, const FDoorDef& DoorData, int& DoorIndex)
{
    uint32 currentRoomCount = GetRooms()->Count();

    // 이전에 스페셜룸 생성을 시도하고, 실패했다면(bTryToPlaceCoreRoomBefore가 true로 남아있음) 우선 일반룸 배치
    // 그렇지 않으면 우선 스페셜룸을 배치해야 하는지 체크
    if (SpecialRoomReady.IsEmpty() == false && bTryToPlaceCoreRoomBefore == false)
    {
        for (int i = 0; i < SpecialRoomReady.Num(); ++i)
        {
            UFHRoomData* roomData = SpecialRoomReady[i].Get();

            if (IsRoomAddable(Cast<const UFHRoomData>(CurrentRoom), CurrentRoomInstance, roomData, DoorData))
            {
                bTryToPlaceCoreRoomBefore = true;
                AddedRoomIndexOfCoreRoomReady = i;
                URoomData* readyToPlace = Cast<URoomData>(roomData);

                DoorIndex = GetCompatibleRandomDoorIndex(readyToPlace, DoorData);
                return readyToPlace;
            }
        }
    }
    if (bTryToPlaceCoreRoomBefore == true)
    {
        bTryToPlaceCoreRoomBefore = false;
    }


    // Last Room을 사용한다면 배치 시도
    if (IsLastRoomPlacing() && SelectedDescriptor->bUseLastRoom && SelectedDescriptor->LastRoom)
    {
        if (IsRoomAddable(Cast<const UFHRoomData>(CurrentRoom), CurrentRoomInstance, SelectedDescriptor->LastRoom, DoorData))
        {
            URoomData* readyToPlace = Cast<URoomData>(SelectedDescriptor->LastRoom);
            DoorIndex = GetCompatibleRandomDoorIndex(readyToPlace, DoorData);
            return readyToPlace;
        }
    }


    // 이후 Regular 룸 배치 시도
    TArray<float> roomWeights;
    TArray<URoomData*> candidates = MakeCandidates(Cast<const UFHRoomData>(CurrentRoom), CurrentRoomInstance, DoorData, roomWeights);

    if (candidates.IsEmpty())
    {
        return nullptr;
    }

    int32 roomIdx = UFHBlueprintFunctionLibrary::GetRandIndexUsingDiscreteDistribution(roomWeights);
    if (roomIdx == -1)
    {
        PRINT_LOG(TEXT("Critical Error Alert :: Failed to find adequate room."));
        return nullptr;
    }
    URoomData* targetRoom = candidates[roomIdx];
    DoorIndex = GetCompatibleRandomDoorIndex(targetRoom, DoorData);
    return targetRoom;
}

bool AFHDungeonGenerator::ContinueToAddRoom_Implementation()
{
    uint32 currentRoomCount = GetRooms()->Count();

    if (SpecialRoomOrder.IsEmpty() == false)
    {
        while (SpecialRoomOrder.IsEmpty() == false)
        {
            if (SpecialRoomOrder[0].Value <= currentRoomCount)
            {
                SpecialRoomReady.Add(SpecialRoomOrder[0].Key);
                SpecialRoomOrder.RemoveAt(0);
            }
            else
            {
                break;
            }
        }
    }

    // 일단 룸 한도에 도달했는데 스페셜 룸이 모두 배치되지 않았다면 컷, 재생성 시도.
    int32 extraRoomNum = 0; // 정해진 룸 개수에서 n개까지만 초과 허용
    if (SpecialRoomReady.IsEmpty() && currentRoomCount >= SelectedDescriptor->RoomNum)
    {
        return false;
    }
    else if (!SpecialRoomReady.IsEmpty() && currentRoomCount >= SelectedDescriptor->RoomNum + extraRoomNum)
    {
        return false;
    }
	/*if (currentRoomCount >= DungeonDescriptor->RoomNum && SpecialRoomReady.IsEmpty())
		return false;*/

    return true;


    //return !GetRooms()->HasAlreadyRoomData(EndingRoom);
}

bool AFHDungeonGenerator::IsValidDungeon_Implementation()
{
    if (static_cast<uint32>(GetRooms()->Count()) < SelectedDescriptor->RoomNum)
        return false;
    if (SpecialRoomReady.Num() > 0 || SpecialRoomOrder.Num() > 0)
    {
        PRINT_LOG(TEXT("Dungeon validation check failed : there is special room left. (RoomReady %d, RoomOrder %d)"), SpecialRoomReady.Num(), SpecialRoomOrder.Num());
        return false;
    }

    return true;
}

TSubclassOf<ADoor> AFHDungeonGenerator::ChooseDoor_Implementation(const URoomData* CurrentRoom, const URoom* CurrentRoomInstance, const URoomData* NextRoom, const URoom* NextRoomInstance, const UDoorType* DoorType, bool& Flipped, EDoorTag RoomATag, EDoorTag RoomBTag)
{
    const bool bAreBothValid = IsValid(CurrentRoom) && IsValid(NextRoom);
    TSubclassOf<ADoor> readyToPlace{ nullptr };

    // Place door
    if (bAreBothValid)
    {
        for (const FDoorSet& door : SelectedDescriptor->DoorSetting)
        {
            if (door.bUseDoorArray == false)
            {
                if (RoomATag == EDoorTag::EmptyIfBoth && RoomBTag == EDoorTag::EmptyIfBoth)
                {
                    return nullptr;
                }
                else if (RoomATag == EDoorTag::EmptyIfAny || RoomBTag == EDoorTag::EmptyIfAny)
                {
                    return nullptr;
                }
                else
                {
                    return door.Door;
                }
            }
            else
            {
                if (RoomATag == EDoorTag::EmptyIfBoth && RoomBTag == EDoorTag::EmptyIfBoth)
                {
                    return nullptr;
                }
                if (RoomATag == EDoorTag::ForceIndex0IfBoth && RoomBTag == EDoorTag::ForceIndex0IfBoth)
                {
                    if (door.Doors.Num() > 0)
                        return door.Doors[0];
                }
                if (RoomATag == EDoorTag::ForceIndex1IfBoth && RoomBTag == EDoorTag::ForceIndex1IfBoth)
                {
                    if (door.Doors.Num() > 1)
                        return door.Doors[1];
                }
                if (RoomATag == EDoorTag::ForceIndex2IfBoth && RoomBTag == EDoorTag::ForceIndex2IfBoth)
                {
                    if (door.Doors.Num() > 2)
                        return door.Doors[2];
                }
                if (RoomATag == EDoorTag::EmptyIfAny || RoomBTag == EDoorTag::EmptyIfAny)
                {
                    return nullptr;
                }
                if (RoomATag == EDoorTag::ForceIndex0IfAny || RoomBTag == EDoorTag::ForceIndex0IfAny)
                {
                    if (door.Doors.Num() > 0)
                        return door.Doors[0];
                }
                if (RoomATag == EDoorTag::ForceIndex1IfAny || RoomBTag == EDoorTag::ForceIndex1IfAny)
                {
                    if (door.Doors.Num() > 1)
                        return door.Doors[1];
                }
                if (RoomATag == EDoorTag::ForceIndex2IfAny || RoomBTag == EDoorTag::ForceIndex2IfAny)
                {
                    if (door.Doors.Num() > 2)
                        return door.Doors[2];
                }

                uint32 randIdx = FMath::RandRange(0, door.Doors.Num() - 1);
                return door.Doors[randIdx];
            }
        }
    }
    // Place wall
    else
    {
        for (const FWallSet& wall : SelectedDescriptor->WallSetting)
        {
            if (wall.bUseRandomPick == false)
            {
                return wall.Wall;
            }
            else
            {
                uint32 randIdx = FMath::RandRange(0, wall.Walls.Num() - 1);
                return wall.Walls[randIdx];
            }
        }
    }

    return nullptr;

    //const bool bAreBothRoomValid = IsValid(CurrentRoom) && IsValid(NextRoom);
    //return (bAreBothRoomValid) ? DungeonDescriptor->NormalDoor : DungeonDescriptor->Wall;
}





TSet<URoom*> AFHDungeonGenerator::GetAllRoomsInSuchDistance(int64 RootRoomID, uint32 RoomDistance)
{
    URoom* rootRoom = GetRooms()->GetAllRooms()[RootRoomID];
    return GetAllRoomsInSuchDistance(rootRoom, RoomDistance);
}

TSet<URoom*> AFHDungeonGenerator::GetAllRoomsInSuchDistance(URoom* RootRoom, uint32 RoomDistance)
{
    TSet<URoom*> root{ RootRoom };
    TSet<URoom*> outRooms;
    GetRooms()->TraverseRooms(root, &outRooms, RoomDistance, nullptr);

    return outRooms;
}

TSet<URoom*> AFHDungeonGenerator::GetAllRoomsAwaySuchDistance(int64 RootRoomID, uint32 RoomDistance)
{
    URoom* rootRoom = GetRooms()->GetAllRooms()[RootRoomID];
    return GetAllRoomsAwaySuchDistance(rootRoom, RoomDistance);
}
TSet<URoom*> AFHDungeonGenerator::GetAllRoomsAwaySuchDistance(URoom* RootRoom, uint32 RoomDistance)
{
    TSet<URoom*> roomsIncluded = GetAllRoomsInSuchDistance(RootRoom, RoomDistance);
    TArray<URoom*> allRooms = GetRooms()->GetAllRooms();
    
    TSet<URoom*> roomsExcluded;
    for (URoom* room : allRooms)
    {
        if (!roomsIncluded.Contains(room))
        {
            roomsExcluded.Add(room);
        }
    }
    return roomsExcluded;
}

void AFHDungeonGenerator::Event_PlayerEnterTo(int64 RoomEnter)
{
    AFHPlayerController* pc = Cast<AFHPlayerController>(GetWorld()->GetFirstPlayerController());
    if (!pc) return;

    pc->Server_NotifyRoomDisclosed(RoomEnter);

	//PRINT_LOG(TEXT("Enter to room %d"), RoomEnter->GetRoomID());
}

void AFHDungeonGenerator::Event_PlayerExitFrom(int64 RoomExit)
{
    //PRINT_LOG(TEXT("Exit from room %d"), RoomExit->GetRoomID());
}

bool AFHDungeonGenerator::GetRandomLocationOfRoom(OUT FVector& Result, class UNavigationSystemV1* navSys, URoom* TargetRoom, int32 MaximumTry /*= 100*/)
{
    FVector roomCenter = TargetRoom->GetBoundsCenter();
    FVector roomExtent = TargetRoom->GetBoundsExtent();
    FBox roomBox{ roomCenter - roomExtent, roomCenter + roomExtent };
    roomBox.Max.Z -= 100.0f; // 천장은 거르기

    float searchRadius = FMath::Max3(roomExtent.X, roomExtent.Y, roomExtent.Z * 1.8f); // 바닥에서 시작하는 스페어이므로 Z는 적당히 키워야 함.
    float lowestZ = roomCenter.Z - roomExtent.Z + 10.0f;

    for (int curTry = 0; curTry < MaximumTry; ++curTry)
    {
        // 위치 정하기
        FNavLocation navTargetLoc;
        bool found = navSys->GetRandomReachablePointInRadius(FVector{ roomCenter.X, roomCenter.Y, lowestZ }, searchRadius, navTargetLoc);
        if (!found)
        {
            // PRINT_LOG(TEXT("Failed to find adequate point to spawn. (Room id : %d"), room->GetRoomID());
            continue;
        }
        FVector targetLocation = navTargetLoc.Location;

        // 충돌 테스트
        // 1. 룸 내부에 잡혔는지 확인
        if (roomBox.IsInside(targetLocation) == false)
        {
            // PRINT_LOG(TEXT("Random point is not inside the room. (Room id : %d"), room->GetRoomID());
            continue;
        }

        Result = targetLocation;
        return true;
    }

    PRINT_LOG(TEXT("Failed to find adequate place. (Room id : %d"), TargetRoom->GetRoomID());
    return false;
}

bool AFHDungeonGenerator::GetRandomLocationOfRoom(OUT FVector& Result, class UNavigationSystemV1* navSys, URoom* TargetRoom, FCollisionObjectQueryParams AvoidPreset, float AvoidSphereRadius, int32 MaximumTry /*= 100*/, float AvoidEpsilon /*= 1.0f*/)
{
    FVector roomCenter = TargetRoom->GetBoundsCenter();
	FVector roomExtent = TargetRoom->GetBoundsExtent();
    FBox roomBox{ roomCenter - roomExtent, roomCenter + roomExtent };
    roomBox.Max.Z -= 100.0f; // 천장은 거르기

    float searchRadius = FMath::Max3(roomExtent.X, roomExtent.Y, roomExtent.Z * 1.8f); // 바닥에서 시작하는 스페어이므로 Z는 적당히 키워야 함.
    float lowestZ = roomCenter.Z - roomExtent.Z + 10.0f;

    for (int curTry = 0; curTry < MaximumTry; ++curTry)
    {
        // 위치 정하기
        FNavLocation navTargetLoc;
        bool found = navSys->GetRandomReachablePointInRadius(FVector{ roomCenter.X, roomCenter.Y, lowestZ }, searchRadius, navTargetLoc);
        if (!found)
        {
            // PRINT_LOG(TEXT("Failed to find adequate point to spawn. (Room id : %d"), room->GetRoomID());
            continue;
        }
        FVector targetLocation = navTargetLoc.Location;

        // 바닥과 딱 붙이기
        // navSys GetRandomPoint가 알아서 바닥에 딱 맞는 위치 찾아줌.
        /*FHitResult hitResult;
        FCollisionQueryParams traceParams(FName(TEXT("FindGround")), true);
        traceParams.bTraceComplex = true;
        traceParams.bReturnPhysicalMaterial = false;
        bool isHit = GetWorld()->LineTraceSingleByChannel(hitResult, targetLocation, FVector{ targetLocation.X, targetLocation.Y, roomCenter.Z - roomExtent.Z }, ECC_Visibility, traceParams);
        if (!isHit)
        {
            PRINT_LOG(TEXT("Faild to find ground. (Room id : %d"), room->GetRoomID());
        }
        else
        {
            targetLocation = hitResult.ImpactPoint;
        }*/

        // 충돌 테스트
        // 1. 룸 내부에 잡혔는지 확인
        if (roomBox.IsInside(targetLocation) == false)
        {
            // PRINT_LOG(TEXT("Random point is not inside the room. (Room id : %d"), room->GetRoomID());
            continue;
        }

        // 2. 커스텀 프리셋과 충돌 체크
        FCollisionShape SweepShape = FCollisionShape::MakeSphere(AvoidSphereRadius);
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(nullptr); // 필요시 스폰 주체 등 무시할 액터 추가
        bool isOverlap = GetWorld()->OverlapAnyTestByObjectType(targetLocation + FVector{ 0, 0, AvoidSphereRadius + AvoidEpsilon }, FQuat::Identity, AvoidPreset, SweepShape, QueryParams);

        if (isOverlap)
        {
            //PRINT_LOG(TEXT("Random point overlapped with other stuff. (Room id : %d"), room->GetRoomID());
            continue;
        }

        Result = targetLocation;
        return true;
    }

    PRINT_LOG(TEXT("Failed to find adequate place. (Room id : %d"), TargetRoom->GetRoomID());
    return false;
}

FVector AFHDungeonGenerator::GetRandomRoomLocation(FVector TeleportTargetPosition, float MinimumDistanceFromTarget /*= 100.0f*/)
{
    TArray<URoom*> Rooms = GetRooms()->GetAllRooms();

    int32 counter = 0;
    while (true)
    {
        int32 RandRoomIdx = FMath::RandRange(0, Rooms.Num() - 1);

        FVector ResultLocation;
        UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
        FCollisionObjectQueryParams targetPreset = FCollisionObjectQueryParams(ECC_WorldStatic);
        bool found = GetRandomLocationOfRoom(ResultLocation, navSys, Rooms[RandRoomIdx], targetPreset, 40, 10);

        if (found)
        {
            if (FVector::Distance(TeleportTargetPosition, ResultLocation) > MinimumDistanceFromTarget)
            {
                return ResultLocation;
            }
        }
        else
        {
            ++counter;

            if (counter > 100)
            {
                PRINT_LOG(TEXT("Failed to find adequate place for teleport."));
                return TeleportTargetPosition;
            }
        }
    }
}
