// Copyright F Rank Hunter.. All Rights Reserved.


#include "Lobby/FHAdvancedFriendsGameInstance.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Title/Controller/TitleController.h"
#include "FRankHunter.h"
#include "Kismet/GameplayStatics.h"
#include "Core/FHPlayerStateBase.h"

DEFINE_LOG_CATEGORY(LogPingServer);
DEFINE_LOG_CATEGORY(LogFHOnlineSession);

void UFHAdvancedFriendsGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if(OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface)
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UFHAdvancedFriendsGameInstance::SessionCreateCompleted);
			//SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UFHAdvancedFriendsGameInstance::FindSessionCompleted);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UFHAdvancedFriendsGameInstance::JoinSessionCompleted);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UFHAdvancedFriendsGameInstance::DestroySessionCompleted);
		}
	}
}


void UFHAdvancedFriendsGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UFHAdvancedFriendsGameInstance::ServerResponded(gameserveritem_t& server)
{
	FString SteamID = FString::Printf(TEXT("steam.%llu:%lu"), server.m_steamID.ConvertToUint64(), ServerPort);
	UE_LOG(LogPingServer, Warning, TEXT("SteamID: %s"), *SteamID);
	PingResponse(true, SteamID);
}

void UFHAdvancedFriendsGameInstance::ServerFailedToRespond()
{
	PingResponse(false, "");
}

void UFHAdvancedFriendsGameInstance::PingServerInfo(FString IpPort)
{
	FString IpString, PortString;
	IpPort.Split(":", &IpString, &PortString);

	UE_LOG(LogPingServer, Warning, TEXT("Query Target Server Ip: %s, Port: %s."), *IpString, *PortString);

	ServerPort = FCString::Atoi(*PortString);
	
	FIPv4Address ip;
	FIPv4Address::Parse(IpString, ip);

	ISteamMatchmakingServers* SteamMatchMakingServers = SteamMatchmakingServers();
	if (SteamMatchMakingServers)
	{
		HServerQuery ServerQuery = SteamMatchMakingServers->PingServer(ip.Value, 27015, this);
	}
	else
	{
		UE_LOG(LogPingServer, Warning, TEXT("SteamMatchMakingServers nullptr."));
	}
}

