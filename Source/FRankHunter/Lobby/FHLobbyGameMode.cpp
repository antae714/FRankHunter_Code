// Copyright F Rank Hunter.. All Rights Reserved.


#include "Lobby/FHLobbyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "FRankHunter.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineExternalUIInterface.h"

#include "GameFramework/PlayerState.h"
#include "Algo/RandomShuffle.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

#include "Core/FHPlayerController.h"
#include "Core/ManagerActorRegistrySubsystem.h"
#include "Data/FH_DT_SampleMapData.h"
#include "Core/FHGameInstance.h"
#include "Core/FHPlayerStateBase.h"
#include "FH_GS_LobbyGameState.h"

#include "SocketSubsystemModule.h"

#include "Kismet/GameplayStatics.h"
#include "Props/FHDisplayBoard.h"
#include "Props/FHTestCart.h"
#include "Props/FHKioskBase.h"
#include "Props/FHPurchaseStorage.h"
#include "Data/FHQuotaIncreaseTable.h"
#include "UI/Lobby/FHBuyUIBase.h"
#include "UI/Lobby/FHShopBase.h"
#include "UI/Lobby/LobbyUIUtils.h"
#include "UI/FHLobbyNPCWidget.h"
#include "Core/FHUIManager.h"
#include "Core/FHPropManager.h"
#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "Item/FHInventoryComponent.h"
#include "Item/Actors/FHWorldItemActor.h"
#include "Item/Actors/FHBackpack.h"
#include "Props/FHCart.h"
#include "DataAsset/FHLevelAsset.h"
#include "GameFramework/GameSession.h"
#include "Core/GameDataSubsystem.h"
#include "Component/WidgetRPC/FHSelectCharacterRPC.h"
#include "Props/Preview/FHPreviewActorBase.h"
#include "Component/WidgetRPC/FHGateEnterRequestRPCComponent.h"

DEFINE_LOG_CATEGORY(LogLobby);

AFHLobbyGameMode::AFHLobbyGameMode()
{
	NET_DEBUG_LOG(TEXT(""));
}

void AFHLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	bUseSeamlessTravel = true;

	NET_DEBUG_LOG(TEXT(""));

	LobbyGameState = GetGameState<AFH_GS_LobbyGameState>();
	if (LobbyGameState)
	{
		UE_LOG(LogLobby, Warning, TEXT("LobbyGameState Load Success."));
		/*LobbyGameState->UserCount = 0;*/
	}
	else
	{
		UE_LOG(LogLobby, Warning, TEXT("LobbyGameState Load Failed."));
	}

	ResetGateSelect();
	
	UFHGameInstance* GameInstance = Cast<UFHGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		LobbyGameState->SetLobbyState(GameInstance->GetLobbyState());

		UGameDataSubsystem* GameDataSubsystem = GameInstance->GetSubsystem<UGameDataSubsystem>();
		if (GameDataSubsystem)
		{
			Password = GameDataSubsystem->GetPassword();
		}
		else
		{
			UE_LOG(LogLobby, Error, TEXT("GameDataSubsystem not Found."));
		}
	}
	else
	{
		UE_LOG(LogLobby, Error, TEXT("UFHGameInstance not Found."));
	}

	InitSelectInfo();
	//GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AFHLobbyGameMode::DelayInit);
}

void AFHLobbyGameMode::BeginDestroy()
{
	Super::BeginDestroy();
}

void AFHLobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	bool IsDevelopDebug = UGameplayStatics::HasOption(Options, TEXT("DevelopDebug"));

#if WITH_EDITOR
	IsDevelopDebug = true;
#endif

	if (IsDevelopDebug)
	{
		return;
	}

	FString PasswordOption = UGameplayStatics::ParseOption(Options, TEXT("Password"));
	if(Password != PasswordOption)
	{
		ErrorMessage = TEXT("Password Is Not Correct");
		return;
	}

	FString NickName = UGameplayStatics::ParseOption(Options, TEXT("NickName"));
	if (NickName.IsEmpty())
	{
		ErrorMessage = TEXT("NickName Is Not Setting");
		return;
	}

	if (GetPlayerNames().Contains(NickName))
	{
		ErrorMessage = TEXT("NickName Is Already Exits");
	}
}

#include "GAS/FHAbilitySystemComponent.h"
#include "GAS/FHGameplayTags.h"
#include "Player/FHPlayerBase.h"

void AFHLobbyGameMode::PostLogin(APlayerController* PlayerController)
{
	NET_DEBUG_LOG(TEXT(""));

	AFHPlayerStateBase* PS = PlayerController->GetPlayerState<AFHPlayerStateBase>();
	if (PS)
	{
		FString NickName = PS->GetPlayerName();

		if (NickName == TEXT("KickMe*"))
		{
			if (GameSession)
			{
				GameSession->KickPlayer(PlayerController, FText::AsCultureInvariant(TEXT("닉네임이 이미 존재합니다.")));
			}
		}

		// PS의 이름에 맞는 데이터가 존재하는지 체크.
		bool Result = PS->TryLoad();
		if (!Result)
		{
			// Result == false면 해당 플레이어 이름에 맞는 데이터가 존재하지 않는다는 뜻.
			PS->StartFirstEnterProcess();
		}
		else
		{
			AFH_GS_LobbyGameState* GS = Cast<AFH_GS_LobbyGameState>(GameState);
			if (GameState)
			{
				FName PawnKey = PS->GetPawnKey();
				GS->SelectedPawnKey.Add(PawnKey);
				GS->DisablePreviewActor(PawnKey);
			}
		}
	}

	Super::PostLogin(PlayerController);

}


void AFHLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APlayerController* Controller = Cast<APlayerController>(Exiting);
	if (!Controller)
	{
		return;
	}

	if (LobbyGameState)
	{
		UE_LOG(LogLobby, Warning, TEXT("Player Is Out."));
		LobbyGameState->PlayerCount--;

		AFHPlayerStateBase* ExitingPS = Exiting->GetPlayerState<AFHPlayerStateBase>();
		if (ExitingPS)
		{
			FName ExitPawnKey = ExitingPS->GetPawnKey();
			LobbyGameState->SelectedPawnKey.Remove(ExitPawnKey);
			PRINT_LOG(TEXT("Pawn Key %s can select."), *ExitPawnKey.ToString());
		}

		if (!TimerHandle.IsValid())
		{
			return;
		}

		// 로비가 아침일 때 나가게 되면 자동으로 거절 처리.
		if (LobbyGameState->GetLobbyState())
		{
			FString ExitingPlayerName = Exiting->PlayerState->GetPlayerName();
			// 수락을 누른 상태에서 게임을 나갔을 때 처리되는 곳.
			if(AnsweredPlayerNames.Contains(ExitingPlayerName))
			{
				LobbyGameState->AcceptPlayerCount--;
				AnsweredPlayerNames.Remove(ExitingPlayerName);
				AnswerToRequest(false, Controller);
			}
			// 그 외
			else
			{
				if(!RefusedPlayerNames.Contains(ExitingPlayerName))
				{
					AnswerToRequest(false, Controller);
				}
			}
		}
		// 로비가 밤일 때
		else
		{
			// 휴식 체크하는 도중 클라이언트가 나가지는 상황이 생겼을 때 처리하는 곳.
			if (LobbyGameState->AcceptPlayerCount + LobbyGameState->RefusePlayerCount >= LobbyGameState->PlayerCount)
			{
				if (LobbyGameState->AcceptPlayerCount == LobbyGameState->PlayerCount)
				{
					LobbyGameState->PlayRestResult(LobbyGameState->LoopCount);
					//LobbyGameState->Rest();
				}
				else
				{
					for (auto PCit = GetWorld()->GetControllerIterator(); PCit; PCit++)
					{
						AFHPlayerController* FHPC = Cast<AFHPlayerController>(*PCit);
						if (FHPC)
						{
							FHPC->CloseRestRequest();
						}
					}
				}
			}
		}
	}
}

