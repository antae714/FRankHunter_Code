// Copyright F Rank Hunter. All Rights Reserved.


#include "Core/FHGateGameModeBase.h"
#include "FHGameInstance.h"
#include "FHGateGameStateBase.h"
#include "Data/FH_DT_SampleMapData.h"
#include "DungeonGeneration/FHDungeonGenerator.h"
#include "DungeonGeneration/FHDungeonDescriptor.h"
#include "Core/FHPlayerController.h"
#include "Player/FHObserverPawn.h"
#include "Core/FHPlayerStateBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/Ability/FH_GA_CheckAttackHit.h"
#include "Data/FHGateSpecificationTable.h"
#include "Data/FHPowerLevelIncreaseTable.h"
#include "GAS/FHGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"
#include "DataAsset/FHLevelAsset.h"
#include "Props/GateProps/FHExit.h"
#include "Core/FHPropManager.h"
#include "UI/Gate/FHLoading.h"
#include "FHUIManager.h"
#include "Player/FHPlayerBase.h"
#include "Player/FHObserverPawn.h"
#include "Creature/FHCreatureBase.h"

#include "GameFramework/PlayerStart.h"

#include "Core/FHSoundManagerActor.h"


DEFINE_LOG_CATEGORY(LogGate);

AFHGateGameModeBase::AFHGateGameModeBase()
{
	bUseSeamlessTravel = true;

}

void AFHGateGameModeBase::StartPlay()
{
	Super::StartPlay();
}

void AFHGateGameModeBase::HandleSeamlessTravelPlayer(AController*& C)
{
	UE_LOG(LogGate, Warning, TEXT("Gate GameMode HandleSeamlessTravelPlayer Begin"));

	Super::HandleSeamlessTravelPlayer(C);

	UE_LOG(LogGate, Warning, TEXT("Gate GameMode HandleSeamlessTravelPlayer End"));
}

void AFHGateGameModeBase::PostSeamlessTravel()
{
	UE_LOG(LogGate, Warning, TEXT("Gate GameMode PostSeamlessTravel Begin"));

	Super::PostSeamlessTravel();

	UE_LOG(LogGate, Warning, TEXT("Gate GameMode PostSeamlessTravel End"));

	GateGameState = GetGameState<AFHGateGameStateBase>();
	if (GateGameState)
	{
		UE_LOG(LogGate, Warning, TEXT("GateGameState Load Success."));
	}
	else
	{
		UE_LOG(LogGate, Warning, TEXT("GateGameState Load Failed."));
	}

	GateGameInstance = Cast<UFHGameInstance>(GetGameInstance());
	if (GateGameInstance)
	{
		FString CurrentGateRank = GateGameInstance->GetGateRankString();
		UE_LOG(LogGate, Warning, TEXT("Gate Enter Success. Current GateRank: %s"), *CurrentGateRank);
	}

	TotalPlayerCount = GateGameInstance->GetPlayerCount();

	// Init gate
	ensureMsgf(GateGenerateRuleTable, TEXT("Cannot find GateSpecificationTable."));

	FFHGateSpecificationTableRow* row = GateGenerateRuleTable->FindRow<FFHGateSpecificationTableRow>(FName(*(GateGameInstance->GetGateRankString())), TEXT("Rank"));
	GateGameState->TimeLimit = row->TimeLimit;

	InitGenerator(row);
}

void AFHGateGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AFHGateGameModeBase::Logout(AController* Exiting)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Exiting->PlayerState, GET_GAMEPLAY_TAG_GAMEPLAYEVENT_DEATH, {});
	GateGameState->AddDeathCount();
	GateGameState->AddLogOutPlayer(Exiting->PlayerState);
	AFHPlayerBase* PlayerPawn = Exiting->GetPawn<AFHPlayerBase>();
	if (PlayerPawn)
	{
		PlayerPawn->Die();
	}
	//Cast<AFHPlayerController>(Exiting)->SuperPawnLeavingGame();

	Super::Logout(Exiting);

	UE_LOG(LogGameMode, Log, TEXT("Player %s has logged out."), *Exiting->GetName());

	AFHPlayerStateBase* ExitingPlayerState = Exiting->GetPlayerState<AFHPlayerStateBase>();

	if (GateGameState && ExitingPlayerState)
	{
		EPlayerGateState PlayerGateState = GateGameState->GetPlayerList().GetPlayerGateStateByPlayerState(ExitingPlayerState);
		if (PlayerGateState == EPlayerGateState::Alive)
		{
			GateGameState->GetPlayerList().SetPlayerDied(ExitingPlayerState);
		}
		GateGameState->AddLogOutPlayer(ExitingPlayerState);
	}

	int32 AliveCount = GateGameState ? GateGameState->GetPlayerList().GetAlivePlayerCount() : 0;
	if (AliveCount == 0)
	{
		UE_LOG(LogGameMode, Log, TEXT("Last player left. Gate End."));
		GetWorld()->GetTimerManager().SetTimer(LogOutGateEndTimer, this, &AFHGateGameModeBase::GateEnd, 0.2f, false);
	}
	TotalPlayerCount--;

	Super::Logout(Exiting);
}

void AFHGateGameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	ErrorMessage = TEXT("Room Is Already Started.");
}

void AFHGateGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GateTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(GateTimerHandle);
	}

	if (ExitTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ExitTimerHandle);
	}

	if (PowerLevelIncreaseEventHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(PowerLevelIncreaseEventHandle);
	}

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}




void AFHGateGameModeBase::SetLoadRate()
{
	if (GateGeneratorInstance)
	{
		float DeltaProgressRate = (GateGeneratorInstance->GetLoadingRate() / 2.0f) - LastGateProgress;
		//PRINT_LOG(TEXT("GetLoadingRate: %f, LastGateProgress: %f"), GateGeneratorInstance->GetLoadingRate() / 2.0f, LastGateProgress);
		LastGateProgress += DeltaProgressRate;

		GateGameState->LoadRate += DeltaProgressRate;
		GateGameState->OnRep_LoadRate();
	}
}

void AFHGateGameModeBase::BackToLobby_Implementation()
{
	//GateEnd();
	/*if (GateGeneratorInstance)
	{
		GateGeneratorInstance->Unload();
	}*/

	UFHLevelAsset* LevelAsset = UFHBlueprintFunctionLibrary::GetLevelAsset();
	check(LevelAsset);

	//UFHGameInstance* GameInstance = Cast<UFHGameInstance>(GetGameInstance());
	//if (GameInstance)
	//{
	//	GameInstance->ActiveJoinSession();
	//}

	//ClearAllVoice();

	UE_LOG(LogGate, Warning, TEXT("Serer Back To Lobby"));

	GetWorld()->ServerTravel(LevelAsset->GetEndLobby());
}

void AFHGateGameModeBase::PlayerReady_Implementation(APlayerController* PlayerController)
{
	UE_LOG(LogGate, Warning, TEXT("PlayerController Is Ready."));
	if (GateGameState)
	{
		// 플레이어가 죽었을 때 플레이어 리스트에서 삭제하는 것이 아닌 플래그만 바꿔주는 식으로 변경.
		// 그에 따라 컨트롤러별 플래그를 함께 들고 있는 struct 타입으로 추가 제작해 구조 변경.
		AFHPlayerController* FHPC = Cast<AFHPlayerController>(PlayerController);
		if (FHPC)
		{
			FPlayerInfo PlayerInfo;
			PlayerInfo.PlayerState = FHPC->GetPlayerState<AFHPlayerStateBase>();
			PlayerInfo.PlayerGateState = EPlayerGateState::Alive;
			GateGameState->GetPlayerList().AddPlayer(PlayerInfo);

			//if (AFHPlayerStateBase* PS = FHPC->GetPlayerState<AFHPlayerStateBase>())
			//{
			//	PS->StartSetting();
			//}
		}
		ReadyPlayerCount++;

		GateGameState->LoadRate += 0.5f / static_cast<float>(TotalPlayerCount);
		GateGameState->OnRep_LoadRate();

		//TryGateStart();
	}

}

void AFHGateGameModeBase::OnGatePowerLevelChanged()
{
	OnGatePowerLevelChangeDelegate.Broadcast();
}