void UFHAdvancedFriendsGameInstance::PingResponse(bool result, FString steamID)
{
	if (!result)
	{
		UE_LOG(LogPingServer, Warning, TEXT("Ping Failed."));
	}
	else
	{
		ATitleController* TitleController = Cast<ATitleController>(GetWorld()->GetFirstPlayerController());
		if (TitleController)
		{
			TitleController->ClientTravel(steamID, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UFHAdvancedFriendsGameInstance::SessionCreateCompleted(FName SessionName, bool success)
{
	if (IsDedicatedServerInstance())
	{
		UE_LOG(LogTemp, Warning, TEXT("Dedicated Server Session Create Complted"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Session Create Completed."));

	//APlayerController* Controller = GetWorld()->GetFirstPlayerController();
	//if (Controller && Controller->IsLocalController())
	//{
	//	ATitleController* TitleController = Cast<ATitleController>(Controller);
	//	if (TitleController)
	//	{
	//		TitleController->OpenLobby();
	//	}
	//}
}

void UFHAdvancedFriendsGameInstance::FindSession()
{
	IOnlineSessionPtr OnlineSessionPtr = Online::GetSessionInterface(TEXT("Null"));
	if (IsValid(OnlineSession))
	{
		// No OnlineSession.
		return;
	}

	SearchOption->bIsLanQuery = true;

	OnlineSessionPtr->FindSessions(0, SearchOption);
}


void UFHAdvancedFriendsGameInstance::DebugSessionResults(const TArray<FBlueprintSessionResult>& Results)
{
	//for (auto& Result : Results)
	//{
	//	FOnlineSessionSearchResult SearchResult = Result.OnlineResult;
	//	SearchOption->SearchResults.Add(SearchResult);

	//	IOnlineSubsystem* OSS = Online::GetSubsystem(GetWorld());
	//	IOnlineSessionPtr SessionInterfase = OSS->GetSessionInterface();

	//	FString SessionName = SearchResult.Session.SessionSettings.Settings.Find(TEXT("ServerName"))->Data.ToString();

	//	APlayerState* PS = UGameplayStatics::GetPlayerState(GetWorld(), 0);
	//	//SessionInterfase->JoinSession(*PS->GetUniqueId().GetUniqueNetId(), FName(SessionName), SearchResult);
	//	break;
	//}
}

void UFHAdvancedFriendsGameInstance::FindSessionCompleted(bool Result)
{
	if (Result)
	{
		if (SearchOption->SearchResults.IsEmpty())
		{
			UE_LOG(LogFHOnlineSession, Warning, TEXT("[GameInstance|FindSessionCompleted] : Session Not Found."));
			return;
		}
		else
		{
			UE_LOG(LogFHOnlineSession, Warning, TEXT("[GameInstance|FindSessionCompleted] Session Find Success."));
			for (auto& Results : SearchOption->SearchResults)
			{
				//FString SessionId = Results.GetSessionIdStr();
				//IOnlineSessionPtr SessionInterfacePtr = Online::GetSessionInterface(TEXT("Null"));
				//FString SessionName;
				//Results.Session.SessionSettings.Get(TEXT("Name"), SessionName);
				//SessionInterfacePtr->JoinSession(0, *SessionName, Results);
				return;
			}
		}
	}
	else
	{
		UE_LOG(LogFHOnlineSession, Warning, TEXT("[GameInstance|FindSessionCompleted] : Session Find Error."));
	}
}

#include "Core/FHPlayerController.h"

void UFHAdvancedFriendsGameInstance::JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		switch (Result)
		{
		case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
		{
			PRINT_LOG(TEXT("Could not retrieve address."));
			break;
		}
		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
		{
			PRINT_LOG(TEXT("Session Does Not Exist."));
			break;
		}
		case EOnJoinSessionCompleteResult::SessionIsFull:
		{
			PRINT_LOG(TEXT("Session Is Full."));
			break;
		}
		case EOnJoinSessionCompleteResult::UnknownError:
		{
			PRINT_LOG(TEXT("Unknown Error."));
			break;
		}
		}
	}

}


void UFHAdvancedFriendsGameInstance::DestroySessionCompleted(FName SessionName, bool bWasSuccessful)
{
	ReturnToMenu();
}

void UFHAdvancedFriendsGameInstance::ReturnToMenu()
{
	ReturnToMainMenu();
}

/*
	Steam Cloud Test Functions.
*/

#include "Kismet/GameplayStatics.h"
#include "SteamCloudSaveSubsystem.h"
#include "Core/FHPlayerStateBase.h"


void UFHAdvancedFriendsGameInstance::GetPlayerLicense(int32 PlayerID)
{
	APlayerState* PlayerState = UGameplayStatics::GetPlayerState(GetWorld(), PlayerID);
	if (!PlayerState)
	{
		PRINT_LOG(TEXT("Invalid Player Num. %d"), PlayerID);
		return;
	}

	AFHPlayerStateBase* FHPS = Cast<AFHPlayerStateBase>(PlayerState);
	if (FHPS)
	{
		ELicenseRank LicenseRank = FHPS->GetLicenseRank();
		PRINT_LOG(TEXT("Player%d LicenseRank: %s"), PlayerID, *UEnum::GetValueAsString(LicenseRank));
	}
}


void UFHAdvancedFriendsGameInstance::UpgradePlayerLicense(int32 PlayerID)
{
	APlayerState* PlayerState = UGameplayStatics::GetPlayerState(GetWorld(), PlayerID);
	if (!PlayerState)
	{
		PRINT_LOG(TEXT("Invalid Player Num. %d"), PlayerID);
		return;
	}

	AFHPlayerStateBase* FHPS = Cast<AFHPlayerStateBase>(PlayerState);
	if (FHPS)
	{
		FHPS->UpgradeRank();
		PRINT_LOG(TEXT("Player%d LicenseRank: %s"), PlayerID, *UEnum::GetValueAsString(FHPS->GetLicenseRank()));
	}
}


void UFHAdvancedFriendsGameInstance::ResetPlayerLicense(int32 PlayerID)
{
	APlayerState* PlayerState = UGameplayStatics::GetPlayerState(GetWorld(), PlayerID);
	if (!PlayerState)
	{
		PRINT_LOG(TEXT("Invalid Player Num. %d"), PlayerID);
		return;
	}

	AFHPlayerStateBase* FHPS = Cast<AFHPlayerStateBase>(PlayerState);
	if (FHPS)
	{
		FHPS->ResetRank();
		PRINT_LOG(TEXT("Player%d LicenseRank: %s"), PlayerID, *UEnum::GetValueAsString(FHPS->GetLicenseRank()));
	}
}

#include "Core/FHUIManager.h"
#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"
#include "CommonUserWidget.h"

void UFHAdvancedFriendsGameInstance::ShowExplorationResultWidget()
{
	AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	if (UIManager)
	{
		UUserWidget* Widget = UIManager->GetWidget<UUserWidget>(TEXT("ExplorationResult"));
		if (Widget)
		{
			UCommonUserWidget* CUW = Cast<UCommonUserWidget>(Widget);
			CUW->SetVisibility(ESlateVisibility::Visible);
		}
	}
}


void UFHAdvancedFriendsGameInstance::HideExplorationResultWidget()
{
	AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	if (UIManager)
	{
		UUserWidget* Widget = UIManager->GetWidget<UUserWidget>(TEXT("ExplorationResult"));
		if (Widget)
		{
			UCommonUserWidget* CUW = Cast<UCommonUserWidget>(Widget);
			CUW->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

#include "Core/FRankHunterTypes.h"
#include "UI/Gate/FHExplorationResult.h"
#include "Core/FHGateGameStateBase.h"
void UFHAdvancedFriendsGameInstance::PlayExplorationResult()
{
	AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	if (UIManager)
	{
		UFHExplorationResult* Widget = UIManager->GetWidget<UFHExplorationResult>(TEXT("ExplorationResult"));
		if (Widget)
		{
			AFHGateGameStateBase* GameState = Cast<AFHGateGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
			if (GameState)
			{
				Widget->SetExplorationResult(GameState->GetPlayerList().GetPlayerList());
			}

		}
	}
}

#include "Core/FHGateGameModeBase.h"
void UFHAdvancedFriendsGameInstance::GateEnd()
{
	AFHGateGameModeBase* GameMode = Cast<AFHGateGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->GateEnd();
	}
}

#include "UI/Lobby/FHRoundResult.h"
void UFHAdvancedFriendsGameInstance::ShowRoundResult()
{
	AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	if (UIManager)
	{
		UFHRoundResult* Widget = UIManager->GetWidget<UFHRoundResult>(TEXT("RoundResult"));
		if (Widget)
		{
			Widget->Show();
		}
	}
}


void UFHAdvancedFriendsGameInstance::VoiceChatOn()
{
	FName SubsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(SubsystemName);
	if (VoiceInterface.IsValid())
	{
		PRINT_LOG(TEXT("<<<<<<>>>>>>|Start Network Voice.|"))
		VoiceInterface->StartNetworkedVoice(0);
	}
}


void UFHAdvancedFriendsGameInstance::VoiceChatOff()
{
	FName SubsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(SubsystemName);
	if (VoiceInterface.IsValid())
	{
		PRINT_LOG(TEXT("<<<<<<>>>>>>|Stop Network Voice.|"))
		VoiceInterface->StopNetworkedVoice(0);
	}
}

#include "Net/VoiceConfig.h"
void UFHAdvancedFriendsGameInstance::SetRange(float Value)
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		UVOIPTalker* VOIP = Pawn->GetComponentByClass<UVOIPTalker>();
		if (VOIP)
		{
			TObjectPtr<USoundAttenuation> SoundAttenuation = VOIP->Settings.AttenuationSettings;
			if (IsValid(SoundAttenuation))
			{
				SoundAttenuation;
			}
		}
	}
}


void UFHAdvancedFriendsGameInstance::SetAttenuationRange(float Value)
{

}


void UFHAdvancedFriendsGameInstance::DebugVoiceInterface()
{
	//UWorld* WorldContext = GetWorld();
	if (!WorldContext)
	{
		UE_LOG(LogTemp, Error, TEXT("WorldContext is null."));
		return;
	}

	// OnlineSubsystem 확인
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContext->World());
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("OnlineSubsystem is NULL."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("OnlineSubsystem Name: %s"), *Subsystem->GetSubsystemName().ToString());

	// VoiceInterface 확인
	IOnlineVoicePtr VoiceInterface = Subsystem->GetVoiceInterface();
	if (!VoiceInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("VoiceInterface is NULL or Invalid."));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("VoiceInterface is valid."));
	}

	// NetDriver 확인
	UNetDriver* NetDriver = WorldContext->World()->GetNetDriver();
	if (!NetDriver)
	{
		UE_LOG(LogTemp, Warning, TEXT("NetDriver is NULL."));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("NetDriver is valid. NetMode: %d"), (int32)WorldContext->World()->GetNetMode());
	}

	// 플레이어 컨트롤러로도 World 확인
	APlayerController* PC = WorldContext->World()->GetFirstPlayerController();
	if (PC)
	{
		UE_LOG(LogTemp, Log, TEXT("PlayerController exists: %s"), *PC->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController is NULL."));
	}
}


void UFHAdvancedFriendsGameInstance::ShowMouse(bool state)
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetShowMouseCursor(state);
}

void UFHAdvancedFriendsGameInstance::DumpOnlineSession()
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		return;
	}
	DumpNamedSession(SessionInterface->GetNamedSession(NAME_GameSession));
}