void AFHLobbyGameMode::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}

FString AFHLobbyGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	NET_DEBUG_LOG(TEXT(""));

	if (NewPlayerController && NewPlayerController->PlayerState)
	{
		FString NickName = UGameplayStatics::ParseOption(Options, TEXT("NickName"));
		if (NickName.IsEmpty())
		{
			UFHGameInstance* GameInstance = Cast<UFHGameInstance>(GetGameInstance());
			UGameDataSubsystem* GameDataSubsystem = GameInstance ? GameInstance->GetSubsystem<UGameDataSubsystem>() : nullptr;
			NickName = GameDataSubsystem ? GameDataSubsystem->GetOwnerNickName() : TEXT("NoName");
		}	


#if WITH_EDITOR
		if (true)
		{

		} else
#endif
		if (GetPlayerNames().Contains(NickName))
		{
			NickName = TEXT("KickMe*");
		}

		IOnlineIdentityPtr OnlineIdentity = Online::GetIdentityInterface(TEXT("Steam"));
		if (OnlineIdentity.IsValid())
		{
			FString UserName = OnlineIdentity->GetPlayerNickname(*UniqueId);
			NickName = UserName;
			NET_DEBUG_LOG(TEXT("%s"), *UserName);
		}

#if WITH_EDITOR    
		if (const UWorld* World = GetWorld())
		{
			if (const AGameStateBase* GS = World->GetGameState<AGameStateBase>())
			{
				NickName = FString::Printf(TEXT("PIE_Player_%d"), GS->PlayerArray.IndexOfByKey(NewPlayerController->PlayerState));
			}
		}
#endif

		NewPlayerController->PlayerState->SetPlayerName(NickName);


	}

	return Result;
}

void AFHLobbyGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	PRINT_LOG(TEXT("PostSeamlessTravel Lobby Level."));

	BP_UpdateSession();
	// 게임 오버됐을 때 게임 데이터를 모두 삭제하는데, 밤 로비 -> 낮 로비로 돌아올 때는 PostLogin() 함수를 거치지 않음.
	// 그래서 PostSeamlessTravel에서 플레이어의 데이터를 체크하고, 
	// 데이터가 존재하지 않으면 캐릭터 선택부터 다시 시작할 수 있게 로직을 작성.
	int32 PlayerNums = UGameplayStatics::GetNumPlayerStates(GetWorld());
	for (int32 i = 0; i < PlayerNums; i++)
	{
		AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(UGameplayStatics::GetPlayerState(GetWorld(), i));
		if (!PS)
		{
			continue;
		}

		bool Result = PS->TryLoad();
		if (!Result)
		{
			// Result == false면 해당 플레이어 이름에 맞는 데이터가 존재하지 않는다는 뜻.
			PS->StartFirstEnterProcess();
		}
	}
	bIsMoveFromNight = true;

	//GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AFHLobbyGameMode::DelaySettingPostSeamlessTravel);
}

void AFHLobbyGameMode::DelaySettingPostSeamlessTravel()
{
	if (LobbyGameState->GetLobbyState())
	{
		return;
	}
	LobbyGameState->ApplyPenalty();
}

void AFHLobbyGameMode::DelayInit()
{
	NET_DEBUG_LOG(TEXT(""));

	UManagerActorRegistrySubsystem* ManagerActorSubsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();

	PropManager = ManagerActorSubsystem->GetManagerActor<AFHPropManager>(TEXT("PropManager"));

	if (LobbyGameState && PropManager)
	{
		if (!LobbyGameState->GetLobbyState())
		{
			return;
		}
		InitShopItemList();
	}
}


void AFHLobbyGameMode::PlayerEnterReady()
{
	// 각 클라이언트에서 voice packet 청소가 완료된 이후 호출하게 됨.	PRINT_LOG(TEXT("Player Ready Count: %d"), EnterReadyPlayerCount);
	PRINT_LOG(TEXT("Player Ready Count: %d"), EnterReadyPlayerCount);
	EnterReadyPlayerCount++;
	if (EnterReadyPlayerCount == LobbyGameState->PlayerCount)
	{
		if (!LobbyGameState->GetLobbyState())
		{
			PRINT_LOG(TEXT("Player Ready Count: %d, TotalPlayerCount: %d"), EnterReadyPlayerCount, LobbyGameState->PlayerCount);
			PRINT_LOG(TEXT("Start BackToLobby."));
			ChangeMap();
		}
		else
		{
			UFHLevelAsset* LevelAsset = UFHBlueprintFunctionLibrary::GetLevelAsset();
			if (LevelAsset)
			{
				GetWorld()->ServerTravel(LevelAsset->GetStartLobby(), true);
			}
		}
	}
}

void AFHLobbyGameMode::InitShopItemList()
{
	// 굳이?? 그래서 뺐음
	//if (HasAuthority())
	//{
	//}
	TArray<FFHItemData*> ItemList;
	LobbyGameState->ItemTable->GetAllRows(TEXT("AllItemList"), ItemList);

	AFHKioskBase* Kiosk = PropManager->GetProp<AFHKioskBase>(TEXT("Kiosk"));
	if (!Kiosk)
	{
		return;
	}

	UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	check(Subsystem);

	AFHUIManager* UIManager = Subsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	check(UIManager);

	UFHLobbyNPCWidget* Widget = UIManager->GetWidget<UFHLobbyNPCWidget>(TEXT("NpcTalk"));
	if (!Widget)
	{
		return;
	}

	for (FFHItemData* ItemData : ItemList)
	{
		EItemType ItemType = ItemData->Type;
		FName ItemID = ItemData->ItemID;
		int32 MaxBuyCount = ItemData->MaxBuyCount;

		Kiosk->GetShopList().AddItem(ItemType, ItemID, MaxBuyCount);
	}

	if (!bIsMoveFromNight)
	{
		bool HasShopSaveData = Kiosk->TryLoadShopList();
	}
	bIsMoveFromNight = false;
	Kiosk->GetShopList().MarkArrayDirty();
	Widget->BuyUI->InitShopList();
}

