// Copyright F Rank Hunter.. All Rights Reserved.


#include "FH_GS_LobbyGameState.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Core/FHPlayerController.h"
#include "Core/ManagerActorRegistrySubsystem.h"
#include "UI/Lobby/FH_W_GateEnterRequest.h"
#include "Kismet/GameplayStatics.h"
#include "Props/FHDisplayBoard.h"
#include "Props/FHKioskBase.h"
#include "Core/FHUIManager.h"
#include "UI/Lobby/FHPaymentBase.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "Item/Actors/FHPickupItemActor.h"
#include "FRankHunter.h"
#include "Item/Actors/FHBackpack.h"
#include "DataAsset/FHLevelAsset.h"
#include "Core/GameDataSubsystem.h"
#include "Core/FHGameInstance.h"
#include "Lobby/FHLobbyGameMode.h"
#include "Core/FHPlayerStateBase.h"
#include "UI/Lobby/FHRoundResult.h"
#include "UI/Lobby/FHGameOver.h"
#include "UI/Common/FHFadeInOut.h"
#include "Props/FHKioskBase.h"
#include "Core/FHPropManager.h"
#include "Core/GameSaveData.h"
#include "SimpleSaveKitFunctionLibrary.h"
#include "FHLobbyGameMode.h"


AFH_GS_LobbyGameState::AFH_GS_LobbyGameState()
{
	NET_DEBUG_LOG(TEXT(""));

	bReplicates = true;
	PropSaveSlotName = TEXT("ItemActors");
}

void AFH_GS_LobbyGameState::BeginPlay()
{
	Super::BeginPlay();

	NET_DEBUG_LOG(TEXT(""));

	if (HasAuthority())
	{
		bIsGameOver = false;
		PlayerMoney = 0;
		RequiredMoney = InitRequireMagic;
		LoopCount = 1;
		LoopCount_AtCurrentRank = 1;
		DeathCount = 0;
		CurrentReturnedCoreCount = 0;
		TeamLicenseRank = ELicenseRank::F;
	}

	UGameInstance* GameInstance = GetGameInstance();
	check(GameInstance);

	UGameDataSubsystem* GameData = GameInstance->GetSubsystem<UGameDataSubsystem>();
	if (GameData)
	{
		bIsLAN = GameData->IsLAN();
		bIsPublic = GameData->IsPublic();
		bIsFriendOnly = GameData->IsFriendOnly();

	}



	if (HasAuthority())
	{
		//Rest();
		LoadState();
	}
	OnRep_PlayerMoney();
	OnRep_RequiredMoney();
}

void AFH_GS_LobbyGameState::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	bool isQuit = EndPlayReason == EEndPlayReason::Quit;
	bool isLobby = !!GetWorld()->GetGameState<AFH_GS_LobbyGameState>();
	if (HasAuthority() && !bIsGameOver && !bIsTravle)
	{
		
		SaveState();
	}

}

void AFH_GS_LobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFH_GS_LobbyGameState, LeftTime);
	DOREPLIFETIME(AFH_GS_LobbyGameState, RequiredMoney);
	DOREPLIFETIME(AFH_GS_LobbyGameState, PlayerMoney);
	DOREPLIFETIME(AFH_GS_LobbyGameState, bIsGameOver);
	DOREPLIFETIME(AFH_GS_LobbyGameState, LoopCount);
	DOREPLIFETIME(AFH_GS_LobbyGameState, LoopCount_AtCurrentRank);
	DOREPLIFETIME(AFH_GS_LobbyGameState, TeamLicenseRank);
	


	
}

FString AFH_GS_LobbyGameState::GetSaveSlot() const
{
	return TEXT("GameState");
}

bool AFH_GS_LobbyGameState::IsGlobal() const
{
	return true;
}