void AFHGateGameModeBase::ForceClearGate()
{
	if (GateGameState)
	{
		GateGameState->ClearPlayerList();
	}

	BackToLobby();
}

void AFHGateGameModeBase::TryGateStart()
{
	if (GateState == EGateState::InPlay)
	{
		return;
	}

	GateState = EGateState::InPlay;

	if (GateGameState)
	{
		if ((TotalPlayerCount != ReadyPlayerCount) || !bIsGenerateComplete)
		{
			return;
		}
	}

	UE_LOG(LogGate, Warning, TEXT("Regist Gate Timer."));

	if (GateGameState)
	{
		GateGameState->OnRep_TimeLimit();
	}

	TArray<FPlayerInfo>& PlayerList = GateGameState->GetPlayerList().GetPlayerList();
	for (FPlayerInfo& Info : PlayerList)
	{
		AFHPlayerBase* player = Cast<AFHPlayerBase>(Info.PlayerState->GetPawn());
		player->AddInputMappingContext();
	}

	// Start updating minimap visibility
	if (GateGeneratorInstance)
	{
		GetWorld()->GetTimerManager().SetTimer(UpdateMinimapVisibilityHandle,
			FTimerDelegate::CreateLambda([this]()
		{
			GateGeneratorInstance->bUpdateRoomVisibility = true;
		}),
			0.1f,
			false
		);
	}

	// Regist Gate Timer
	GetWorld()->GetTimerManager().SetTimer(GateTimerHandle,
		FTimerDelegate::CreateLambda([&]()
	{
		if (GateGameState)
		{
			if (GateGameState->TimeLimit == 0)
			{
				GateEnd();
				return;
			}

			GateGameState->TimeLimit--;
			GateGameState->OnRep_TimeLimit();
		}
	}),
		1.0f,
		true,
		5.0f
	);


	if (GateGeneratorInstance)
	{
		// Regist Increase PowerLevel Event
		GetWorld()->GetTimerManager().SetTimer(
			PowerLevelIncreaseEventHandle,
			FTimerDelegate::CreateUObject(this, &ThisClass::IncreaseGatePowerLevel, false),
			GateGeneratorInstance->SelectedDescriptor->PowerLevelIncreaseCool,
			true,
			GateGeneratorInstance->SelectedDescriptor->InitialPowerLevelIncreasingDelay
		);
	}
	

	for (FPlayerInfo& Info : PlayerList)
	{
		AFHPlayerController* PC = Cast<AFHPlayerController>(Info.PlayerState->GetPlayerController());
		if (PC)
		{
			PC->FadeOut();
		}
	}

	// TODO: 여기서 GateGameState의 OnGatePowerLevelChanged Delegate에 크리쳐 스폰 함수 등록해줘야함.
	//GateGameState->GatePowerLevelChangeDelegate.AddDynamic(this, &AFHGateGameModeBase::SpawnCreature);
}