void AFHLobbyGameMode::ResetShopItemList()
{
	TArray<FFHItemData*> ItemList;
	LobbyGameState->ItemTable->GetAllRows(TEXT("AllItemList"), ItemList);

	UManagerActorRegistrySubsystem* ManagerActorSubsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();

	PropManager = ManagerActorSubsystem->GetManagerActor<AFHPropManager>(TEXT("PropManager"));

	AFHKioskBase* Kiosk = PropManager->GetProp<AFHKioskBase>(TEXT("Kiosk"));
	if (!Kiosk)
	{
		return;
	}

	UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	check(Subsystem);

	AFHUIManager* UIManager = Subsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	check(UIManager);

	UFHLobbyNPCWidget* Widget = UIManager->GetWidget<UFHLobbyNPCWidget>(TEXT("NpcTalk"));
	if (!Widget)
	{
		return;
	}

	for (FFHItemData* ItemData : ItemList)
	{
		EItemType ItemType = ItemData->Type;
		FName ItemID = ItemData->ItemID;
		int32 MaxBuyCount = ItemData->MaxBuyCount;

		Kiosk->GetShopList().AddItem(ItemType, ItemID, MaxBuyCount);
	}

	Kiosk->TrySaveShopList();
	Kiosk->GetShopList().MarkArrayDirty();
	Widget->BuyUI->InitShopList();
}

void AFHLobbyGameMode::ResetGateSelect()
{
	// 1. shuffle AreaID and choose 3 areas that are not adjacent to each other
	// 2. shuffle 3 AreaIDs and Assign Rank starting from front.
	// 3. Set Grade for each Ranks.
	// 4. send data use client RPC. Parameter = TArray<uint8>

	UE_LOG(LogLobby, Warning, TEXT("Reset Gate SelectButton"));

	SelectedGateInfo.Empty();

	// Step 1.
	TArray<uint8> AreaID;
	for (int i = 0; i < 25; i++)
	{
		AreaID.Add(i);
	}
	Algo::RandomShuffle(AreaID);

	// Step 2.
	// Get Host PlayerState
	AFHPlayerStateBase* HostPlayerState = Cast<AFHPlayerStateBase>(UGameplayStatics::GetPlayerState(GetWorld(), 0));
	
	// 이제 공용 등급으로 전환. 쓰지않음.
	// ELicenseRank HostLicenseRank = HostPlayerState->GetLicenseRank();
	// 등급이 공용으로 전환됨 + 게이트가 등급 상관없이 고정적으로 6개가 뜨기 떄문에 이 코드도 래거시 코드.
	// If Rank Is MaxRank Or MinRank, Can Select 2 Gates.
	//int32 GateCount = ((HostLicenseRank == ELicenseRank::A || HostLicenseRank == ELicenseRank::F) ? 2 : 3);
	PRINT_LOG(TEXT("Cur GateCount: %d."), GateCount);

	for (uint8& Id : AreaID)
	{
		if (SelectedGateInfo.IsEmpty())
		{
			SelectedGateInfo.Add(Id);
			continue;
		}

		bool bPass = true;
		for (uint8& Selected : SelectedGateInfo)
		{
			FAreaAdjacentData* AdjacentData = AreaAdjacentData->FindRow<FAreaAdjacentData>(FName(*(FString::FromInt(Selected))), TEXT("AreaID"));

			if (AdjacentData->AdjacentNodeValue & (1 << Id))
			{
				bPass = false;
				break;
			}
		}

		if (!bPass)
		{
			continue;
		}

		SelectedGateInfo.Add(Id);

		if (SelectedGateInfo.Num() == GateCount)
		{
			break;
		}
	}

	// Step 3.
	// 게이트의 출현 개수가 고정됨에 따라 게이트의 모든 등급이 등장. 
	// 하지만 게이트 입장 가능한 등급은 이거 필요함. 이건 셋팅할 때 체크하면 될듯.
	//int32 LicenseRankInt = (uint8)HostLicenseRank;
	//int32 MaxGateRank = FMath::Min(LicenseRankInt + 1, (int32)EGateRank::S);
	//int32 MinGateRank = FMath::Max(LicenseRankInt - 1, (int32)EGateRank::E);

	int32 MaxGateRank = (int32)EGateRank::S;
	int32 MinGateRank = (int32)EGateRank::E;

	TArray<int32> SelectedGateRank;
	for (int32 Rank = MinGateRank; Rank <= MaxGateRank; Rank++)
	{
		SelectedGateRank.Add(Rank);
	}

	Algo::RandomShuffle(SelectedGateRank);
	Algo::RandomShuffle(SelectedGateInfo);
	for (int i = 0; i < SelectedGateInfo.Num(); i++)
	{
		// uint8 -> ( 000 | 00000 ) -> ( GradeNum | AreaNum )  (AreaNum = 0 ~ 24, GradeNum = 1 ~ 6)
		SelectedGateInfo[i] |= ((SelectedGateRank[i]) << 5);
	}

	// step 4. Move to PlayerReady.
	//for (auto PCit = GetWorld()->GetControllerIterator(); PCit; PCit++)
	//{
	//	AFHPlayerController* FHPC = Cast<AFHPlayerController>(*PCit);
	//	if (FHPC)
	//	{
	//		FHPC->ResetGateButton(SelectedGateInfo);
	//	}
	//}
}

bool AFHLobbyGameMode::CheckQuota()
{
	return LobbyGameState->GetRequiredMoney() <= LobbyGameState->GetMoney();
}


void AFHLobbyGameMode::NotifyRest()
{
	LobbyGameState->ResetCount();

	if (TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}

	LobbyGameState->LeftTime = 15;
	LobbyGameState->OnRep_LeftTime();

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle, 
		this,
		&AFHLobbyGameMode::TimerCountDown, 
		1.0f, 
		true
	);

	for (auto PCit = GetWorld()->GetControllerIterator(); PCit; PCit++)
	{
		AFHPlayerController* FHPC = Cast<AFHPlayerController>(*PCit);
		if (FHPC)
		{
			FHPC->OpenRestRequest();
		}
	}
}