void UFHAdvancedFriendsGameInstance::DumpVoiceChat()
{
	FName SubsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(SubsystemName);
	if (VoiceInterface.IsValid())
	{
		FString VoiceDebugString = VoiceInterface->GetVoiceDebugState();
		PRINT_LOG(TEXT("Voice Debug Info:\n%s"), *VoiceDebugString);
	}
	else
	{
		PRINT_LOG(TEXT("Invalid VoiceInterface"));
	}
}

void UFHAdvancedFriendsGameInstance::RegistLocalTalker()
{
	FName SubsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(SubsystemName);
	if (VoiceInterface.IsValid())
	{
		VoiceInterface->RegisterLocalTalker(0);
	}
}

void UFHAdvancedFriendsGameInstance::MuteOnOff(bool IsMute, bool IsSystemMute)
{
	if (IsMute)
	{
		Online::GetVoiceInterface()->MuteRemoteTalker(0, *UGameplayStatics::GetPlayerState(GetWorld(), 0)->GetUniqueId(), IsSystemMute);
	}
	else
	{
		Online::GetVoiceInterface()->UnmuteRemoteTalker(0, *UGameplayStatics::GetPlayerState(GetWorld(), 0)->GetUniqueId(), IsSystemMute);
	}
}

void UFHAdvancedFriendsGameInstance::WriteData()
{
	USteamCloudSaveSubsystem* Subsystem = GetSubsystem<USteamCloudSaveSubsystem>();
	if (Subsystem)
	{
		if (!Subsystem->OnWriteFileCompletedDelegate.IsBound())
		{
			Subsystem->OnWriteFileCompletedDelegate.AddUObject(this, &UFHAdvancedFriendsGameInstance::WriteFileComplete);
		}

		APlayerState* PlayerState = UGameplayStatics::GetPlayerState(GetWorld(), 0);
		TArray<uint8> data = { 0, 1, 2, 3, 4, 5, 6 };
		Subsystem->SaveFileData(*PlayerState->GetUniqueId(), TEXT("FHTestFile"), data);
	}
}