void AFHGateGameModeBase::GateEnd()
{
	UE_LOG(LogGate, Warning, TEXT("UnRegist Gate Timer."));
	UE_LOG(LogGate, Warning, TEXT("Gate Is Collapsed."));
	if (GateTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(GateTimerHandle);
	}

	if (ExitTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ExitTimerHandle);
	}

	if (PowerLevelIncreaseEventHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(PowerLevelIncreaseEventHandle);
	}

	EnterReadyPlayerCount = 0;

	if (GateGameState)
	{
		if (GateGameState->TimeLimit == 0 && GateGameState)
		{
			// TODO1: 탈출한 플레이어 체크 후 탈출 인원수에 따라 추가 조치.

			// TODO2: 탈출 못한 플레이어에 대해 모든 아이템을 떨구고 사망 처리.
			TArray<AFHPlayerStateBase*> AlivePlayerList;
			GateGameState->GetAlivePlayerList(AlivePlayerList);
			for (AFHPlayerStateBase* PS : AlivePlayerList)
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PS, GET_GAMEPLAY_TAG_GAMEPLAYEVENT_DEATH, FGameplayEventData());
				GateGameState->AddDeathCount();
			}
		}


		GateGameState->GateEnd();

		// TODO: Play Gate Exploration Result UI and Event.  @ 2025.07.28 권세민
		// GateGameState->ShowExplorationResult();
		/*TArray<FPlayerInfo>& PlayerList = GateGameState->GetPlayerList().GetPlayerList();
		for (FPlayerInfo& Info : PlayerList)
		{
			if (Info.PlayerState)
			{
				AFHPlayerController* PC = Cast<AFHPlayerController>(Info.PlayerState->GetPlayerController());
				if (PC && PC->GetPawn())
				{
					PC->SeeGateEnd();
				}
			}
		}*/

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			AFHPlayerController* PC = Cast<AFHPlayerController>(It->Get());
			if (IsValid(PC))
			{
				UE_LOG(LogGate, Log, TEXT("Calling SeeGateEnd for %s"), *PC->GetName());
				PC->SeeGateEnd();
			}
			else
			{
				UE_LOG(LogGate, Warning, TEXT("GateEnd: Found an invalid PlayerController during iteration. Skipping."));
			}
		}

		AFHPropManager* PropManager = UFHBlueprintFunctionLibrary::GetPropManager(GetWorld());
		if (PropManager)
		{
			AFHExit* ExitDoor = PropManager->GetProp<AFHExit>(TEXT("ExitDoor"));
			if (ExitDoor)
			{
				ExitDoor->CloseDoor();
			}
		}
	}
	
}


void AFHGateGameModeBase::IncreaseGatePowerLevel(bool bIsCoreDestroyed)
{
	FFHPowerLevelIncreaseTable* PowerIncreaseTable = IncreasePowerTable->FindRow<FFHPowerLevelIncreaseTable>(
		FName(*(GateGameInstance->GetGateRankString())), TEXT("IncreasePowerRow"));
	if (PowerIncreaseTable)
	{
		int32 IncreaseValue;
		if (bIsCoreDestroyed)
		{
			IncreaseValue = PowerIncreaseTable->IncreasePerCoreDestroy;
		}
		else
		{
			switch (PowerIncreaseStep)
			{
			case 1:
			{
				IncreaseValue = PowerIncreaseTable->Increase1;
				break;
			}
			case 2:
			{
				IncreaseValue = PowerIncreaseTable->Increase2;
				break;
			}
			case 3:
			{
				IncreaseValue = PowerIncreaseTable->Increase3;
				break;
			}
			case 4:
			{
				IncreaseValue = PowerIncreaseTable->Increase4;
				break;
			}
			default:
			{
				IncreaseValue = PowerIncreaseTable->Increase4;
				break;
			}
			}
		}

		GatePowerLevel += static_cast<float>(IncreaseValue);
		PowerIncreaseStep++;
		OnGatePowerLevelChanged();
	}
}

void AFHGateGameModeBase::PlayerEscape(AFHPlayerController* Controller)
{
	FPlayerInfoArray& PlayerArray = GateGameState->GetPlayerList();
	bool Result = PlayerArray.SetPlayerEscape(Controller->GetPlayerState<AFHPlayerStateBase>());
	//if (Result)
	//{
	//	APawn* PlayerOldPawn = Controller->GetPawn();
	//	PlayerOldPawn->Destroy();
	//}

	int32 AliveCount = PlayerArray.GetAlivePlayerCount();
	//for (FPlayerInfo& Info : PlayerList)
	//{
	//	if (Info.PlayerGateState == EPlayerGateState::Alive)
	//	{
	//		AliveCount++;
	//	}
	//}

	Controller->EnterObserverMode();

	if (AliveCount == 0)
	{
		GetWorld()->GetTimerManager().SetTimer(EscapeGateEndTimer, this, &AFHGateGameModeBase::GateEnd, 0.2f, false);
	}
}

