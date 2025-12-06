// Copyright F Rank Hunter. All Rights Reserved.


#include "Core/FHGateGameStateBase.h"
#include "Core/FHPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "FRankHunter.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHUIManager.h"
#include "Core/FHPlayerStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Core/FHGateGameModeBase.h"
#include "UI/Gate/FHExplorationResult.h"
#include "UI/Gate/FHLoading.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/FHGameplayTags.h"
#include "Core/FHGameInstance.h"

#include "SimpleSaveKitFunctionLibrary.h"
#include "Core/GameDataSubsystem.h"
#include "Core/GameSaveData.h"

DEFINE_LOG_CATEGORY(LogGateState);

AFHGateGameStateBase::AFHGateGameStateBase()
	: TimeLimit(0),
	  bIsCollapsing(false),
	  bIsGateOpen(false)
{

}

void AFHGateGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		UFHBlueprintFunctionLibrary::LoadObject(this);

		UFHGameInstance* GameInstance = GetGameInstance<UFHGameInstance>();
		if (GameInstance)
		{
			PlayerMoney = GameInstance->GetCurrentMoneyForGate();
			RequiredMoney = GameInstance->GetRequiredMoneyForGate();

			OnRep_PlayerMoney();
			OnRep_RequiredMoney();
		}
	}
	
	// Not Use.
	//AFHPlayerController* Controller = Cast<AFHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//if (Controller)
	//{
	//	Controller->CreateLoadingUI();
	//}
	UFHGameInstance* GameInstance = GetGameInstance<UFHGameInstance>();
	if (!GameInstance)
	{
		return;
	}
	GameInstance->InitLoadingWidget();
}

void AFHGateGameStateBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (HasAuthority())
	{
		UFHBlueprintFunctionLibrary::SaveObject(this);
	}
}

void AFHGateGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFHGateGameStateBase, PlayerMoney);
	DOREPLIFETIME(AFHGateGameStateBase, RequiredMoney);
	DOREPLIFETIME(AFHGateGameStateBase, TotalMagicStoneValue);
	DOREPLIFETIME(AFHGateGameStateBase, TimeLimit);
	DOREPLIFETIME(AFHGateGameStateBase, bIsGateOpen);
	DOREPLIFETIME(AFHGateGameStateBase, bIsCollapsing);
	DOREPLIFETIME(AFHGateGameStateBase, PlayerList);
	DOREPLIFETIME(AFHGateGameStateBase, CurrentCoreCount);
	DOREPLIFETIME(AFHGateGameStateBase, LoadRate);
	DOREPLIFETIME(AFHGateGameStateBase, RoomDisclosedStates);
}

FString AFHGateGameStateBase::GetSaveSlot() const
{
	return TEXT("GameState");
}

bool AFHGateGameStateBase::IsGlobal() const
{
	return true;
}

void AFHGateGameStateBase::SerializeData(FArchive& Ar)
{
	Ar.UsingCustomVersion(FFRankHunterCustomVersion::GUID);
	
	UGameInstance* GameInstance = GetGameInstance();
	check(GameInstance);

	UGameDataSubsystem* GameData = GameInstance->GetSubsystem<UGameDataSubsystem>();
	check(GameData);

	if (Ar.CustomVer(FFRankHunterCustomVersion::GUID) >= FFRankHunterCustomVersion::FirstVersion)
	{
		//언젠가 로비상태랑 통합

		GameData->GetGameSaveData().PlayerMoney = PlayerMoney;
		GameData->GetGameSaveData().RequiredMoney = RequiredMoney;
		GameData->GetGameSaveData().LoopCount = LoopCount;
		GameData->GetGameSaveData().LoopCount_AtCurrentRank = LoopCount_AtCurrentRank;
		GameData->GetGameSaveData().DeathCount = DeathCount;
		GameData->GetGameSaveData().CurrentReturnedCoreCount = CurrentReturnedCoreCount;
		GameData->GetGameSaveData().TeamLicenseRank = TeamLicenseRank;
		GameData->GetGameSaveData().SelectedPawnKey = SelectedPawnKey;

		GameData->GetGameSaveData().Serialize(Ar);


		PlayerMoney = GameData->GetGameSaveData().PlayerMoney;
		RequiredMoney = GameData->GetGameSaveData().RequiredMoney;
		LoopCount = GameData->GetGameSaveData().LoopCount;
		LoopCount_AtCurrentRank = GameData->GetGameSaveData().LoopCount_AtCurrentRank;
		DeathCount = GameData->GetGameSaveData().DeathCount;
		CurrentReturnedCoreCount = GameData->GetGameSaveData().CurrentReturnedCoreCount;
		TeamLicenseRank = GameData->GetGameSaveData().TeamLicenseRank;
		SelectedPawnKey = GameData->GetGameSaveData().SelectedPawnKey;
	}
}