void AFHLobbyGameMode::ChangeMap()
{
	// 게이트 입장할 때 참가 방지.
	UFHGameInstance* GameInstance = Cast<UFHGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->SetPlayerCount(LobbyGameState->PlayerCount);
		GameInstance->SetLobbyState(false);
		GameInstance->DeactiveJoinSession();

		if (LobbyGameState)
		{
			LobbyGameState->bIsTravle = true;
			LobbyGameState->SaveState();
			GameInstance->SetCurrentMoneyForGate(LobbyGameState->GetMoney());
			GameInstance->SetRequiredMoneyForGate(LobbyGameState->GetRequiredMoney());

			//TArray<APlayerState*> PlayerStates = LobbyGameState->PlayerArray;
			//for (APlayerState* PS : PlayerStates)
			//{
			//	if (AFHPlayerStateBase* FHPlayerStateBase = Cast<AFHPlayerStateBase>(PS))
			//	{
			//		FHPlayerStateBase->TrySave();
			//	}
			//}
		}

	}

	UFHLevelAsset* LevelAssetDefault = UFHBlueprintFunctionLibrary::GetLevelAsset();
	check(LevelAssetDefault);

	UE_LOG(LogLobby, Warning, TEXT("Server Map Change. Change Target: %s"), *LevelAssetDefault->GetGate());
	GetWorld()->ServerTravel(LevelAssetDefault->GetGate());
}

void AFHLobbyGameMode::OpenEnterRequestWidget(const FString& GateName, const EGateRank& EnterGateRank)
{
	AnsweredPlayerNames.Empty();
	RefusedPlayerNames.Empty();

	UFHGameInstance* GameInstance = Cast<UFHGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->SetGateRank(EnterGateRank);
	}

	LobbyGameState->LeftTime = 15;
	LobbyGameState->OnRep_LeftTime();
	LobbyGameState->ResetCount();

	for (auto PCit = GetWorld()->GetControllerIterator(); PCit; PCit++)
	{
		AFHPlayerController* FHPC = Cast<AFHPlayerController>(*PCit);
		if (FHPC)
		{
			FHPC->OpenEnterRequestUI(GateName);
		}
	}
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFHLobbyGameMode::TimerCountDown, 1.0f, true);
}

void AFHLobbyGameMode::AnswerToRequest_Implementation(bool Answer, APlayerController* PlayerController)
{
	UE_LOG(LogLobby, Warning, TEXT("AnswerToRequest."));

	if (Answer)
	{
		AnsweredPlayerNames.Add(PlayerController->PlayerState->GetPlayerName());
		LobbyGameState->AcceptPlayerCount++;
		for (auto PCit = GetWorld()->GetControllerIterator(); PCit; PCit++)
		{
			UFHGateEnterRequestRPCComponent* RPCComponent = Cast<UFHGateEnterRequestRPCComponent>((*PCit)->GetComponentByClass(UFHGateEnterRequestRPCComponent::StaticClass()));
			if (!RPCComponent)
			{
				continue;
			}
			RPCComponent->OnPlayerAnserRequest(PlayerController->PlayerState->GetPlayerName());
		}
	}
	else
	{
		RefusedPlayerNames.Add(PlayerController->PlayerState->GetPlayerName());
		LobbyGameState->RefusePlayerCount++;
		// TODO: 여기서 거절을 누른 유저의 정보를 찾아 모든 컨트롤러에 대해 client RPC로 정보를 넘겨줘야 합니다.
		//int32 Id = PlayerController->PlayerState->GetPlayerId();
		//UE_LOG(LogLobby, Warning, TEXT("Player ID: %d"), Id);
		FString PlayerName = PlayerController->PlayerState->GetPlayerName();
		PRINT_LOG(TEXT("Refused Player Name: %s"), *PlayerName);
		
		//for (auto PCit = GetWorld()->GetControllerIterator(); PCit; PCit++)
		//{
		//	AFHPlayerController* FHPC = Cast<AFHPlayerController>(*PCit);
		//	if (FHPC)
		//	{
		//		FHPC->OnPlayerRefuse(PlayerName);
		//	}
		//}
	}
	
	int32 PlayerSum = LobbyGameState->AcceptPlayerCount + LobbyGameState->RefusePlayerCount;
	if (PlayerSum >= LobbyGameState->PlayerCount)
	{
		if (LobbyGameState->RefusePlayerCount >= 1)
		{
			// Show CloseEnterRequestWidget.
			for (auto PCit = GetWorld()->GetControllerIterator(); PCit; PCit++)
			{
				AFHPlayerController* FHPC = Cast<AFHPlayerController>(*PCit);
				if (FHPC)
				{
					for(FString& RefusedName : RefusedPlayerNames)
					{
						FHPC->OnPlayerRefuse(RefusedName);
					}
					FHPC->OpenEnterErrorUI();
				}
			}

			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

			LobbyGameState->ResetCount();

		}
		else // 게이트 시작 부분.
		{
			// By ATH 25.07.15
			//LobbyGameState->LoopCount++;
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			// Change to Night
			LobbyGameState->SetLobbyState(false);
			//ChangeMap();
			EnterReadyPlayerCount = 0;
			//ClearAllVoice();
			FOnFadeEnd FadeEndDelegate;
			FadeEndDelegate.BindDynamic(this, &AFHLobbyGameMode::ClearAllVoice);
			GetGameInstance<UFHGameInstance>()->PlayFadeIn(FadeEndDelegate);

			// level 전환 시 클라이언트들의 fade in 호출.
			int32 NumPlayers = UGameplayStatics::GetNumPlayerControllers(GetWorld());
			for(int32 index = 1; index < NumPlayers; index++)
			{
				APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), index);
				if (PC)
				{
					AFHPlayerController* FHPC = Cast<AFHPlayerController>(PC);
					if (FHPC)
					{
						FHPC->FadeIn();
					}
				}
			}
			
		}
	}
}


void AFHLobbyGameMode::SubmitPay_Implementation(int32 Value)
{
	// 안쓰는 함수.
	if (LobbyGameState)
	{
		//LobbyGameState->SubmitMagic += Value;
		//LobbyGameState->OnRep_SubmitMagic();
		//LobbyGameState->AddMoney(-Value);
	}
}

void AFHLobbyGameMode::PlayerReady_Implementation(APlayerController* Player)
{
	NET_DEBUG_LOG(TEXT(""));

	if (LobbyGameState)
	{
		UE_LOG(LogLobby, Warning, TEXT("PlayerController Is Ready."));
		LobbyGameState->PlayerCount++;

		AFHPlayerController* FHPC = Cast<AFHPlayerController>(Player);
		if (FHPC)
		{
			UFHSelectCharacterRPC* RPCComponent = Cast<UFHSelectCharacterRPC>(FHPC->GetComponentByClass(UFHSelectCharacterRPC::StaticClass()));
			if (!RPCComponent)
			{
				return;
			}
			FHPC->ResetGateButton(SelectedGateInfo);
			if (FHPC->GetPlayerState<AFHPlayerStateBase>()->IsFirstPlay())
			{
				RPCComponent->InitPawnState(LobbyGameState->SelectedPawnKey);
				FHPC->CheckFirstPlay();
				FHPC->SetAutoRefuse(true);
			}
			else
			{
				//if (AFHPlayerStateBase* PS = FHPC->GetPlayerState<AFHPlayerStateBase>())
				//{
				//	PS->StartSetting();
				//}
				
				FHPC->ShowQuota(LobbyGameState->LoopCount);
				FHPC->SetAutoRefuse(false);
			}
			FHPC->StartVoiceChat();
		}
	}
}