void AFHGateGameModeBase::PlayerDied(AFHPlayerController* Controller)
{
	FPlayerInfoArray& PlayerArray = GateGameState->GetPlayerList();
	bool Result = PlayerArray.SetPlayerDied(Controller->GetPlayerState<AFHPlayerStateBase>());
	if (Result)
	{
		GateGameState->AddDeathCount();
		GateGameState->MulticastOnPlayerDied(Controller->GetPlayerState<APlayerState>());
	}

	//TArray<FPlayerInfo>& PlayerList = GateGameState->GetPlayerList();
	//for (FPlayerInfo& Info : PlayerList)
	//{
	//	AFHPlayerController* PC = Cast<AFHPlayerController>(Info.PlayerState->GetPlayerController());
	//	if (PC == Controller)
	//	{
	//		// 플레이어가 사망했을 경우 리스트에서 지우는 것이 아닌 플래그만 변경하도록 수정.
	//		Info.PlayerGateState = EPlayerGateState::Died;
	//		GateGameState->AddDeathCount();

	//		/** TODO: 모든 플레이어에게 해당 플레이어가 죽었다는 이벤트를 전송해야함.그리고 각각의 클라이언트에서 해당하는 이벤트 실행.
	//		 -> MulticastOnPlayerDied_Implementation */
	//		GateGameState->MulticastOnPlayerDied(Info.PlayerState);
	//		break;
	//	}
	//}

	int32 AliveCount = PlayerArray.GetAlivePlayerCount();
	//int32 AliveCount = 0;
	//for (FPlayerInfo& Info : PlayerList)
	//{
	//	if (Info.PlayerGateState == EPlayerGateState::Alive)
	//	{
	//		AliveCount++;
	//	}
	//}

	if (AliveCount == 0)
	{
		GetWorld()->GetTimerManager().SetTimer(PlayerDiedGateEndTimer, this, &AFHGateGameModeBase::GateEnd, 0.2f, false);
	}
}

void AFHGateGameModeBase::PossessObserverPawn(AFHPlayerController* Controller)
{
	if (!Controller) return;

	APawn* OldPawn = Controller->GetPawn();
	Controller->UnPossess();
	if (OldPawn)
	{
		OldPawn->Destroy();
	}

	//Controller->ChangeState(NAME_Spectating);

	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	if (Controller->PlayerCameraManager)
	{
		SpawnLocation = Controller->PlayerCameraManager->GetCameraLocation();
		SpawnRotation = Controller->PlayerCameraManager->GetCameraRotation();
	}

	APawn* NewObserverPawn = GetWorld()->SpawnActor<APawn>(SpectatorClass, SpawnLocation, SpawnRotation);
	if (NewObserverPawn)
	{
		Controller->Possess(NewObserverPawn);
		AFHObserverPawn* op = Cast<AFHObserverPawn>(NewObserverPawn);
		if (op)
		{
			op->DelayedInitializeHUD();
		}
	}
	else
	{
		UE_LOG(LogGameMode, Error, TEXT("Failed to possess ObserverPawn for %s."), *Controller->GetName());
	}
}

void AFHGateGameModeBase::InformCoreDestroyed()
{
	UE_LOG(LogGate, Warning, TEXT("Core Is Destroyed."));
	if (GateGameState)
	{
		GateGameState->CurrentCoreCount--;
		GateGameState->OnRep_CurrentCoreCount();

		if (GateGameState->CurrentCoreCount <= 0)
		{
			// ======== Play Sound =========
			AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
			if (SoundManager)
			{
				TArray<AFHPlayerStateBase*> Alives;
				GateGameState->GetAlivePlayerList(Alives);
				for (AFHPlayerStateBase* alive : Alives)
				{
					AActor* player = alive->GetPawn();
					if (!player) continue;
					SoundManager->PlaySoundAtLocationMulticastByTag(GET_GAMEPLAY_TAG("Sound.Event.GateCollapse"), player->GetActorLocation(), 1.0f, true, 3.0f, player);
				}
			}
			


			// Not Use
			//UE_LOG(LogGate, Warning, TEXT("All Cores Destroyed."));

			//GateGameState->bIsCollapsing = true;
			//GateGameState->OnRep_IsCollapsing();

			//GateGameState->TimeLimit = 180;
			//GateGameState->OnRep_TimeLimit();

			//GetWorld()->GetTimerManager().PauseTimer(GateTimerHandle);
			//GetWorld()->GetTimerManager().SetTimer(ExitTimerHandle, 
			//	[&](){
			//		if (GateGameState)
			//		{
			//			if (GateGameState->TimeLimit == 0)
			//			{
			//				GateEnd();
			//				return;
			//			}

			//			GateGameState->TimeLimit--;
			//			GateGameState->OnRep_TimeLimit();
			//		}
			//	}, 
			//	1.0f,
			//	true
			//	);

			//if (!GateGameState->bIsGateOpen)
			//{
			//	UE_LOG(LogGate, Warning, TEXT("Gate Is Opened."));
			//	GateGameState->bIsGateOpen = true;
			//	// TODO: Open Gate
			//}

			// TODO: 연출 추가.
		}
		else
		{
			IncreaseGatePowerLevel(true);
		}
	}
}