void UFHAdvancedFriendsGameInstance::ReadData()
{
	USteamCloudSaveSubsystem* Subsystem = GetSubsystem<USteamCloudSaveSubsystem>();
	if (Subsystem)
	{
		if (!Subsystem->OnWriteFileCompletedDelegate.IsBound())
		{
			Subsystem->OnWriteFileCompletedDelegate.AddUObject(this, &UFHAdvancedFriendsGameInstance::WriteFileComplete);
		}

		APlayerState* PlayerState = UGameplayStatics::GetPlayerState(GetWorld(), 0);
		Subsystem->SyncSaveFile(*PlayerState->GetUniqueId(), TEXT("FHTestFile"));
	}
}

void UFHAdvancedFriendsGameInstance::DumpFile(const FString& FileName)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineUserCloudPtr OnlineCloud = OnlineSubsystem->GetUserCloudInterface();
		if (OnlineCloud.IsValid())
		{
			APlayerState* PlayerState = UGameplayStatics::GetPlayerState(GetWorld(), 0);
			TArray<uint8> data;
			OnlineCloud->DumpCloudFileState(*PlayerState->GetUniqueId(), FileName);
		}
		else
		{
			PRINT_LOG(TEXT("Invalid OnlineCloud"));
		}
	}
}


void UFHAdvancedFriendsGameInstance::DeleteFile(const FString& FileName)
{

}