void AFH_GS_LobbyGameState::SerializeData(FArchive& Ar)
{
	Ar.UsingCustomVersion(FFRankHunterCustomVersion::GUID);
	AFHLobbyGameMode* LobbyGameMode = Cast<AFHLobbyGameMode>(AuthorityGameMode);
	check(LobbyGameMode);

	UGameInstance* GameInstance = GetGameInstance();
	check(GameInstance);

	UGameDataSubsystem* GameData = GameInstance->GetSubsystem<UGameDataSubsystem>();
	check(GameData);

	if (Ar.CustomVer(FFRankHunterCustomVersion::GUID) >= FFRankHunterCustomVersion::FirstVersion)
	{
		// 언젠가 게이트상태랑 통합 해야하는데  OnRep같은거 꼬여있어서 
		// 지금은 갱신하고싶은값만 갱신후 저장하게 변경
		GameData->GetGameSaveData().PlayerMoney = PlayerMoney;
		GameData->GetGameSaveData().RequiredMoney = RequiredMoney;
		GameData->GetGameSaveData().LoopCount = LoopCount;
		GameData->GetGameSaveData().LoopCount_AtCurrentRank = LoopCount_AtCurrentRank;
		GameData->GetGameSaveData().DeathCount = DeathCount;
		GameData->GetGameSaveData().CurrentReturnedCoreCount = CurrentReturnedCoreCount;
		GameData->GetGameSaveData().TeamLicenseRank = TeamLicenseRank;
		GameData->GetGameSaveData().WhiteboardText = WhiteboardText;
		GameData->GetGameSaveData().Password = LobbyGameMode->Password;
		GameData->GetGameSaveData().bIsLAN = bIsLAN;
		GameData->GetGameSaveData().bIsPublic = bIsPublic;
		GameData->GetGameSaveData().bIsFriendOnly = bIsFriendOnly;
		GameData->GetGameSaveData().SelectedPawnKey = SelectedPawnKey;
		GameData->SetIsMorning(bIsMorning);
		
		GameData->GetGameSaveData().Serialize(Ar);
		

		PlayerMoney = GameData->GetGameSaveData().PlayerMoney;
		RequiredMoney = GameData->GetGameSaveData().RequiredMoney;
		LoopCount = GameData->GetGameSaveData().LoopCount;
		LoopCount_AtCurrentRank = GameData->GetGameSaveData().LoopCount_AtCurrentRank;
		DeathCount = GameData->GetGameSaveData().DeathCount;
		CurrentReturnedCoreCount = GameData->GetGameSaveData().CurrentReturnedCoreCount;
		TeamLicenseRank = GameData->GetGameSaveData().TeamLicenseRank;
		WhiteboardText = GameData->GetGameSaveData().WhiteboardText;
		LobbyGameMode->Password = GameData->GetGameSaveData().Password;
		bIsLAN = GameData->GetGameSaveData().bIsLAN;
		bIsPublic = GameData->GetGameSaveData().bIsPublic;
		bIsFriendOnly = GameData->GetGameSaveData().bIsFriendOnly;
		SelectedPawnKey = GameData->GetGameSaveData().SelectedPawnKey;
		SetLobbyState(GameData->IsMorning());
	
	}
}

void AFH_GS_LobbyGameState::LoadState()
{
	NET_DEBUG_LOG(TEXT(""));

	FString GameID = UFHBlueprintFunctionLibrary::GetGameName(this);
	if (!USimpleSaveKitFunctionLibrary::IsExistSaveFile(GameID, this))
	{
		OnFirstPlayGameStarted();
	}

	UFHBlueprintFunctionLibrary::LoadObject(this);
	UFHBlueprintFunctionLibrary::LoadActors(this, PropSaveSlotName);

	OnLoadEnded.Broadcast();
}

void AFH_GS_LobbyGameState::SaveState()
{
	TArray<TSubclassOf<AActor>> SaveActorClassArray;
	SaveActorClassArray.Add(AFHPickupItemActor::StaticClass());
	SaveActorClassArray.Add(AFHBackpack::StaticClass());


	UFHBlueprintFunctionLibrary::SaveObject(this);
	if (GetLobbyState())
	{
		UFHBlueprintFunctionLibrary::SaveActors(this, PropSaveSlotName, SaveActorClassArray);
	}

	TArray<APlayerState*> PlayerStates = PlayerArray;
	for (APlayerState* PS : PlayerStates)
	{
		if (AFHPlayerStateBase* FHPlayerStateBase = Cast<AFHPlayerStateBase>(PS))
		{
			FHPlayerStateBase->TrySave();
		}
	}
}

void AFH_GS_LobbyGameState::ResetCount()
{
	AcceptPlayerCount = 0;
	RefusePlayerCount = 0;
}