void AFHLobbyGameMode::PurchaseItem_Implementation(APlayerController* Player, const TArray<struct FPurchaseData>& ItemData, int32 TotalPrice)
{
	UManagerActorRegistrySubsystem* ManagerActorSubsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	if (!ManagerActorSubsystem)
	{
		return;
	}

	PropManager = ManagerActorSubsystem->GetManagerActor<AFHPropManager>(TEXT("PropManager"));

	AFHPlayerController* FHPC = Cast<AFHPlayerController>(Player);
	if (!ensure(FHPC))
	{
		return;
	}
	UFHItemClassDataAsset* DataAsset = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
	if (!DataAsset)
	{
		return;
	}
	UFHFRankHunterSettings* FHSettings = UFHFRankHunterSettings::StaticClass()->GetDefaultObject<UFHFRankHunterSettings>();
	if (!FHSettings)
	{
		return;
	}
	TSoftObjectPtr<UDataTable> ItemDataTable = FHSettings->ItemDataTable;
	if (!ItemDataTable.IsValid())
	{
		return;
	}
	AFHPurchaseStorage* PurchaseStorage = UFHBlueprintFunctionLibrary::GetPropManager(GetWorld())->GetProp<AFHPurchaseStorage>(TEXT("PurchaseStorage"));
	if (!PurchaseStorage)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("PlayerMoney: %d"), LobbyGameState->GetMoney());

	AFHKioskBase* Kiosk = PropManager->GetProp<AFHKioskBase>(TEXT("Kiosk"));
	// if Kiosk is nullptr check kiosk is registered in prop manager or check propkey.
	check(Kiosk);

	if (PurchaseStorage->bIsOpen)
	{
		FHPC->PurchaseError(EPurchaseError::AlreadyOpen);
		return;
	}

	int32 PlayerMoney = LobbyGameState->GetMoney();
	if (TotalPrice > PlayerMoney)
	{
		FHPC->PurchaseError(EPurchaseError::NoMoney);
		return;
	}
	//BP_BuyItemSpawnPoint_C_1
	TArray<FVector> SpawnLocations;
	TArray<FRotator> SpawnRotations;
	for(int32 i = 0; i < 6; i++)
	{
		FString SpawnPointActorKey = FString::Printf(TEXT("BP_BuyItemSpawnPoint_C_%d"), i);
		AActor* SpawnPointActor = PropManager->GetProp<AActor>(*SpawnPointActorKey);
		// if SpawnPointActor is nullptr then Add (BP_BuyItemSpawmPoint_C_0 ~ 5) on Lobby level. 
		// (X=690.000000, Y=870.000000, Z=150.000000)
		// (X=610.000000, Y=870.000000, Z=150.000000)
		// (X=770.000000, Y=870.000000, Z=150.000000)
		// (X=690.000000, Y=910.000000, Z=150.000000)
		// (X=610.000000, Y=910.000000, Z=150.000000)
		// (X=770.000000, Y=910.000000, Z=150.000000)
		check(SpawnPointActor);

		FVector SpawnLocation = SpawnPointActor->GetActorLocation();
		FRotator SpawnRotation = FRotator::ZeroRotator;

		SpawnLocations.Add(SpawnLocation);
		SpawnRotations.Add(SpawnRotation);
	}


	FActorSpawnParameters SpawnParams{};
	SpawnParams.Instigator = nullptr;
	SpawnParams.Owner = nullptr; // NPC??
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 아이템 재고 확인.
	for (const FPurchaseData& PurchaseData : ItemData)
	{
		int32 PurchaseCount = PurchaseData.ItemCount;
		if (!Kiosk->GetShopList().CheckPurchasePossible(PurchaseData.ItemID, PurchaseCount))
		{
			FHPC->PurchaseError(EPurchaseError::LowOnStock);
			return;
		}
	}

	int32 SpawnIndex = 0;
	AFHWorldItemActor* SpawnedActor = nullptr;
	for (const FPurchaseData& PurchaseData : ItemData)
	{
		FFHItemData* FHItemData = ItemDataTable->FindRow<FFHItemData>(PurchaseData.ItemID, TEXT("LobbyGameMode|PurchaseItem"));
		if (!FHItemData)
		{
			// Invalid ItemID.
			continue;
		}



		// 특수 아이템인 가방만 임시로 따로 처리.
		if (FHItemData->Type == EItemType::BackPack)
		{
			if (BackPackActorClass == nullptr)
			{
				FHPC->PurchaseError(EPurchaseError::UnKnownError);
				return;
			}
			AFHBackpack* BackPackActor = GetWorld()->SpawnActor<AFHBackpack>(
				BackPackActorClass,
				SpawnLocations[SpawnIndex],
				SpawnRotations[SpawnIndex],
				SpawnParams
			);

			if (!BackPackActor)
			{
				FHPC->PurchaseError(EPurchaseError::UnKnownError);
				return;
			}
			UFHInventoryComponent* ItemInventory = UFHBlueprintFunctionLibrary::GetInventoryComponent(BackPackActor);
			if (!ItemInventory)
			{
				FHPC->PurchaseError(EPurchaseError::UnKnownError);
				return;
			}
			
			if (PurchaseData.ItemID == TEXT("Backpack_Basic"))
			{
				BackPackActor->BackpackName = TEXT("Backpack_Basic");
				BackPackActor->OnRep_BackpackName();
				ItemInventory->Server_SetMaxItemCount(4);
			}
			else if (PurchaseData.ItemID == TEXT("Backpack_Pro"))
			{
				BackPackActor->BackpackName = TEXT("Backpack_Pro");
				BackPackActor->OnRep_BackpackName();
				ItemInventory->Server_SetMaxItemCount(8);
			}
			else
			{
				FHPC->PurchaseError(EPurchaseError::UnKnownError);
				return;
			}
			UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(BackPackActor->GetRootComponent());
			if (PrimComp)
			{
				PrimComp->SetSimulatePhysics(true);
				PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}

			Kiosk->GetShopList().SubMaxBuyCount(PurchaseData.ItemID, 1);
			Kiosk->NotifyShopBuyCountChanged(PurchaseData);

			SpawnIndex++;
			SpawnIndex %= SpawnLocations.Num();
			LobbyGameState->AddMoney(-FHItemData->BuyPrice);

			continue;
		} // end if

		int32 ItemSpawnCount = PurchaseData.ItemCount;
		for (int32 Count = 0; Count < ItemSpawnCount; Count++)
		{
			SpawnedActor = GetWorld()->SpawnActor<AFHWorldItemActor>(
				DropItemActor,
				SpawnLocations[SpawnIndex],
				SpawnRotations[SpawnIndex],
				SpawnParams
			);
			if (!SpawnedActor)
			{
				// unknown error.
				// error가 발생하기 전 까지의 아이템 개수를 동기화.
				Kiosk->GetShopList().SubMaxBuyCount(PurchaseData.ItemID, Count + 1);
				Kiosk->NotifyShopBuyCountChanged(PurchaseData);
				FHPC->PurchaseError(EPurchaseError::UnKnownError);
				return;
			}
			SpawnedActor->FindComponentByClass<UStaticMeshComponent>()->SetSimulatePhysics(false);

			UFHInventoryComponent* ItemInventory = UFHBlueprintFunctionLibrary::GetInventoryComponent(SpawnedActor);
			if (!ItemInventory)
			{
				// unknown error.
				// error가 발생하기 전 까지의 아이템 개수를 동기화.
				Kiosk->GetShopList().SubMaxBuyCount(PurchaseData.ItemID, Count + 1);
				Kiosk->NotifyShopBuyCountChanged(PurchaseData);
				FHPC->PurchaseError(EPurchaseError::UnKnownError);
				return;
			} // end if
			ItemInventory->Server_SetMaxItemCount(1);
			ItemInventory->Server_AddItem(DataAsset->GetItemClass(PurchaseData.ItemID), 1);
			LobbyGameState->AddMoney(-FHItemData->BuyPrice);
		} // end for
		// error가 발생하지 않았다면 여기까지 옴.
		Kiosk->GetShopList().SubMaxBuyCount(PurchaseData.ItemID, ItemSpawnCount);
		Kiosk->NotifyShopBuyCountChanged(PurchaseData);

		SpawnIndex++;
		SpawnIndex %= SpawnLocations.Num();
	} // end for

	FHPC->PurchaseError(EPurchaseError::None);
	Kiosk->TrySaveShopList();

	PurchaseStorage->PurchaseEnd();

	//AFHCart* Cart = PropManager->GetProp<AFHCart>(TEXT("Cart"));
	//check(Cart);

	//AFHKioskBase* Kiosk = PropManager->GetProp<AFHKioskBase>(TEXT("Kiosk"));
	//check(Kiosk);

	//TArray<FSiItemDataElement>& ItemArray = Cart->GetInventoryComponent()->GetItemArray().GetItemArray();
	//TArray<FSiItemDataElement> SimulatedItemArray = ItemArray;
	//UFHItemClassDataAsset* DataAsset = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
	//if (DataAsset)
	//{
	//	bool bPurchaseResult = true;
	//	int32 RequiredNewSpace = 0;
	//	for (FPurchaseData Data : ItemData)
	//	{
	//		if (!Kiosk->ShopItemList.CheckPurchasePossible(Data.ItemID, Data.ItemCount))
	//		{
	//			FHPC->PurchaseError(EPurchaseError::LowOnStock);
	//			return;
	//		}

	//		TSubclassOf<UFHItemBase> ItemClass = DataAsset->GetItemClass(Data.ItemID);
	//		int32 MaxItemStack = ItemClass.GetDefaultObject()->ItemMaxStack;

	//		int32 Index = SimulatedItemArray.IndexOfByPredicate(
	//			[ItemClass, MaxItemStack](const FSiItemDataElement& ItemData)
	//			{
	//				if (ItemData.ItemInstance == nullptr)
	//				{
	//					return false;
	//				}
	//				bool bIsSameClass = ItemClass == ItemData.ItemInstance.GetClass();
	//				bool bIsFull = ItemData.ItemStack == MaxItemStack;
	//				return bIsSameClass && !bIsFull;
	//			}
	//		);
	//		if (Index == INDEX_NONE)
	//		{
	//			RequiredNewSpace += (Data.ItemCount % MaxItemStack == 0 ? (Data.ItemCount / MaxItemStack) : ((Data.ItemCount / MaxItemStack) + 1));
	//		}
	//		else
	//		{
	//			int32 RemainedCount = MaxItemStack - SimulatedItemArray[Index].ItemStack;
	//			if (RemainedCount < Data.ItemCount)
	//			{
	//				int32 ExceededCount = Data.ItemCount - RemainedCount;
	//				RequiredNewSpace += (ExceededCount % MaxItemStack == 0 ? (ExceededCount / MaxItemStack) : ((ExceededCount / MaxItemStack) + 1));
	//			}
	//		}
	//	}
	//	UE_LOG(LogTemp, Warning, TEXT("Required New Space: %d"), RequiredNewSpace);

	//	int32 EmptySpaceCount = 0;
	//	for (const FSiItemDataElement& ItemElement : SimulatedItemArray)
	//	{
	//		if (ItemElement.ItemInstance == nullptr && !ItemElement.bIsLcok)
	//		{
	//			EmptySpaceCount++;
	//		}
	//	}
	//	if (EmptySpaceCount < RequiredNewSpace)
	//	{
	//		bPurchaseResult = false;
	//	}

	//	if (bPurchaseResult == true)
	//	{
	//		UFHShopBase* Widget = nullptr;

	//		UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	//		check(Subsystem);

	//		AFHUIManager* UIManager = Subsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
	//		check(UIManager);

	//		Widget = UIManager->GetWidget<UFHShopBase>(TEXT("Shop"));

	//		for (FPurchaseData Data : ItemData)
	//		{
	//			TSubclassOf<UFHItemBase> ItemClass = DataAsset->GetItemClass(Data.ItemID);
	//			Cart->GetInventoryComponent()->Server_AddItem(ItemClass, Data.ItemCount);
	//			Kiosk->ShopItemList.SubMaxBuyCount(Data.ItemID, Data.ItemCount);

	//			// if Controller is Host
	//			if (Widget)
	//			{
	//				Widget->OnShopBuyCountChanged(Data.ItemID, Data.ItemCount);
	//			}
	//		}
	//		LobbyGameState->PlayerMoney -= TotalPrice;
	//		LobbyGameState->OnRep_PlayerMoney();



	//		FHPC->PurchaseError(EPurchaseError::None);
	//	}
	//	else
	//	{
	//		FHPC->PurchaseError(EPurchaseError::NoSpace);
	//	}
	//}
}