void AFHGateGameStateBase::SaveState()
{
	if (!HasAuthority())
	{
		return;
	}

	UFHBlueprintFunctionLibrary::SaveObject(this);

	TArray<APlayerState*> PlayerStates = PlayerArray;
	for (APlayerState* PS : PlayerStates)
	{
		if (AFHPlayerStateBase* FHPlayerStateBase = Cast<AFHPlayerStateBase>(PS))
		{
			FHPlayerStateBase->TrySave();
		}
	}

	for (size_t i = 0; i < FMath::Min(LogOutPlayerSlotNameArray.Num(), LogOutPlayerArray.Num()); i++)
	{
		UGameplayStatics::SaveGameToSlot(LogOutPlayerArray[i], LogOutPlayerSlotNameArray[i], 0);
	}

}



void AFHGateGameStateBase::AddDeathCount()
{
	++DeathCount;
}

void AFHGateGameStateBase::AddPlayer(FPlayerInfo& PlayerInfo)
{
	PlayerList.AddPlayer(PlayerInfo);
}

void AFHGateGameStateBase::ClearPlayerList()
{
	if (HasAuthority())
	{
		PlayerList.ClearArray();
	}
}

void AFHGateGameStateBase::GetAlivePlayerList(TArray<AFHPlayerStateBase*>& OutList)
{
	PlayerList.GetAlivePlayerList(OutList);
}

void AFHGateGameStateBase::GetDiedPlayerList(TArray<AFHPlayerStateBase*>& OutList)
{
	PlayerList.GetDiedPlayerList(OutList);
}

void AFHGateGameStateBase::GetNonAlivePlayerList(TArray<AFHPlayerStateBase*>& OutList)
{
	PlayerList.GetNonAlivePlayerList(OutList);
}

void AFHGateGameStateBase::MulticastOnPlayerDied_Implementation(APlayerState* DiedPlayerState)
{
	if (!DiedPlayerState)
	{
		return;
	}

	// TODO : 모든 플레이어에게 해당 플레이어가 죽었다는 이벤트를 전송해야함. 그리고 각각의 클라이언트에서 해당하는 이벤트 실행.

}

void AFHGateGameStateBase::AddLogOutPlayer(APlayerState* LogOutPlayer)
{
	const FString& GameID = UFHBlueprintFunctionLibrary::GetGameName(this);

	LogOutPlayerArray.Add(USimpleSaveKitFunctionLibrary::GetSaveGameFromObject(GameID, LogOutPlayer, false));
	LogOutPlayerSlotNameArray.Add(USimpleSaveKitFunctionLibrary::GetSlotName(GameID, LogOutPlayer));

	for (size_t i = 0; i < PlayerList.GetPlayerList().Num(); i++)
	{
		if (PlayerList.GetPlayerList()[i].PlayerState == LogOutPlayer)
		{
			PlayerList.GetPlayerList()[i].PlayerGateState = EPlayerGateState::Died;
		}
	}
}

void AFHGateGameStateBase::ShowExplorationResult_Implementation()
{
	AFHGateGameModeBase* GateGameMode = Cast<AFHGateGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GateGameMode)
	{
		GateGameMode->UnloadDungeonAndTurnOffPhysics();
	}

	AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	if (UIManager)
	{
		UFHExplorationResult* Widget = UIManager->GetWidget<UFHExplorationResult>(TEXT("ExplorationResult"));
		if (Widget)
		{
			Widget->SetExplorationResult(PlayerList.GetPlayerList());
			Widget->ShowResult();
		}
	}
}