// ================= Gate Generation =================

void AFHGateGameModeBase::InitGenerator(struct FFHGateSpecificationTableRow* GateSpecification)
{
	//GateGenerator = CastChecked<AFHDungeonGenerator>(GetWorld()->SpawnActor(GateGeneratorActorClass));

	if (GeneratorClasses.Contains(DungeonGeneratorType))
	{
		GateGeneratorInstance = Cast<AFHDungeonGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), GeneratorClasses[DungeonGeneratorType]));
	}

	if (GateGeneratorInstance == nullptr)
	{
		GateGeneratorInstance = Cast<AFHDungeonGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), DefaultGeneratorClass));
	}

	ensureMsgf(GateGeneratorInstance, TEXT("No Generator in InGate Level."));

	GateGeneratorInstance->OnLoadRateChangedDelegate.AddDynamic(this, &AFHGateGameModeBase::SetLoadRate);

	// 대충 게이트 타입 선택
	// 대충 GateDescriptor 선정
	TArray<float> GateThemeWeights;
	TArray<FString> GateThemeNames;
	for (const FGateThemeSpecificationSet& set : GateSpecification->GateTheme)
	{
		GateThemeWeights.Add(set.Weight);
		GateThemeNames.Add(set.ThemeName);
	}

	int32 ThemeIdx = UFHBlueprintFunctionLibrary::GetRandIndexUsingDiscreteDistribution(GateThemeWeights);
	if (ThemeIdx == -1)
	{
		checkf(false, TEXT("No GateTheme found."));
		return;
	}

	FString TargetThemeName = GateThemeNames[ThemeIdx];
	UFHGameInstance* GameInstance = Cast<UFHGameInstance>(GetGameInstance());
	if (GameInstance == nullptr)
	{
		checkf(false, TEXT("Failed to get FHGameInstance."));
		return;
	}
	EGateRank TargetGateRank = GameInstance->GetGateRank();

	GateGeneratorInstance->SettingUpGate(TargetThemeName, TargetGateRank, GateSpecification);
	GateGeneratorInstance->GenerateGate();

	//GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	//{
	//	// ======== Play Sound =========
	//	AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
	//	if (SoundManager)
	//	{
	//		SoundManager->PlaySound2DMulticastByTag(GET_GAMEPLAY_TAG("Sound.Event.LoadingVehicle"));
	//	}
	//});
}

void AFHGateGameModeBase::ReplacePlayerCharacters()
{
	TArray<AActor*> starts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), starts);

	int32 playerToStartNum = GetWorld()->GetNumPlayerControllers();
	if (starts.Num() < playerToStartNum)
	{
		PRINT_LOG(TEXT("Warning! There are %d Players to spawning but APlayerStarts are only %d exist."), playerToStartNum, starts.Num());
		ensureMsgf(starts.Num() > 0, TEXT("Critical error: There are %d Players to spawning but APlayerStarts are only %d exist."), playerToStartNum, starts.Num());
	}

	// for debugging
	if (starts.IsEmpty())
	{
		return;
	}

	auto startIter = starts.CreateIterator();
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AFHPlayerController* PlayerController = Cast<AFHPlayerController>(Iterator->Get());
		if (PlayerController)
		{
			//ChoosePlayerStart(PlayerController);
			//AActor* PlayerStart = FindPlayerStart(PlayerController);
			//RestartPlayerAtPlayerStart(PlayerController, PlayerStart);

			FVector StartLoc = (*startIter)->GetActorLocation();
			FRotator StartRot = (*startIter)->GetActorRotation();
			PlayerController->GetPawn()->TeleportTo(StartLoc, StartRot);
			++startIter;
		}
	}
}