void AFH_GS_LobbyGameState::Rest()
{
	if (!HasAuthority())
	{
		return;
	}
	UFHGameInstance* GameInstance = Cast<UFHGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->SetLobbyState(true);
	}
	SetLobbyState(true);

	FString GameId = UFHBlueprintFunctionLibrary::GetGameID(this);
	UFHBlueprintFunctionLibrary::RemoveSaveGame(GameId, PropSaveSlotName);
	UFHBlueprintFunctionLibrary::RemoveSaveGame(GameId, TEXT("Shop"));

	if (LoopCount % 2 == 0 && LoopCount > 0)
	{
		// TODO1: 할당량 체크 해서 충족하지 못하면 실패 문구, 아니면 다음으로.
		if (RequiredMoney <= PlayerMoney)
		{
			PlayerMoney -= RequiredMoney;
			OnRep_PlayerMoney();
			SetPayment();
		}
		else
		{
			PlayGameOver();
			//GameOver();
			return;
		}
	}
	else
	{
		SetPayment();
	}


	LoopCount++;
	LoopCount_AtCurrentRank++;

	SaveState();

	AFHLobbyGameMode* GameMode = Cast<AFHLobbyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->ClearAllVoice();
	}
	//UFHLevelAsset* LevelAsset = UFHBlueprintFunctionLibrary::GetLevelAsset();
	//if (LevelAsset)
	//{
	//	GetWorld()->ServerTravel(LevelAsset->GetStartLobby(), true);
	//}
}

void AFH_GS_LobbyGameState::SetPayment()
{
	if (!HasAuthority())
	{
		return;
	}

	int32 Round = LoopCount;
	int32 Require = RequiredMoney;
	int32 NewRequire = 0;
	//int32 PlayerDeathCount = DeathCount;
	//int32 Panerty = FMath::RoundToInt32((double)PlayerMoney * ((double)PlayerDeathCount * 0.1));

	//PlayerMoney = FMath::Clamp(PlayerMoney - Panerty, 0, PlayerMoney);

	if (Round % 2 == 0)
	{
		double IncreaseRate = 0.0;
		if (Round < 2)
		{
			IncreaseRate = 1;
		}
		else if (Round < 8)
		{
			IncreaseRate = 1.5;
		}
		else
		{
			IncreaseRate = 1.15;
		}

		NewRequire = FMath::RoundToInt32((double)Require * IncreaseRate);
		//NewRequire += RequireIncreasement;
	}
	else
	{
		NewRequire = Require;
	}

	if (NewRequire >= 9999)
	{
		NewRequire = 9999;
	}

	RequiredMoney = NewRequire;
	OnRep_RequiredMoney();

	//DeathCount = 0;
}

void AFH_GS_LobbyGameState::ApplyPenalty()
{
	int32 PlayerDeathCount = DeathCount;
	int32 Panerty = FMath::RoundToInt32((double)PlayerMoney * ((double)PlayerDeathCount * 0.1));

	PlayerMoney = FMath::Clamp(PlayerMoney - Panerty, 0, PlayerMoney);

	DeathCount = 0;
	OnRep_PlayerMoney();
}

void AFH_GS_LobbyGameState::GameOver()
{
	if (!HasAuthority())
	{
		return;
	}
	bIsGameOver = true;

	FString GameId = UFHBlueprintFunctionLibrary::GetGameID(this);
	UFHBlueprintFunctionLibrary::RemoveGame(GameId);

	AFHLobbyGameMode* GameMode = Cast<AFHLobbyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->ClearAllVoice();
	}


	//UFHLevelAsset* LevelAsset = UFHBlueprintFunctionLibrary::GetLevelAsset();
	//if (LevelAsset)
	//{
	//	GetWorld()->ServerTravel(LevelAsset->GetStartLobby(), true);
	//}

	// BP에서 연출 딜레이 전부 줬음. 2025.08.01 권세민
	//// 딜레이필요해서 테스트 임시용
	//// 연출들어가면 해당 연출 끝나고 트레블 하면 될듯
	//FTimerHandle TimerHandle_Restart;
	//GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::RestartGame);

}

void AFH_GS_LobbyGameState::RestartGame()
{
	if (!HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		/*
			현재 구현돼있는 로직 기반으로는 모든 플레이어가 휴식 수락을 눌렀을 시
			밤 로비에서 할당량 체크 후 낮 로비로 이동.
		*/
		//FString TravelURL = UFHBlueprintFunctionLibrary::GetMapName(this);
		//UE_LOG(LogTemp, Warning, TEXT("Restarting map: %s"), *TravelURL);
		//World->ServerTravel(TravelURL);

		UFHLevelAsset* LevelAsset = UFHBlueprintFunctionLibrary::GetLevelAsset();
		if (LevelAsset)
		{
			GetWorld()->ServerTravel(LevelAsset->GetStartLobby(), true);
		}
	}
}

int AFH_GS_LobbyGameState::GetCurrentYear()
{
	return UFHBlueprintFunctionLibrary::GetCurrentYear(LoopCount);
}