// 기존에 아이템을 드래그해서 판매리스트에 올리는 것을 구현했지만, 사용하지 않게 되면서 레거시 코드로 전환.
void AFHLobbyGameMode::AddToSellItemCart_Implementation(APlayerController* Player, const FSellItemInfo& ItemInfo)
{
//	AFHPlayerController* PC = Cast<AFHPlayerController>(Player);
//	if (!PC)
//	{
//		return;
//	}
//
//	AFHKioskBase* Kiosk = PropManager->GetProp<AFHKioskBase>(TEXT("Kiosk"));
//	AFHCart* Cart = PropManager->GetProp<AFHCart>(TEXT("Cart"));
//	if (Kiosk && Cart)
//	{
//		TObjectPtr<UFHInventoryComponent> CartInventory = Cast<UFHInventoryComponent>(Cart->GetInventoryComponent());
//
//		UFHItemBase* ItemBase = CartInventory->GetItemInstance<UFHItemBase>(ItemInfo.InventoryIndex);
//		if (ItemBase)
//		{
//			if (ItemBase->GetItemData().ItemID != ItemInfo.ItemID)
//			{
//				PC->AddToSellingCartError(ESellItemError::NoItem);
//				return;
//			}
//
//			int32 InventoryStack = CartInventory->GetItemStack(ItemInfo.InventoryIndex);
//			if (InventoryStack < ItemInfo.ItemStack)
//			{
//				PC->AddToSellingCartError(ESellItemError::NotEnoughCount);
//				return;
//			}
//
//			UFHItemClassDataAsset* DataAsset = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
//			if (DataAsset)
//			{
//				int32 AddedIndex;
//				bool Result = Kiosk->SellingItemList.AddItem(ItemInfo.ItemID, ItemInfo.ItemStack, AddedIndex);
//				CartInventory->Server_RemoveItemAtIndex(ItemInfo.InventoryIndex, ItemInfo.ItemStack);
//				
//				if (Result)
//				{
//					Kiosk->NotifySellItemAdded(AddedIndex);
//				}
//				else
//				{
//					Kiosk->NotifySellItemChanged({ AddedIndex });
//				}
//			}
//		}
//	}
}