void AFHGateGameStateBase::GateEnd()
{
	SaveState();
}

//void AFHGateGameStateBase::ChangeTimeLimit(float deltaTime)
//{
//	TimeLimit -= deltaTime;
//	OnRep_TimeLimit();
//
//	if (TimeLimit <= 0 && HasAuthority())
//	{	
//		TArray<AFHPlayerStateBase*> AlivePlayerList;
//		GetAlivePlayerList(AlivePlayerList);
//		for (AFHPlayerStateBase* PS : AlivePlayerList)
//		{
//			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PS, GET_GAMEPLAY_TAG_GAMEPLAYEVENT_DEATH, FGameplayEventData());
//			AddDeathCount();
//		}
//
//		AFHGateGameModeBase* GateGameMode = Cast<AFHGateGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
//		if (GateGameMode)
//		{
//			GateGameMode->GateEnd();
//		}
//
//	}
//}

void AFHGateGameStateBase::OnRep_TimeLimit()
{
	/*if (!bIsCollapsing)
	{
		TArray<int32> times = GetModifiedTimeLimit();

		UE_LOG(LogGateState, Warning, TEXT("Gate Left Time(H/M): %02d:%02d"), times[0], times[1]);
	}
	else
	{
		TArray<int32> times = GetModifiedTimeLimit();

		UE_LOG(LogGateState, Warning, TEXT("Gate Left Time(H/M): %02d:%02d"), times[0], times[1]);
	}*/

	OnTimerTickEvent.Broadcast(TimeLimit);
}

void AFHGateGameStateBase::OnRep_LoadRate()
{
	OnLoadRateReplicatedDelegate.Broadcast(LoadRate);

	//AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	//if (!UIManager)
	//{
	//	return;
	//}
	//UFHLoading* Widget = UIManager->GetWidget<UFHLoading>(TEXT("Loading"));
	//if (!Widget)
	//{
	//	return;
	//}

	//Widget->SetProgress(LoadRate);
}

void AFHGateGameStateBase::OnRep_IsCollapsing()
{

}

void AFHGateGameStateBase::OnRep_IsGateOpen()
{

}

FString AFHGateGameStateBase::GetModifiedStringFromTimeLimit()
{
	//int32 ModifiedTimeLimit = TimeLimit * 72;
	//int32 Hour = ModifiedTimeLimit / 3600;
	//int32 Minute = (ModifiedTimeLimit % 3600) / 60;
	//int32 Second = (ModifiedTimeLimit % 3600) % 60;

	//return FString::Printf(TEXT("%02d:%02d:%02d"), Hour, Minute, Second);

	int32 Minute = TimeLimit / 60;
	int32 Second = TimeLimit % 60;

	return FString::Printf(TEXT("%02d:%02d"), Minute, Second);
}

void AFHGateGameStateBase::OnRep_PlayerMoney()
{

}

void AFHGateGameStateBase::OnRep_RequiredMoney()
{

}

void AFHGateGameStateBase::OnRep_CurrentCoreCount()
{
	OnCoreCountChangedEvent.Broadcast(CurrentCoreCount);
}

// FastArraySerializer


void FPlayerInfoArray::GetAlivePlayerList(TArray<AFHPlayerStateBase*>& InPlayerArr)
{
	for (FPlayerInfo& Info : Players)
	{
		if (Info.PlayerGateState == EPlayerGateState::Alive)
		{
			InPlayerArr.Add(Info.PlayerState);
		}
	}
}

void FPlayerInfoArray::GetDiedPlayerList(TArray<AFHPlayerStateBase*>& InPlayerArr)
{
	for (FPlayerInfo& Info : Players)
	{
		if (Info.PlayerGateState == EPlayerGateState::Died)
		{
			InPlayerArr.Add(Info.PlayerState);
		}
	}
}