int AFH_GS_LobbyGameState::GetCurrentMounth()
{
	return UFHBlueprintFunctionLibrary::GetCurrentMounth(LoopCount);
}

int AFH_GS_LobbyGameState::GetRemainingExplorationChances()
{
	return LoopCount % 2 + 1;
}



void AFH_GS_LobbyGameState::PlayRestResult_Implementation(int32 loopCount)
{
	LoopCount = loopCount;

	//AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	//if (UIManager)
	//{
	//	if (LoopCount % 2 == 1)
	//	{
	//		UFHFadeInOut* FadeInOut = UIManager->GetWidget<UFHFadeInOut>(TEXT("FadeInOut"));
	//		if (FadeInOut)
	//		{
	//			FadeInOut->OnFadeInEndDelegate.AddDynamic(this, &AFH_GS_LobbyGameState::Rest);
	//			FadeInOut->PlayFadeIn();
	//		}
	//	}
	//	else
	//	{
	//		UFHRoundResult* Widget = UIManager->GetWidget<UFHRoundResult>(TEXT("RoundResult"));
	//		if (Widget)
	//		{
	//			Widget->Show();
	//		}
	//	}
	//}

	if (LoopCount % 2 == 1)
	{
		FOnFadeEnd FadeEndDelegate;
		FadeEndDelegate.BindDynamic(this, &AFH_GS_LobbyGameState::Rest);
		GetGameInstance<UFHGameInstance>()->PlayFadeIn(FadeEndDelegate);
	}
	else
	{
		AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
		if (!UIManager)
		{
			return;
		}
		UFHRoundResult* Widget = UIManager->GetWidget<UFHRoundResult>(TEXT("RoundResult"));
		if (Widget)
		{
			Widget->Show();
		}
	}
}


void AFH_GS_LobbyGameState::PlayGameOver_Implementation()
{
	AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	if (UIManager)
	{
		UFHGameOver* Widget = UIManager->GetWidget<UFHGameOver>(TEXT("GameOver"));
		if (Widget)
		{
			Widget->Show();
		}
	}
}

void AFH_GS_LobbyGameState::AddMoney(int32 Amount)
{
	PlayerMoney += Amount;
	OnRep_PlayerMoney();
}

void AFH_GS_LobbyGameState::SetMoney(int32 NewMoney)
{
	PlayerMoney = NewMoney;
	OnRep_PlayerMoney();
}

int32 AFH_GS_LobbyGameState::GetMoney()
{
	return PlayerMoney;
}

void AFH_GS_LobbyGameState::SetLobbyState(bool State)
{
	UGameInstance* GameInstance = GetGameInstance();
	check(GameInstance);

	UGameDataSubsystem* GameData = GameInstance->GetSubsystem<UGameDataSubsystem>();
	check(GameData);

	GameData->SetIsMorning(State);
	bIsMorning = State;
}

// Client Only
void AFH_GS_LobbyGameState::DisablePreviewActor_Implementation(const FName& PawnMeshKey)
{
	BP_DisablePreviewActor(PawnMeshKey);
}

void AFH_GS_LobbyGameState::OnRep_LeftTime()
{
	OnLeftTimeReplicatedDelegate.Broadcast(LeftTime);
	if (LeftTime == 0)
	{
		OnTimerEndDelegate.Broadcast();
	}
}

void AFH_GS_LobbyGameState::OnRep_RequiredMoney()
{
	OnChangedRequiredMoney.Broadcast(RequiredMoney);
}

void AFH_GS_LobbyGameState::OnRep_TeamLicenseRank()
{
}

#include "UI/FHLobbyNPCWidget.h"
void AFH_GS_LobbyGameState::SetLicenseRankAndReturnedCoreCount_Implementation(ELicenseRank Rank, int32 ReturnedCoreCount, bool IsRankUp)
{
	TeamLicenseRank = Rank;
	OnRep_TeamLicenseRank();
	CurrentReturnedCoreCount = ReturnedCoreCount;

	if (IsRankUp)
	{
		LoopCount_AtCurrentRank = 1;
	}
	
	AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	if (!UIManager)
	{
		return;
	}
	UFHLobbyNPCWidget* Widget = UIManager->GetWidget<UFHLobbyNPCWidget>(TEXT("NpcTalk"));
	if (!Widget)
	{
		return;
	}

	Widget->ShowReturnResult(IsRankUp);

}

void AFH_GS_LobbyGameState::OnRep_PlayerMoney()
{
	OnChangedPlayerMoney.Broadcast(PlayerMoney);
}