#include "Core/FHPropManager.h"
#include "Props/FHKioskBase.h"
void UFHAdvancedFriendsGameInstance::SaveShopList()
{
	AFHPropManager* PropManager = UFHBlueprintFunctionLibrary::GetPropManager(GetWorld());
	if (PropManager)
	{
		AFHKioskBase* Npc = PropManager->GetProp<AFHKioskBase>(TEXT("Kiosk"));
		if (Npc)
		{
			Npc->TrySaveShopList();
		}
	}
}

void UFHAdvancedFriendsGameInstance::LoadShopList()
{
	AFHPropManager* PropManager = UFHBlueprintFunctionLibrary::GetPropManager(GetWorld());
	if (PropManager)
	{
		AFHKioskBase* Npc = PropManager->GetProp<AFHKioskBase>(TEXT("Kiosk"));
		if (Npc)
		{
			Npc->TryLoadShopList();
		}
	}
}

void UFHAdvancedFriendsGameInstance::ReadFileComplete(bool bWasSuccessful, const FUniqueNetId& UserID, const FString& FileName)
{
	if (bWasSuccessful)
	{
		PRINT_LOG(TEXT("File Read Success."));
		IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
		if (OnlineSubsystem)
		{
			IOnlineUserCloudPtr OnlineCloud = OnlineSubsystem->GetUserCloudInterface();
			if (OnlineCloud.IsValid())
			{
				TArray<uint8> data;
				OnlineCloud->GetFileContents(UserID, FileName, data);
				for (uint8 element : data)
				{
					PRINT_LOG(TEXT("%d"), element);
				}
			}
			else
			{
				PRINT_LOG(TEXT("Invalid OnlineCloud"));
			}
		}
	}
	else
	{
		PRINT_LOG(TEXT("File Read Failed."));
	}
}

void UFHAdvancedFriendsGameInstance::WriteFileComplete(bool bWasSuccessful, const FUniqueNetId& UserID, const FString& FileName)
{
	if (bWasSuccessful)
	{
		PRINT_LOG(TEXT("File Write Success."));
	}
	else
	{
		PRINT_LOG(TEXT("File Write Failed."));
	}
}