void AFHLobbyGameMode::ItemMoveToCart_Implementation(APlayerController* Player, const struct FSellItemInfo& ItemInfo)
{
//	AFHPlayerController* PC = Cast<AFHPlayerController>(Player);
//	if (!PC)
//	{
//		return;
//	}
//	AFHKioskBase* Kiosk = PropManager->GetProp<AFHKioskBase>(TEXT("Kiosk"));
//	AFHCart* Cart = PropManager->GetProp<AFHCart>(TEXT("Cart"));
//	if (Kiosk && Cart)
//	{
//		TObjectPtr<UFHInventoryComponent> CartInventory = Cast<UFHInventoryComponent>(Cart->GetInventoryComponent());
//		FName ItemID = ItemInfo.ItemID;
//		int32 ItemStack = ItemInfo.ItemStack;
//		int32 TargetInventoryIndex = ItemInfo.InventoryIndex;
//
//		if (bool bNoItem = false; !Kiosk->SellingItemList.CheckItemRemoveable(ItemID, ItemStack, bNoItem))
//		{
//			if (bNoItem)
//			{
//				PC->ItemMoveToCartError(ESellItemError::NoItem);
//				return;
//			}
//			else
//			{
//				PC->ItemMoveToCartError(ESellItemError::NotEnoughCount);
//				return;
//			}
//		}
//		UFHItemClassDataAsset* DataAsset = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
//		if (DataAsset)
//		{
//			int32 RemovedIndex;
//			bool Result = Kiosk->SellingItemList.RemoveItem(ItemID, ItemStack, RemovedIndex);
//			CartInventory->Server_AddItem(DataAsset->GetItemClass(ItemID), ItemStack, TargetInventoryIndex);
//			if (Result)
//			{
//				Kiosk->NotifySellItemRemoved({ RemovedIndex });
//			}
//			else
//			{
//				Kiosk->NotifySellItemChanged({ RemovedIndex });
//			}
//		}
//	}
}

void AFHLobbyGameMode::SellItem_Implementation(const TArray<FSellItemInfo>& SellItems, int32 TotalPrice)
{
	

}


void AFHLobbyGameMode::AnswerRest_Implementation(bool Answer, APlayerController* PlayerController)
{
	if (Answer)
	{
		LobbyGameState->AcceptPlayerCount++;
	}
	else
	{
		LobbyGameState->RefusePlayerCount++;
	}

	if (LobbyGameState->AcceptPlayerCount + LobbyGameState->RefusePlayerCount >= LobbyGameState->PlayerCount)
	{
		if (LobbyGameState->RefusePlayerCount >= 1)
		{
			for (auto PCit = GetWorld()->GetControllerIterator(); PCit; PCit++)
			{
				AFHPlayerController* FHPC = Cast<AFHPlayerController>(*PCit);
				if (FHPC)
				{
					FHPC->CloseRestRequest();
				}
			}
		}
		else
		{
			LobbyGameState->bIsTravle = true;
			LobbyGameState->SaveState();
			LobbyGameState->PlayRestResult(LobbyGameState->LoopCount);
			//LobbyGameState->Rest();
		}
	}
}

void AFHLobbyGameMode::RequestChangePassword_Implementation(const FString& NewPassword)
{
	/*UE_LOG(LogLobby, Log, TEXT("Host requests to change password %s to %s"), *Password, *NewPassword);
	
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FNamedOnlineSession* CurrentSession = SessionInterface->GetNamedSession(NAME_GameSession);
			if (CurrentSession)
			{
				FOnlineSessionSettings NewSessionSettings = CurrentSession->SessionSettings;
				if (!NewPassword.IsEmpty())
				{
					NewSessionSettings.Set(PASSWORD_KEY, NewPassword, EOnlineDataAdvertisementType::ViaOnlineService);
				}
				else
				{
					NewSessionSettings.Remove(PASSWORD_KEY);
				}

				FOnUpdateSessionCompleteDelegate OnUpdateCompleteDelegate;
				OnUpdateCompleteDelegate.BindUObject(this, &AFHLobbyGameMode::OnUpdateSessionComplete);
				SessionInterface->AddOnUpdateSessionCompleteDelegate_Handle(OnUpdateCompleteDelegate);

				SessionInterface->UpdateSession(NAME_GameSession, NewSessionSettings, true);

				Password = NewPassword;
			}
			else
			{
				UE_LOG(LogLobby, Warning, TEXT("UpdateSession failed: Could not find named session 'GameSession'."));
			}
		}
	}*/
	// TODO : 
}

