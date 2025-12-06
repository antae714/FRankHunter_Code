// Copyright F Rank Hunter.. All Rights Reserved.

#include "TitleController.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemSteam.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"

#include "SocketSubsystemModule.h"

#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"
#include "DataAsset/FHLevelAsset.h"
#include "Core/FHGameInstance.h"

DEFINE_LOG_CATEGORY(LogTitle);
DEFINE_LOG_CATEGORY(LogSessionTest);

ATitleController::ATitleController()
{
}

void ATitleController::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidget)
	{
		MainTitleWidget = CreateWidget<UUserWidget>(this, MainWidget, TEXT("MainTitle"));
		if (MainTitleWidget)
		{
			UE_LOG(LogTitle, Warning, TEXT("MainTitleWidget Create Success. Widget Name: %s"), *MainTitleWidget->GetName());
			MainTitleWidget->AddToViewport();
			bShowMouseCursor = true;

		}
	}
	if (SubWidget)
	{
		SubTitleWidget = CreateWidget<UUserWidget>(this, SubWidget, TEXT("SubTitle"));
		if (SubTitleWidget)
		{
			UE_LOG(LogTitle, Warning, TEXT("SubTitleWidget Create Success. Widget Name: %s"), *SubTitleWidget->GetName());
			SubTitleWidget->AddToViewport();
			SubTitleWidget->SetVisibility(ESlateVisibility::Collapsed);
			bShowMouseCursor = true;
		}
	}
	if (EnterWidget)
	{
		EnterServerWidget = CreateWidget<UUserWidget>(this, EnterWidget, TEXT("EnterWidget"));
		if (EnterServerWidget)
		{
			UE_LOG(LogTitle, Warning, TEXT("SubTitleWidget Create Success. Widget Name: %s"), *EnterServerWidget->GetName());
			EnterServerWidget->AddToViewport();
			EnterServerWidget->SetVisibility(ESlateVisibility::Collapsed);
			bShowMouseCursor = true;
		}
	}

	if (MessageBoxWidgetClass)
	{
		MessageBoxWidget = CreateWidget<UUserWidget>(this, MessageBoxWidgetClass);
		if (MessageBoxWidget)
		{
			MessageBoxWidget->AddToViewport();
		}
	}


	IOnlineSessionPtr OnlineSessionPtr = Online::GetSessionInterface(GetWorld());
	if (OnlineSessionPtr.IsValid())
	{
		OnlineSessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &ATitleController::JoinSessionCompleted);
	}

	//IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	//if (OnlineSubsystem)
	//{
	//	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	//	if (SessionInterface)
	//	{
	//		SessionInterface->OnSessionInviteReceivedDelegates.AddUObject(this, &ATitleController::InviteReceive);
	//		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &ATitleController::JoinSessionCompleted);
	//		SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &ATitleController::InviteAccept);
	//		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &ATitleController::SessionFindCompleted);
	//		SessionInterface->OnFindFriendSessionCompleteDelegates[0].AddUObject(this, &ATitleController::FriendSessionFindCompleted);
	//	}
	//}
}


void ATitleController::OpenLobby()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Travel Map and Host Listen Server."));

	UFHLevelAsset* LevelAssetDefault = UFHBlueprintFunctionLibrary::GetLevelAsset();
	check(LevelAssetDefault)

	UGameplayStatics::OpenLevel(GetWorld(), FName(LevelAssetDefault->GetStartLobby()), true, "listen");

	//ClientTravel("/Game/Main/Level/Map_Lobby?listen", ETravelType::TRAVEL_Absolute);
	//UGameplayStatics::OpenLevel(GetWorld(), "/Game/Main/Level/Map_Lobby?listen", true, "listen");
}

void ATitleController::ChangeWidget(FName WidgetName)
{
	if (WidgetName == "MainTitle")
	{
		MainTitleWidget->SetVisibility(ESlateVisibility::Visible);
		SubTitleWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (WidgetName == "SubTitle")
	{
		MainTitleWidget->SetVisibility(ESlateVisibility::Collapsed);
		SubTitleWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ATitleController::VisibleEnterWidget()
{
	//UGameplayStatics::OpenLevel(GetWorld(), "/Game/Sample/SampleMap/Map_Sample", true, "listen");

	UE_LOG(LogTitle, Warning, TEXT("EnterWidget Open."));

	EnterServerWidget->SetVisibility(ESlateVisibility::Visible);

	/*UE_LOG(LogSessionTest, Warning, TEXT("Finding Session..."));
	PrintMessage(FString::Printf(TEXT("Finding Session...")));


	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SearchResult = MakeShareable(new FOnlineSessionSearch());
			SearchResult->bIsLanQuery = false;
			SearchResult->MaxSearchResults = 10;
			SearchResult->QuerySettings.Set(TEXT("FHTest"), FString("FH"), EOnlineComparisonOp::Equals);
			SearchResult->QuerySettings.Set(TEXT("FHTest1"), FString("FH1"), EOnlineComparisonOp::Equals);
			SearchResult->QuerySettings.Set(TEXT("FHTest"), FString("FHMap"), EOnlineComparisonOp::Equals);
			SearchResult->QuerySettings.Set(TEXT("A"), FString("AA"), EOnlineComparisonOp::Equals);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &ATitleController::SessionFindCompleted);

			UE_LOG(LogSessionTest, Warning, TEXT("Start Finding Session..."));
			PrintMessage(FString::Printf(TEXT("Start Finding Session...")));
			SessionInterface->FindSessions(0, SearchResult.ToSharedRef());
		}
	}
	else
	{
		PrintMessage(FString::Printf(TEXT("No OnlineSubsystem.")));
	}*/

}

void ATitleController::EnterToServer(FString Ip)
{
	UE_LOG(LogSessionTest, Warning, TEXT("Start Travel To Server. Ip: %s"), *Ip);
	//FString::Printf(TEXT("Start Travel To Server. Ip: %s"), *Ip);
	ClientTravel(Ip, ETravelType::TRAVEL_Absolute);
}

void ATitleController::ShowGateTooltip(const FString& GateName)
{
	if (SubTitleWidget->IsVisible())
	{
		return;
	}

}

void ATitleController::HideGateTooltip()
{
	if (SubTitleWidget)
	{
		SubTitleWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

UUserWidget* ATitleController::GetMessageBoxWidget()
{
	return MessageBoxWidget;
}

void ATitleController::JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSessionPtr SessionInterfacePtr = Online::GetSessionInterface(GetWorld());
	FString URL;
	SessionInterfacePtr->GetResolvedConnectString(SessionName, URL);
	URL += FString::Printf(TEXT("?NickName=%s"), *NickName);
	URL += FString::Printf(TEXT("?Password=%s"), *Password);
	
	ClientTravel(URL, ETravelType::TRAVEL_Absolute);
}