void FPlayerInfoArray::GetNonAlivePlayerList(TArray<AFHPlayerStateBase*>& InPlayerArr)
{
	for (FPlayerInfo& Info : Players)
	{
		if (Info.PlayerGateState != EPlayerGateState::Alive)
		{
			InPlayerArr.Add(Info.PlayerState);
		}
	}
}

int32 FPlayerInfoArray::GetAlivePlayerCount()
{
	int32 Count = 0;
	for (FPlayerInfo& Info : Players)
	{
		if (Info.PlayerGateState == EPlayerGateState::Alive)
		{
			Count++;
		}
	}
	return Count;
}

const EPlayerGateState FPlayerInfoArray::GetPlayerGateStateByIndex(int32 Index)
{
	return Players[Index].PlayerGateState;
}

const EPlayerGateState FPlayerInfoArray::GetPlayerGateStateByPlayerState(AFHPlayerStateBase* PlayerState)
{
	FPlayerInfo* Info = Players.FindByPredicate([PlayerState](const FPlayerInfo& Info) {
		return Info.PlayerState == PlayerState;
		});

	if (Info == nullptr)
	{
		return EPlayerGateState::Died;
	}

	return Info->PlayerGateState;
}

bool FPlayerInfoArray::SetPlayerEscape(AFHPlayerStateBase* Player)
{
	int32 Index = Players.IndexOfByPredicate([Player](const FPlayerInfo& Info) {
		return Info.PlayerState == Player;
	});

	if (Index == INDEX_NONE)
	{
		return false;
	}

	Players[Index].PlayerGateState = EPlayerGateState::Escape;
	OnPlayerStateChangedDelegate.Broadcast(Index);
	MarkItemDirty(Players[Index]);

	return true;
}

bool FPlayerInfoArray::SetPlayerDied(AFHPlayerStateBase* Player)
{
	int32 Index = Players.IndexOfByPredicate([Player](const FPlayerInfo& Info) {
		return Info.PlayerState == Player;
		});

	if (Index == INDEX_NONE)
	{
		return false;
	}

	if (Players[Index].PlayerGateState == EPlayerGateState::Escape)
	{
		return false;
	}

	Players[Index].PlayerGateState = EPlayerGateState::Died;
	OnPlayerStateChangedDelegate.Broadcast(Index);
	MarkItemDirty(Players[Index]);

	return true;
}

void FPlayerInfoArray::AddPlayer(FPlayerInfo& PlayerInfo)
{
	for (int i = 0; i < Players.Num(); ++i)
	{
		if (Players[i].PlayerState == PlayerInfo.PlayerState)
		{
			return;
		}
	}

	Players.Add(PlayerInfo);
	MarkArrayDirty();
}

void FPlayerInfoArray::IncreaseMagicStoneValue(AFHPlayerStateBase* Player, float IncreaseValue)
{
	int32 Index = Players.IndexOfByPredicate([Player](const FPlayerInfo& Info) {
		return Info.PlayerState == Player;
		});

	if (Index == INDEX_NONE)
	{
		return;
	}

	Players[Index].MiningAmount += IncreaseValue;
	MarkItemDirty(Players[Index]);

	PRINT_LOG(TEXT("Player %s Increased Magic Stone Value : %f"), *Player->GetPlayerName(), Players[Index].MiningAmount);
}

void FPlayerInfoArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		OnPlayerStateChangedDelegate.Broadcast(Index);
	}
}




bool AFHGateGameStateBase::IsRoomDisclosed(int64 RoomID)
{
	if (RoomDisclosedStates.Num() <= RoomID)
	{
		return false;
	}
	return static_cast<bool>(RoomDisclosedStates[RoomID]);
}

void AFHGateGameStateBase::NotifyMagicStoneDestroyed(AFHPlayerStateBase* PlayerState, float Value)
{
	PlayerList.IncreaseMagicStoneValue(PlayerState, Value);
}

void AFHGateGameStateBase::Server_NotifyRoomDisclosed_Implementation(int64 RoomID)
{
	if (RoomDisclosedStates.Num() <= RoomID)
	{
		return;
	}
	RoomDisclosedStates[RoomID] = true;
}