void AFHLobbyGameMode::TimerCountDown()
{
	int32 LeftTime = LobbyGameState->LeftTime;
	if (LeftTime <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
	else
	{
		LobbyGameState->LeftTime--;
		LobbyGameState->OnRep_LeftTime();
	}
}

void AFHLobbyGameMode::ReturnCore(APlayerController* Player)
{
	AFHPlayerController* Controller = Cast<AFHPlayerController>(Player);
	if (!Controller)
	{
		return;
	}
	AFHPlayerStateBase* PlayerState = Controller->GetPlayerState<AFHPlayerStateBase>();
	if (!PlayerState)
	{
		return;
	}
	USiInventoryComponent* PlayerInventory = PlayerState->GetInventoryComponent();
	if (!PlayerInventory)
	{
		return;
	}

	int32 ReturnedCoreCount = 0;

	int32 InventoryIndex = 0;
	TArray<FSiItemDataElement>& ItemArray = PlayerInventory->GetItemArray().GetItemArray();
	for (FSiItemDataElement& Item : ItemArray)
	{
		UFHItemBase* ItemBase = Cast<UFHItemBase>(Item.ItemInstance);
		if (!ItemBase)
		{
			InventoryIndex++;
			continue;
		}

		if (ItemBase->ItemID == FName(TEXT("Core_Fragment")))
		{
			ReturnedCoreCount += Item.ItemStack;
			PlayerInventory->Server_RemoveItemAtIndex(InventoryIndex, Item.ItemStack);
		}
		InventoryIndex++;
	}

	LobbyGameState->CurrentReturnedCoreCount += ReturnedCoreCount;
	CheckLicenseUpgrade();
	// TODO: CheckRankupCount();
}

#include "DataAsset/FHSelectInfoIconDataAsset.h"
#include "Kismet/KismetArrayLibrary.h"

void AFHLobbyGameMode::InitSelectInfo()
{
	//UFHSelectInfoIconDataAsset* DataAsset =  UFHBlueprintFunctionLibrary::GetSelectInfoDataAsset();
	//if (!DataAsset)
	//{
	//	return;
	//}
	//int32 SlotCount = DataAsset->GetIconMap().Num();
	//for (int i = 0; i < SlotCount; i++) {
	//	SelectSlotStates.Add(false);
	//}
}


void AFHLobbyGameMode::SlotSelected(int32 Index)
{
	//if (!SelectSlotStates.IsValidIndex(Index))
	//{
	//	return;
	//}

	//SelectSlotStates[Index] = true;

	//int32 NumPlayers = UGameplayStatics::GetNumPlayerControllers(GetWorld());
	//for (int i = 0; i < NumPlayers; i++) {
	//	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), i);
	//	if (!PC) 
	//	{
	//		continue;
	//	}
	//	UFHSelectCharacterRPC* RPCComponent = Cast<UFHSelectCharacterRPC>(PC->GetComponentByClass(UFHSelectCharacterRPC::StaticClass()));
	//	if (!RPCComponent) 
	//	{
	//		return;
	//	}
	//	RPCComponent->IconStateReplicated(Index, SelectSlotStates[Index]);
	//}
}


void AFHLobbyGameMode::SlotReleaseSelect(int32 Index)
{
	//if (!SelectSlotStates.IsValidIndex(Index))
	//{
	//	return;
	//}

	//SelectSlotStates[Index] = false;

	//int32 NumPlayers = UGameplayStatics::GetNumPlayerControllers(GetWorld());
	//for (int i = 0; i < NumPlayers; i++) {
	//	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), i);
	//	if (!PC)
	//	{
	//		continue;
	//	}
	//	UFHSelectCharacterRPC* RPCComponent = Cast<UFHSelectCharacterRPC>(PC->GetComponentByClass(UFHSelectCharacterRPC::StaticClass()));
	//	if (!RPCComponent)
	//	{
	//		return;
	//	}
	//	RPCComponent->IconStateReplicated(Index, SelectSlotStates[Index]);
	//}
}

bool AFHLobbyGameMode::IsValidKey(FName ActorKey)
{
	AFH_GS_LobbyGameState* GS = GetGameState<AFH_GS_LobbyGameState>();
	int32 Index = GS->SelectedPawnKey.IndexOfByPredicate([ActorKey](const FName& Key)
														 {
															 return Key == ActorKey;
														 }
	);
	return Index == INDEX_NONE;
}

void AFHLobbyGameMode::PlayerSelectCompleted(APlayerController* Player, AActor* MeshPreviewActor /*deprecated*/, FName ActorKey, FName SkillID)
{
	AFHPlayerController* FHPlayerController = Cast<AFHPlayerController>(Player);
	AFHPlayerStateBase* PlayerState = Player ? Player->GetPlayerState<AFHPlayerStateBase>() : nullptr;
	if (!PlayerState || !FHPlayerController)
	{
		return;
	}

	AFH_GS_LobbyGameState* GS = Cast<AFH_GS_LobbyGameState>(GameState);
	if (!GS)
	{
		return;
	}
	//PreviewActor->SetDisableActor();
	//if (SelectSlotStates[Index] == true)
	//{
	UFHSelectCharacterRPC* RPCComponent = Cast<UFHSelectCharacterRPC>(Player->GetComponentByClass(UFHSelectCharacterRPC::StaticClass()));
	if (!RPCComponent)
	{
		return;
	}
	//Other Already Selected.

	//	return;
	//}

	//SelectSlotStates[Index] = true;
	//FHPlayerController->bAutoManageActiveCameraTarget = true;

	if (!IsValidKey(ActorKey))
	{
		RPCComponent->SelectError();
		return;
	}

	PlayerState->SetSkillID(SkillID);
	PlayerState->SetPawnMesh(ActorKey);

	int32 NumPlayers = UGameplayStatics::GetNumPlayerControllers(GetWorld());
	for (int i = 0; i < NumPlayers; i++) {
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), i);
		if (!PC && PC == FHPlayerController)
		{
			continue;
		}
		UFHSelectCharacterRPC* IndividualRPCComponent = Cast<UFHSelectCharacterRPC>(PC->GetComponentByClass(UFHSelectCharacterRPC::StaticClass()));
		if (!IndividualRPCComponent)
		{
			return;
		}
		IndividualRPCComponent->PawnSelected(ActorKey);
	}

	
	GS->SelectedPawnKey.Add(ActorKey);
	GS->DisablePreviewActor(ActorKey);

	FHPlayerController->ShowQuota(GS->LoopCount);
	FHPlayerController->StartVoiceChat();

}

// ================= Logic Test Area ==================

void AFHLobbyGameMode::SetNextRound_Implementation()
{
	LobbyGameState->Reset();
}

void AFHLobbyGameMode::IncreaseDeathCount_Implementation()
{
	LobbyGameState->DeathCount++;
}

void AFHLobbyGameMode::IncreaseMoney_Implementation()
{
	LobbyGameState->AddMoney(1000);
}

void AFHLobbyGameMode::SetGate()
{
	AFHPlayerController* PC = Cast<AFHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->ResetGateButton(SelectedGateInfo);
	}

}

TArray<FString> AFHLobbyGameMode::GetPlayerNames()
{
	TArray<FString> PlayerNameArray{};
	if (GameState)
	{
		for (auto& PS : GameState->PlayerArray)
		{
			PlayerNameArray.Add(PS->GetPlayerName());
		}
	}

	return PlayerNameArray;
}

void AFHLobbyGameMode::OnUpdateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	//IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	//if (Subsystem)
	//{
	//	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	//	if (SessionInterface.IsValid())
	//	{
	//		SessionInterface->ClearOnUpdateSessionCompleteDelegates(this);
	//	}
	//}

	//if (bWasSuccessful)
	//{
	//	UE_LOG(LogLobby, Log, TEXT("Session password updated."));

	//	// TODO: password change callback
	//}
	//else
	//{
	//	UE_LOG(LogLobby, Warning, TEXT("Failed to update session password."));
	//}

	// TODO : 
}