void AFHGateGameModeBase::PlayerEnterReady()
{
	EnterReadyPlayerCount++;
	PRINT_LOG(TEXT("Player Ready Count: %d"), EnterReadyPlayerCount);
	if (EnterReadyPlayerCount >= TotalPlayerCount)
	{
		PRINT_LOG(TEXT("Player Ready Count: %d, TotalPlayerCount: %d"), EnterReadyPlayerCount, TotalPlayerCount);
		PRINT_LOG(TEXT("Start BackToLobby."));
		BackToLobby();
	}
}

void AFHGateGameModeBase::MakePlayerReady()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AFHPlayerController* PlayerController = Cast<AFHPlayerController>(Iterator->Get());
		if (PlayerController)
		{
			PlayerController->TurnOnPhysics();
			PlayerController->ClientRoomLoadEnd();
		}
	}
}

void AFHGateGameModeBase::GenerateCompleted()
{
	if (GateGeneratorInstance == nullptr)
	{
		PRINT_LOG(TEXT("Cannot find Generator."));
		return;
	}


	UE_LOG(LogGate, Warning, TEXT("Dungeon Generate Success."));
	if (GateGameState)
	{
		GateGameState->CurrentCoreCount = GateGeneratorInstance->SelectedDescriptor->DungeonCoreCount;
		GateGameState->OnRep_CurrentCoreCount();

		bIsGenerateComplete = true;

		MakePlayerReady();
		TryGateStart();
		//GateState = EGateState::InPlay;
	}

	OnGateLoadingCompleteEvent.Broadcast();

	// 클라가 APlayerStart 리플리케이트 받는 충분한 시간 보장
	/*GetWorld()->GetTimerManager().SetTimer(GateStartDelayTimerHandle,
		[&]()
		{

		},
		0.5f,
		false
	);*/
}

void AFHGateGameModeBase::UnloadDungeonAndTurnOffPhysics()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AFHPlayerController* PlayerController = Cast<AFHPlayerController>(Iterator->Get());
		if (PlayerController)
		{
			PlayerController->TurnOffPhysics();
			PlayerController->StopMinimapUpdate();
		}
	}

	// if (GateGeneratorInstance) GateGeneratorInstance->UnloadDungeon();
	if (GateGeneratorInstance)
	{
		GateGeneratorInstance->Destroy();
	}


	// GStar Test
	TArray<AActor*> Creatures;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFHCreatureBase::StaticClass(), Creatures);
	for (AActor* creature : Creatures)
	{
		AFHCreatureBase* _creature = Cast<AFHCreatureBase>(creature);
		if (_creature)
		{
			_creature->Destroy();
		}
	}
}

void AFHGateGameModeBase::PlayerLoadEnd(AController* LoadedPlayer)
{
	//if(GateState == EGateState::InPlay)
	//{
	//	return;
	//}
	//TryGateStart();
	//GateState = EGateState::InPlay;

	/*if (LoadedPlayer != nullptr)
	{
		if (!LoadedPlayer || GateState == EGateState::InPlay)
		{
			return;
		}

		if (LoadedPlayers.Contains(LoadedPlayer))
		{
			return;
		}

		LoadedPlayers.Add(LoadedPlayer);
	}*/

	//UE_LOG(LogGameMode, Log, TEXT("Player %s has finished loading. (%d / %d)"), *LoadedPlayer->GetName(), LoadedPlayers.Num(), TotalPlayerCount);
	/*if (LoadedPlayers.Num() >= TotalPlayerCount && bIsGenerateComplete)
	{
		UE_LOG(LogGameMode, Log, TEXT("All players ready and dungeon generated. Starting the game."));

		GetWorld()->GetTimerManager().SetTimer(StartGameTimerHandle, this, &AFHGateGameModeBase::TryGateStart, 0.2f, false);

		return;
	}*/

	//TryGateStart();
}

